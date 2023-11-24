#include "util.h"

int read_file_source(const char* file_path, char* dest_buffer, int buffer_size) {
    FILE *fp;
    int status = UTIL_FAILURE;

    dest_buffer[0] = '\0';
    fp = fopen(file_path, "r");

    if (fp) {
        char line[FILE_LINE_BUFFER_SIZE];
        unsigned int pos = 0;
        while (fgets(line, FILE_LINE_BUFFER_SIZE, fp) != NULL) {
            unsigned int line_len = strnlen(line, FILE_LINE_BUFFER_SIZE);
            if (line_len + pos >= buffer_size) {
                // Prevent buffer overflow on strcpy
#ifdef UTIL_DEBUG
                printf("read_file_source '%s' exceeds buffer size %d\n", file_path, buffer_size);
#endif
                break;
            }
            strcpy(dest_buffer + pos, line);
            pos += line_len;
        }
        dest_buffer[pos] = '\0';  // set last char to null terminating char
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
               float** normals_buffer,
               unsigned int *n_vertices,
               unsigned int *n_indices,
               unsigned int flags,
               int flip_winding_order) {
#ifdef UTIL_DEBUG
    printf("load_model file: %s\n", file_path);
#endif
    int status = UTIL_FAILURE;

    unsigned int import_flags = aiProcessPreset_TargetRealtime_Fast;
    // Check for flip winding order flag
    if (flip_winding_order) {
        import_flags = import_flags | aiProcess_FlipWindingOrder;
    }
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
        *vertex_buffer  = malloc(*n_vertices * sizeof(float));
        *index_buffer   = malloc(*n_indices * sizeof(unsigned int));
        *normals_buffer = malloc(*n_vertices * sizeof(float));

        vi = 0;
        fi = 0;
        for (mi = 0; mi < scene->mNumMeshes; mi++) {
            mesh = *scene->mMeshes[mi];

            // vertices & normals
            unsigned int cur_vi = 0;
            while (cur_vi < mesh.mNumVertices) {
                (*vertex_buffer)[vi * 3]     = mesh.mVertices[cur_vi].x;
                (*vertex_buffer)[vi * 3 + 1] = mesh.mVertices[cur_vi].y;
                (*vertex_buffer)[vi * 3 + 2] = mesh.mVertices[cur_vi].z;

                (*normals_buffer)[vi * 3]     = mesh.mNormals[cur_vi].x;
                (*normals_buffer)[vi * 3 + 1] = mesh.mNormals[cur_vi].y;
                (*normals_buffer)[vi * 3 + 2] = mesh.mNormals[cur_vi].z;
                
                cur_vi++;
                vi++;
            }

            // indices
#ifdef UTIL_DEBUG
            unsigned int max_mNumIndices = 0;
#endif
            unsigned int cur_fi = 0;
            while (cur_fi < mesh.mNumFaces) {
                const struct aiFace face = mesh.mFaces[cur_fi];

                (*index_buffer)[fi * 3]     = face.mIndices[0];
                (*index_buffer)[fi * 3 + 1] = face.mIndices[1];
                (*index_buffer)[fi * 3 + 2] = face.mIndices[2];

                cur_fi++;
                fi++;
#ifdef UTIL_DEBUG
                if (max_mNumIndices < face.mNumIndices) {
                    max_mNumIndices = face.mNumIndices;
                }
#endif
            }
#ifdef UTIL_DEBUG
            printf("max_mNumIndices: %d\n", max_mNumIndices);
#endif
        }

        // process flags (except for flip winding order)
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

#ifdef UTIL_DEBUG
        printf("n_vertices: %d\nn_indices:  %d\n", *n_vertices, *n_indices);
        for (int i = 0; i < *n_vertices; ++i) {
            printf("%f ", (*vertex_buffer)[i]);
            if ((i + 1) % 3 == 0) {
                printf("\n");
            }
        }
        for (int i = 0; i < *n_indices; ++i) {
            printf("%d ", (*index_buffer)[i]);
            if ((i + 1) % 3 == 0) {
                printf("\n");
            }
        }
        for (int i = 0; i < *n_vertices; ++i) {
            printf("%f ", (*normals_buffer)[i]);
            if ((i + 1) % 3 == 0) {
                printf("\n");
            }
        }
        printf("\n");
#endif
        aiReleaseImport(scene);
        status = UTIL_SUCCESS;
    } else {
        printf("load_model failed\nReason: %s\n", aiGetErrorString());
    }

    return status;
}