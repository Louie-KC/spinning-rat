#include "scene-object.h"

// TODO: Write own face triangulation code when faces are of an order greater than 3
// Note: ASSIMP triangulation appears to only work when a single models faces
//       are ALL of the same order.
struct scene_object scn_obj_load(const char* file_path, unsigned int flags) {
#ifdef SCN_OBJ_DEBUG
    printf("\nscn_obj_load: %s\n", file_path);
#endif

    scene_object object;
    object.texture = 0;  // 0 is the (silent) ignore value for OpenGL
    object.shader_prog = 0;
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

        // Calculate and allocated the required memory for the models mesh.
        object.vertex_buffer_len = scene->mMeshes[0]->mNumVertices;
        object.index_buffer_len  = scene->mMeshes[0]->mNumFaces;
        for (mi = 1; mi < scene->mNumMeshes; mi++) {
            object.vertex_buffer_len += scene->mMeshes[mi]->mNumVertices;
            object.index_buffer_len  += scene->mMeshes[mi]->mNumFaces;
        }
        object.index_buffer_len *= 3;  // 3 indices are used to make up a face

        object.vertex_buffer    = malloc(object.vertex_buffer_len * sizeof(vec3));
        object.normal_buffer    = malloc(object.vertex_buffer_len * sizeof(vec3));
        object.tex_coord_buffer = malloc(object.vertex_buffer_len * sizeof(vec2));
        object.index_buffer     = malloc(object.index_buffer_len  * sizeof(face));

        vi = 0;
        fi = 0;
        for (mi = 0; mi < scene->mNumMeshes; mi++) {
            mesh = *scene->mMeshes[mi];

            // vertices & normals
            unsigned int cur_vi = 0;
            while (cur_vi < mesh.mNumVertices) {
                object.vertex_buffer[vi].x = mesh.mVertices[cur_vi].x;
                object.vertex_buffer[vi].y = mesh.mVertices[cur_vi].y;
                object.vertex_buffer[vi].z = mesh.mVertices[cur_vi].z;

                object.normal_buffer[vi].x = mesh.mNormals[cur_vi].x;
                object.normal_buffer[vi].y = mesh.mNormals[cur_vi].y;
                object.normal_buffer[vi].z = mesh.mNormals[cur_vi].z;

                if (mesh.mTextureCoords[0]) {
                    object.tex_coord_buffer[vi].x = mesh.mTextureCoords[0][cur_vi].x;
                    object.tex_coord_buffer[vi].y = mesh.mTextureCoords[0][cur_vi].y;
                }
                
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
                object.index_buffer[fi].indices[0] = face.mIndices[0];
                object.index_buffer[fi].indices[1] = face.mIndices[1];
                object.index_buffer[fi].indices[2] = face.mIndices[2];

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
            // vec3 is contiguous in memory, len * 3 to cover entire vec3 buffer
            bounds = bounding_box((float*) object.vertex_buffer, object.vertex_buffer_len * 3);
        }
        if (flags & SCN_OBJ_IMPORT_CENTRED) {
            scn_obj_centre_model(object.vertex_buffer, object.vertex_buffer_len, bounds);
        }
        if (flags & SCN_OBJ_IMPORT_UNIT_RESCALE) {
            normalise_model_scale(object.vertex_buffer, object.vertex_buffer_len, bounds);
        }
        if (flags & SCN_OBJ_IMPORT_FLIP_NORMALS) {
            // vec3 is contiguous in memory, len * 3 to cover entire vec3 buffer
            invert_float_buffer((float*) object.normal_buffer, object.vertex_buffer_len * 3);
        }

#ifdef SCN_OBJ_DEBUG
        printf("n_vertices: %d\nn_indices:  %d\n", object.vertex_buffer_len, object.index_buffer_len);
        for (int i = 0; i < object.vertex_buffer_len; ++i) {
            printf("v %f %f %f\n", object.vertex_buffer[i].x,
                    object.vertex_buffer[i].y, object.vertex_buffer[i].z);
        }
        for (int i = 0; i < object.vertex_buffer_len; ++i) {
            printf("vn %f %f %f\n", object.normal_buffer[i].x,
                    object.normal_buffer[i].y, object.normal_buffer[i].z);
        }
        printf("texture coords for face\n");
        for (int i = 0; i < object.vertex_buffer_len; ++i) {
            printf("%f, %f\n", object.tex_coord_buffer[i].x, object.tex_coord_buffer[i].y);
        }
        for (int i = 0; i < object.index_buffer_len / 3; ++i) {
            printf("f %u %u %u\n", object.index_buffer[i].indices[0],
                    object.index_buffer[i].indices[1], object.index_buffer[i].indices[2]);
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

void scn_obj_centre_model(vec3 *vertex_buffer, int n_vertices, struct bounding_box bounds) {
    float x_offset = (bounds.x_min + bounds.x_max) / 2.0f;
    float y_offset = (bounds.y_min + bounds.y_max) / 2.0f;
    float z_offset = (bounds.z_min + bounds.z_max) / 2.0f;

    // printf("x offset: %f\ny_offset: %f\nz_offset: %f\n", x_offset, y_offset, z_offset);

    for (int i = 0; i < n_vertices; i++) {
        vertex_buffer[i].x -= x_offset;
        vertex_buffer[i].y -= y_offset;
        vertex_buffer[i].z -= z_offset;
    }
}

void normalise_model_scale(vec3* vertex_buffer, int n_vertices, struct bounding_box bounds) {
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

    for (int i = 0; i < n_vertices; i++) {
        vertex_buffer[i].x *= scale;
        vertex_buffer[i].y *= scale;
        vertex_buffer[i].z *= scale;
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
    int vertex_buffer_size = sizeof(vec3) * scn_obj->vertex_buffer_len;
    int texcoord_buffer_size = sizeof(vec2) * scn_obj->vertex_buffer_len;
    // allocate buffer for BOTH vertices and normals. Then load first half with vertices
    // and second half with normals (batching).
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * 2 + texcoord_buffer_size,
                NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_buffer_size, scn_obj->vertex_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, vertex_buffer_size, vertex_buffer_size, scn_obj->normal_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, vertex_buffer_size * 2,
                    texcoord_buffer_size, scn_obj->tex_coord_buffer);

#ifdef SCN_OBJ_DEBUG
    vec3 *buffer = malloc(vertex_buffer_size * 2);
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertex_buffer_size * 2, buffer);
    printf("From gl buffer\n");
    for (int i = 0; i < scn_obj->vertex_buffer_len; i++) {
        printf("v %f %f %f\n", buffer[i].x, buffer[i].y, buffer[i].z);
    }
    for (int i = scn_obj->vertex_buffer_len; i < scn_obj->vertex_buffer_len * 2; i++) {
        printf("vn %f %f %f\n", buffer[i].x, buffer[i].y, buffer[i].z);
    }
    free(buffer);
#endif

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*) (vertex_buffer_size+0L));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*) (vertex_buffer_size*2L));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scn_obj->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(face) * scn_obj->index_buffer_len,
                 scn_obj->index_buffer, GL_STATIC_DRAW);

