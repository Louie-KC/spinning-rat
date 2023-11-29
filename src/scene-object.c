#include "scene-object.h"

// TODO: Write own face triangulation code when faces are of an order greater than 3
// Note: ASSIMP triangulation appears to only work when a single models faces
//       are ALL of the same order.
struct scene_object scn_obj_load(const char* file_path, unsigned int flags) {
    scene_object object;
    object.model_matrix = mat4_identity();
    object.parent = NULL;

    unsigned int ai_import_flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices
                                   | aiProcess_GenSmoothNormals | aiProcess_ImproveCacheLocality;
    // winding order flip check
    if (flags & SCN_OBJ_IMPORT_FLIP_WINDING_ORDER) {
        ai_import_flags |= aiProcess_FlipWindingOrder;
    }
    const C_STRUCT aiScene *scene = aiImportFile(file_path, ai_import_flags);

    if (scene && scene->mFlags != AI_SCENE_FLAGS_INCOMPLETE) {
        C_STRUCT aiMesh mesh;
        unsigned int mi;  // mesh index
        unsigned int vi;  // vertex index
        unsigned int fi;  // face index

        // At least 1 mesh present with AI_SCENE_FLAGS_INCOMPLETE not being set

        // Calculate and allocate required memory for model. Each face is made
        // up of 3 faces with the aiProcess_Triangulate flag.
        object.vertex_buffer_len = 3 * scene->mMeshes[0]->mNumVertices;
        object.index_buffer_len  = 3 * scene->mMeshes[0]->mNumFaces;
        for (mi = 1; mi < scene->mNumMeshes; mi++) {
            object.vertex_buffer_len += 3 * scene->mMeshes[mi]->mNumVertices;
            object.index_buffer_len  += 3 * scene->mMeshes[mi]->mNumFaces;
        }
        object.vertex_buffer  = malloc(object.vertex_buffer_len * sizeof(float));
        object.normals_buffer = malloc(object.vertex_buffer_len * sizeof(float));
        object.index_buffer   = malloc(object.index_buffer_len  * sizeof(unsigned int));

        vi = 0;
        fi = 0;
        for (mi = 0; mi < scene->mNumMeshes; mi++) {
            mesh = *scene->mMeshes[mi];

            // vertices & normals & texture coords
            unsigned int cur_vi = 0;
            while (cur_vi < mesh.mNumVertices) {
                object.vertex_buffer[vi * 3]     = mesh.mVertices[cur_vi].x;
                object.vertex_buffer[vi * 3 + 1] = mesh.mVertices[cur_vi].y;
                object.vertex_buffer[vi * 3 + 2] = mesh.mVertices[cur_vi].z;


                object.normals_buffer[vi * 3]     = mesh.mNormals[cur_vi].x;
                object.normals_buffer[vi * 3 + 1] = mesh.mNormals[cur_vi].y;
                object.normals_buffer[vi * 3 + 2] = mesh.mNormals[cur_vi].z;
                
                cur_vi++;
                vi++;
            }

            // indices
#ifdef SCN_OBJ_DEBUG
            unsigned int max_mNumIndices = 0;
#endif
            unsigned int cur_fi = 0;
            while (cur_fi < mesh.mNumFaces) {
                const struct aiFace face = mesh.mFaces[cur_fi];

                object.index_buffer[fi * 3]     = face.mIndices[0];
                object.index_buffer[fi * 3 + 1] = face.mIndices[1];
                object.index_buffer[fi * 3 + 2] = face.mIndices[2];

                cur_fi++;
                fi++;
#ifdef SCN_OBJ_DEBUG
                if (max_mNumIndices < face.mNumIndices) {
                    max_mNumIndices = face.mNumIndices;
                }
#endif
            }
#ifdef SCN_OBJ_DEBUG
            printf("max_mNumIndices: %d\n", max_mNumIndices);
#endif
        }

        // process flags (except for flip winding order)
        struct bounding_box bounds;
        if (flags & (SCN_OBJ_IMPORT_CENTRED | SCN_OBJ_IMPORT_UNIT_RESCALE)) {
            bounds = bounding_box(object.vertex_buffer, object.vertex_buffer_len);
        }
        if (flags & SCN_OBJ_IMPORT_CENTRED) {
            scn_obj_centre_model(object.vertex_buffer, object.vertex_buffer_len, bounds);
        }
        if (flags & SCN_OBJ_IMPORT_UNIT_RESCALE) {
            normalise_model_scale(object.vertex_buffer, object.vertex_buffer_len, bounds);
        }
        if (flags & SCN_OBJ_IMPORT_FLIP_NORMALS) {
            invert_float_buffer(object.normals_buffer, object.vertex_buffer_len);
        }

#ifdef SCN_OBJ_DEBUG
        printf("n_vertices: %d\nn_indices:  %d\n", object.vertex_buffer_len, object.index_buffer_len);
        for (int i = 0; i < object.vertex_buffer_len; ++i) {
            printf("%f ", (object.vertex_buffer)[i]);
            if ((i + 1) % 3 == 0) {
                printf("\n");
            }
        }
        for (int i = 0; i < object.index_buffer_len; ++i) {
            printf("%d ", (object.index_buffer)[i]);
            if ((i + 1) % 3 == 0) {
                printf("\n");
            }
        }
        for (int i = 0; i < object.vertex_buffer_len; ++i) {
            printf("%f ", (object.normals_buffer)[i]);
            if ((i + 1) % 3 == 0) {
                printf("\n");
            }
        }
        printf("\n");
#endif
        aiReleaseImport(scene);
        scn_obj_set_buffers(&object);
    } else {
        printf("load_model failed\nReason: %s\n", aiGetErrorString());
    }
    return object;
}

