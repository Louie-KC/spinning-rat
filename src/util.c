#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define FILE_LINE_BUFFER_SIZE 256
#define FILE_BUFFER_SIZE 4096

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

// TODO: Load more than just first mesh. Files can be made up of multiple meshes.
int load_model(const char* file_path,
               float** vertex_buffer,
               unsigned int** index_buffer,
               int *n_vertices,
               int *n_indices) {
    int status = UTIL_FAILURE;

    unsigned int flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;
    const C_STRUCT aiScene *scene = aiImportFile(file_path, flags);

    if (scene && scene->mFlags != AI_SCENE_FLAGS_INCOMPLETE) {
        // at least 1 mesh with AI_SCENE_FLAGS_INCOMPLETE not being set
        C_STRUCT aiMesh mesh = *scene->mMeshes[0];

        // assign buffers memory
        // each face of the mesh is made up of 3 points due to aiProcess_Triangulate
        *n_vertices = 3 * mesh.mNumVertices;
        *n_indices  = 3 * mesh.mNumFaces;
        *vertex_buffer = malloc(*n_vertices * sizeof(float));
        *index_buffer  = malloc(*n_indices * sizeof(unsigned int));

        unsigned int i = 0;
        for (i = 0; i < mesh.mNumVertices; i++) {
            // printf("%f %f %f\n", mesh.mVertices[i].x,
            //    mesh.mVertices[i].y, mesh.mVertices[i].z);
            (*vertex_buffer)[i * 3]     = mesh.mVertices[i].x;
            (*vertex_buffer)[i * 3 + 1] = mesh.mVertices[i].y;
            (*vertex_buffer)[i * 3 + 2] = mesh.mVertices[i].z;
        }

        for (i = 0; i < mesh.mNumFaces; i++) {
            const struct aiFace face = mesh.mFaces[i];
            for (int fi = 0; fi < face.mNumIndices; fi++) {
                // printf("%d ", face.mIndices[fi]);
                (*index_buffer)[i * face.mNumIndices + fi] = face.mIndices[fi];
            }
        }

        aiReleaseImport(scene);
        status = UTIL_SUCCESS;
    } else {
        printf("load_model failed\nReason: %s\n", aiGetErrorString());
    }

    return status;
}