#ifdef SCN_OBJ_DEBUG
    unsigned int idx_buffer_byte_size = sizeof(face) * scn_obj->index_buffer_len;
    face *idx_debug_buffer = malloc(idx_buffer_byte_size);
    glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, idx_buffer_byte_size, idx_debug_buffer);
    for (int i = 0; i < scn_obj->index_buffer_len / 3; i++) {
        printf("f %u %u %u\n", idx_debug_buffer[i].indices[0],
                idx_debug_buffer[i].indices[1], idx_debug_buffer[i].indices[2]);
    }
    free(idx_debug_buffer);
#endif

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);  // release VAO
}

void scn_obj_load_texture(scene_object* scn_obj, char* texture_path) {
    glDeleteTextures(1, &scn_obj->texture);
#ifdef SCN_OBJ_DEBUG
    printf("scn_obj_load_texture texture_path: %s\n", texture_path);
#endif
    glGenTextures(1, &scn_obj->texture);
    glBindTexture(GL_TEXTURE_2D, scn_obj->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, n_channels;
    unsigned char *data = stbi_load(texture_path, &width, &height, &n_channels, 0);
#ifdef SCN_OBJ_DEBUG
    printf("width: %d, height: %d, n_channels: %d\n", width, height, n_channels);
#endif
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Failed to load texture: %s\n", texture_path);
    }

    stbi_image_free(data);
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

    // Clear mesh buffers
    if (!scn_obj->vertex_buffer) {
        free(scn_obj->vertex_buffer);
    }
    if (!scn_obj->normal_buffer) {
        free(scn_obj->normal_buffer);
    }
    if (!scn_obj->tex_coord_buffer) {
        free(scn_obj->tex_coord_buffer);
    }
    if (!scn_obj->index_buffer) {
        free(scn_obj->index_buffer);
    }

    glDeleteProgram(scn_obj->shader_prog);

    // Clear OpenGL buffers
    glDeleteVertexArrays(1, &scn_obj->VAO);
    glDeleteBuffers(1, &scn_obj->VBO);
    glDeleteBuffers(1, &scn_obj->EBO);
    glDeleteTextures(1, &scn_obj->texture);
}