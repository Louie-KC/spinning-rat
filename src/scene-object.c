#include "scene-object.h"

void scn_obj_init(scene_object* scn_obj) {
    // OpenGL buffers
    glGenVertexArrays(1, &scn_obj->VAO);
    glGenBuffers(1, &scn_obj->VBO);
    glGenBuffers(1, &scn_obj->EBO);

    glBindVertexArray(scn_obj->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, scn_obj->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * scn_obj->vertex_buffer_len,
                 scn_obj->vertex_buffer, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scn_obj->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * scn_obj->index_buffer_len,
                 scn_obj->index_buffer, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);  // release VAO

    scn_obj->model_matrix = mat4_identity();
    scn_obj->parent = NULL;
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