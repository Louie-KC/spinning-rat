#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define FILE_LINE_BUFFER_SIZE 256
// #define FILE_BUFFER_SIZE 4096
#define FILE_BUFFER_SIZE 65536 * 128

#define UTIL_SUCCESS 1
#define UTIL_FAILURE 0

#define UTIL_PROCESS_CENTRE_MODEL 0b10000000
#define UTIL_PROCESS_SCALE_MODEL  0b01000000

struct bounding_box {
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float z_min;
    float z_max;
};

int read_file_source(const char* file_path, char* dest_buffer) {
    FILE *fp;
    int status = UTIL_FAILURE;

    dest_buffer[0] = '\0';
    fp = fopen(file_path, "r");

    if (fp) {
        char line[FILE_LINE_BUFFER_SIZE];
        unsigned int pos = 0;
        while (fgets(line, FILE_LINE_BUFFER_SIZE, fp) != NULL) {
            unsigned int line_len = strnlen(line, FILE_LINE_BUFFER_SIZE);
            strcpy(dest_buffer + pos, line);
            pos += line_len;
        }
        fgets(dest_buffer, FILE_BUFFER_SIZE, fp);
        status = UTIL_SUCCESS;
        fclose(fp);
    }

    return status;
}

struct bounding_box model_bounding_box(float *vertex_buffer, int n_vertices) {
    struct bounding_box bounds = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    for (int i = 0; i < n_vertices / 3; i++) {
        float x = vertex_buffer[i * 3];
        float y = vertex_buffer[i * 3 + 1];
        float z = vertex_buffer[i * 3 + 2];

        if (x < bounds.x_min) {
            bounds.x_min = x;
        }
        if (x > bounds.x_max) {
            bounds.x_max = x;
        }
        if (y < bounds.y_min) {
            bounds.y_min = y;
        }
        if (y > bounds.y_max) {
            bounds.y_max = y;
        }
        if (z < bounds.z_min) {
            bounds.z_min = z;
        }
        if (z > bounds.z_max) {
            bounds.z_max = z;
        }
    }

    return bounds;
}

void centre_model(float *vertex_buffer, int n_vertices, struct bounding_box bounds) {
    float x_offset = (bounds.x_min + bounds.x_max) / 2.0f;
    float y_offset = (bounds.y_min + bounds.y_max) / 2.0f;
    float z_offset = (bounds.z_min + bounds.z_max) / 2.0f;

    // printf("x offset: %f\ny_offset: %f\nz_offset: %f\n", x_offset, y_offset, z_offset);

    for (int i = 0; i < n_vertices / 3; i++) {
        vertex_buffer[i * 3]     -= x_offset;
        vertex_buffer[i * 3 + 1] -= y_offset;
        vertex_buffer[i * 3 + 2] -= z_offset;
    }
}

// Throw away original units (metres, feet, etc) and normalise to unit length [-1...1]
void normalise_model_scale(float* vertex_buffer, int n_vertices, struct bounding_box bounds) {
    float x_scale = 1.0f / (bounds.x_max - bounds.x_min);
    float y_scale = 1.0f / (bounds.y_max - bounds.y_min);
    float z_scale = 1.0f / (bounds.z_max - bounds.z_min);

    float scale = x_scale;
    if (scale < y_scale) {
        scale = y_scale;
    }
    if (scale < z_scale) {
        scale = z_scale;
    }

    for (int i = 0; i < n_vertices / 3; i++) {
        vertex_buffer[i * 3]     *= scale;
        vertex_buffer[i * 3 + 1] *= scale;
        vertex_buffer[i * 3 + 2] *= scale;
    }
}

// TODO: Write own face triangulation code when faces are of an order greater than 3
// Note: ASSIMP triangulation appears to only work when a single models faces
//       are ALL of the same order.
int load_model(const char* file_path,
               float** vertex_buffer,
               unsigned int** index_buffer,
               int *n_vertices,
               int *n_indices,
               unsigned int flags) {
    int status = UTIL_FAILURE;

    unsigned int import_flags = aiProcessPreset_TargetRealtime_Fast;
    const C_STRUCT aiScene *scene = aiImportFile(file_path, import_flags);

    if (scene && scene->mFlags != AI_SCENE_FLAGS_INCOMPLETE) {
        C_STRUCT aiMesh mesh;
        unsigned int mi;  // mesh index
        unsigned int vi;  // vertex index
        unsigned int fi;  // face index

        // At least 1 mesh present with AI_SCENE_FLAGS_INCOMPLETE not being set

        // Calculate and allocate required memory for model. Each face is made
        // up of 3 faces with the aiProcess_Triangulate flag.
        *n_vertices = 3 * scene->mMeshes[0]->mNumVertices;
        *n_indices  = 3 * scene->mMeshes[0]->mNumFaces;
        for (mi = 1; mi < scene->mNumMeshes; mi++) {
            *n_vertices += 3 * scene->mMeshes[mi]->mNumVertices;
            *n_indices  += 3 * scene->mMeshes[mi]->mNumFaces;
        }
        *vertex_buffer = malloc(*n_vertices * sizeof(float));
        *index_buffer  = malloc(*n_indices * sizeof(unsigned int));

        vi = 0;
        fi = 0;
        for (mi = 0; mi < scene->mNumMeshes; mi++) {
            mesh = *scene->mMeshes[mi];

            // vertices
            unsigned int cur_vi = 0;
            while (cur_vi < mesh.mNumVertices) {
                (*vertex_buffer)[vi * 3]     = mesh.mVertices[cur_vi].x;
                (*vertex_buffer)[vi * 3 + 1] = mesh.mVertices[cur_vi].y;
                (*vertex_buffer)[vi * 3 + 2] = mesh.mVertices[cur_vi].z;
                cur_vi++;
                vi++;
            }

            // indices
            // unsigned int max_mNumIndices = 0;
            unsigned int cur_fi = 0;
            while (cur_fi < mesh.mNumFaces) {
                const struct aiFace face = mesh.mFaces[cur_fi];

                (*index_buffer)[fi * 3]     = face.mIndices[0];
                (*index_buffer)[fi * 3 + 1] = face.mIndices[1];
                (*index_buffer)[fi * 3 + 2] = face.mIndices[2];

                cur_fi++;
                fi++;

                // if (max_mNumIndices < face.mNumIndices) {
                //     max_mNumIndices = face.mNumIndices;
                // }
            }
        }

        // process flags
        struct bounding_box bounds;
        if (flags) {
            bounds = model_bounding_box(*vertex_buffer, *n_vertices);
        }
        if (flags & UTIL_PROCESS_CENTRE_MODEL) {
            centre_model(*vertex_buffer, *n_vertices, bounds);
        }
        if (flags & UTIL_PROCESS_SCALE_MODEL) {
            normalise_model_scale(*vertex_buffer, *n_vertices, bounds);
        }

        aiReleaseImport(scene);
        status = UTIL_SUCCESS;
    } else {
        printf("load_model failed\nReason: %s\n", aiGetErrorString());
    }

    return status;
}