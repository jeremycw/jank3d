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

#endif
