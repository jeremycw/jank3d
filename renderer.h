#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "math_3d.h"

typedef struct {
  GLuint vao;
  int vert_count;
  mat4_t transform;
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

void renderer_init(renderer_t* renderer);
void render(renderer_t* renderer);
render_obj_t* renderer_create_obj(renderer_t* renderer, float* vertices, int vert_count);

#endif
