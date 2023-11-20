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

// TODO: Write own face triangulation code when faces are of an order greater than 3
// Note: ASSIMP triangulation appears to only work when a single models faces
//       are ALL of the same order.
int load_model(const char* file_path,
               float** vertex_buffer,
               unsigned int** index_buffer,
               int *n_vertices,
               int *n_indices) {
    int status = UTIL_FAILURE;

    unsigned int flags = aiProcessPreset_TargetRealtime_Fast;
    const C_STRUCT aiScene *scene = aiImportFile(file_path, flags);

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

        aiReleaseImport(scene);
        status = UTIL_SUCCESS;
    } else {
        printf("load_model failed\nReason: %s\n", aiGetErrorString());
    }

    return status;
}