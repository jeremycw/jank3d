#include <GL/glew.h>
#include "renderer.h"

void render_obj_create(render_obj_t* render_obj, float* vertices, int vert_count);
void render_objs(render_obj_t* render_obj, int count, renderer_t* renderer);

void render_objs(render_obj_t* render_objs, int count, renderer_t* renderer) {
  for (int i = 0; i < count; i++) {
    glUseProgram(renderer->program);
    GLuint mat_location = glGetUniformLocation(renderer->program, "model");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, render_objs[i].transform.buf);
    mat_location = glGetUniformLocation(renderer->program, "view");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, renderer->camera.view.buf);
    mat_location = glGetUniformLocation(renderer->program, "projection");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, renderer->camera.projection.buf);
    glBindVertexArray(render_objs[i].vao);
    glDrawArrays(GL_TRIANGLES, 0, render_objs[i].vert_count);
    GLuint error;
    while ((error = glGetError()) != GL_NO_ERROR) {
      printf("%X\n", error);
    }
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

void render_obj_create(render_obj_t* render_obj, float* vertices, int vert_count) {
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(float) * 3, vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  render_obj->transform = m4_identity();
  render_obj->vao = vao;
  render_obj->vbo = vbo;
  render_obj->vert_count = vert_count;

  GLuint error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    printf("%X\n", error);
  }
}

void render(renderer_t* renderer) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  render_objs(renderer->render_objs, renderer->obj_count, renderer);
}

render_obj_t* renderer_create_obj(renderer_t* renderer, float* vertices, int vert_count) {
  render_obj_create(renderer->render_objs + renderer->obj_count, vertices, vert_count);
  renderer->obj_count++;
  return &renderer->render_objs[renderer->obj_count-1];
}
