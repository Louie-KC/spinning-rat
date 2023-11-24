#include "scene-object.h"

void scn_obj_init(scene_object* scn_obj) {
    // OpenGL buffers
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

    scn_obj->model_matrix = mat4_identity();
    scn_obj->parent = NULL;
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