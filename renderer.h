#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "math_3d.h"

typedef struct {
  GLuint vbo;
  int vert_count;
} mesh_t;

typedef struct {
  mat4_t transform;
  quat_t quat;
  GLuint vao;
  GLuint tex;
  mesh_t mesh;
} render_obj_t;

typedef struct {
  vec3_t position;
  vec3_t up;
  mat4_t view;
  mat4_t projection;
} camera_t;

typedef struct {
  camera_t camera;
  GLuint program;
  int obj_count;
  render_obj_t render_objs[1024];
} renderer_t;

typedef struct {
  GLuint tex;
  int x;
  int y;
  int n;
} tex_t;

void renderer_init(renderer_t* renderer);
void render(renderer_t* renderer);
render_obj_t* renderer_create_obj(renderer_t* renderer, mesh_t mesh);
tex_t renderer_buffer_texture(const char* filename);
void render_obj_attach_texture(render_obj_t* render_obj, tex_t tex, float* coords, int size);
mesh_t renderer_buffer_mesh(float* vertices, int vert_count);

#endif
