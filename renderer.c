#include <GL/glew.h>
#include "renderer.h"

void render_objs(render_obj_t* render_obj, int count, camera_t* camera) {
  for (int i = 0; i < count; i++) {
    glUseProgram(render_obj->program);
    GLuint mat_location = glGetUniformLocation(render_obj->program, "model");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, render_obj->transform.buf);
    mat_location = glGetUniformLocation(render_obj->program, "view");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, camera->view.buf);
    mat_location = glGetUniformLocation(render_obj->program, "projection");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, camera->projection.buf);
    glBindVertexArray(render_obj->vao);
    glDrawArrays(GL_TRIANGLES, 0, render_obj->vert_count);
  }
}
