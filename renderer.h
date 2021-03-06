#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include "math_3d.h"

typedef struct {
  GLuint vao;
  int vert_count;
} mesh_t;

typedef struct {
  mat4_t transform;
  quat_t quat;
  GLuint tex;
  mesh_t mesh;
} render_obj_t;

typedef struct {
  float yaw;
  float pitch;
  vec3_t position;
  vec3_t up;
  vec3_t look;
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
void render_obj_attach_texture(render_obj_t* render_obj, tex_t tex);
mesh_t renderer_buffer_mesh(float* vertices, float* normals, float* uv, int vert_count);
mesh_t renderer_buffer_mesh_from_file(char* filename);
void renderer_translate_camera(renderer_t* renderer, float x, float y, float z);
void renderer_yaw_pitch_camera(renderer_t* renderer, float yaw, float pitch);

void render_obj_rotate_around_axis(render_obj_t* render_obj, vec3_t axis, float angle);
void render_obj_set_x(render_obj_t* render_obj, float x);
void render_obj_set_y(render_obj_t* render_obj, float y);
void render_obj_set_z(render_obj_t* render_obj, float z);
void render_obj_translate_x(render_obj_t* render_obj, float x);
void render_obj_translate_y(render_obj_t* render_obj, float y);
void render_obj_translate_z(render_obj_t* render_obj, float z);
float render_obj_x(render_obj_t* render_obj);
float render_obj_y(render_obj_t* render_obj);
float render_obj_z(render_obj_t* render_obj);
#endif