void scn_obj_centre_model(float *vertex_buffer, int n_vertices, struct bounding_box bounds) {
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

void scn_obj_set_buffers(scene_object* scn_obj) {
    // Clear buffers if present
    glDeleteVertexArrays(1, &scn_obj->VAO);
    glDeleteBuffers(1, &scn_obj->VBO);
    glDeleteBuffers(1, &scn_obj->EBO);

    // Create buffers
    glGenVertexArrays(1, &scn_obj->VAO);
    glGenBuffers(1, &scn_obj->VBO);
    glGenBuffers(1, &scn_obj->EBO);

    glBindVertexArray(scn_obj->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, scn_obj->VBO);
    int vertex_buffer_size = sizeof(float) * scn_obj->vertex_buffer_len;
    // allocate buffer for BOTH vertices and normals. Then load first half with vertices
    // and other half with normals (batching).
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * 2, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_buffer_size, scn_obj->vertex_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, vertex_buffer_size, vertex_buffer_size, scn_obj->normals_buffer);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scn_obj->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * scn_obj->index_buffer_len,
                 scn_obj->index_buffer, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (vertex_buffer_size+0L));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);  // release VAO
}

mat4 scn_obj_mvp(scene_object* scn_obj, mat4 view, mat4 proj) {
    mat4 result = proj;
    mat4_multiply(&result, &view);
    mat4_multiply(&result, &scn_obj->model_matrix);
    return result;
}

void scn_obj_clean(scene_object* scn_obj) {
    if (scn_obj == NULL) {
        return;
    }

    // Clear vertex and index buffers
    if (!scn_obj->vertex_buffer) {
        free(scn_obj->vertex_buffer);
    }
    if (!scn_obj->index_buffer) {
        free(scn_obj->index_buffer);
    }

    // Clear OpenGL buffers
    glDeleteVertexArrays(1, &scn_obj->VAO);
    glDeleteBuffers(1, &scn_obj->VBO);
    glDeleteBuffers(1, &scn_obj->EBO);
}