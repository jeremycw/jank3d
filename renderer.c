#include <GL/glew.h>
#include "renderer.h"

void render_objs(render_obj_t* render_obj, int count, renderer_t* renderer) {
  for (int i = 0; i < count; i++) {
    glUseProgram(renderer->program);
    GLuint mat_location = glGetUniformLocation(renderer->program, "model");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, render_obj->transform.buf);
    mat_location = glGetUniformLocation(renderer->program, "view");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, renderer->camera.view.buf);
    mat_location = glGetUniformLocation(renderer->program, "projection");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, renderer->camera.projection.buf);
    glBindVertexArray(render_obj->vao);
    glDrawArrays(GL_TRIANGLES, 0, render_obj->vert_count);
  }
}

void renderer_init(renderer_t* renderer) {
  glewExperimental = GL_TRUE;
  glewInit();

  const GLubyte* gpu = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", gpu);
  printf("Version: %s\n", version);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  const char* vert_shader =
    "#version 150\n"

    "in vec3 vposition;"

    "uniform mat4 model;"
    "uniform mat4 view;"
    "uniform mat4 projection;"

    "void main() {"
    "  gl_Position = projection * view * model * vec4(vposition, 1.0);"
    "}";

  const char* frag_shader =
    "#version 150\n"

    "out vec4 frag_colour;"

    "void main() {"
    "  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
    "}";

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vert_shader, NULL);
  glCompileShader(vs);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &frag_shader, NULL);
  glCompileShader(fs);

  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glBindAttribLocation(program, 0, "vposition");
  glLinkProgram(program);
  renderer->program = program;

  GLsizei len;
  char log[1024];
  glGetProgramInfoLog(program, 1024, &len, log);
  printf("%s\n", log);
  GLuint error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    printf("%X\n", error);
  }

  camera_t camera;
  camera.position = vec3(2.0f, 0.0f, -2.0f);
  camera.up = vec3(0,1,0);
  camera.view = m4_look_at(camera.position, vec3(0,0,0), camera.up);
  camera.projection = m4_perspective(67.0f, 1.333, 0.1f, 1000);
  renderer->camera = camera;
}
