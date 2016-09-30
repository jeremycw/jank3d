#include <stdio.h>
#include <stdlib.h>
#include "renderer.h"
#include "window.h"
#define MATH_3D_IMPLEMENTATION
#include "math_3d.h"

void update(float delta_time, void* data);

typedef struct {
  renderer_t* renderer;
  render_obj_t* tri1;
  render_obj_t* tri2;
  float speed;
} app_t;

int main() {
  window_t window;
  window_init(&window, 640, 480);

  renderer_t* renderer = malloc(sizeof(renderer_t));
  renderer_init(renderer);

  app_t app;
  window.data = &app;
  app.renderer = renderer;
  app.speed = 1.0f;

  float points[] = {
     0.0f,  0.5f, 0.0f, //top
     0.5f, -0.5f, 0.0f, //bot left
    -0.5f, -0.5f, 0.0f, //bot right
  };

  mesh_t mesh = renderer_buffer_mesh(points, 3);
  app.tri1 = renderer_create_obj(renderer, mesh);
  app.tri1->quat = quat_axis_angle(renderer->camera.up, 0.78);
  app.tri1->transform = m4_from_quat(app.tri1->quat);
  app.tri2 = renderer_create_obj(renderer, mesh);
  app.tri2->transform.m32 = 4.0f;

  //create texture coords
  float texcoords[] = {
    0.5f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f
  };

  tex_t tex = renderer_buffer_texture("textures/default_grass.png");
  tex_t tex2 = renderer_buffer_texture("textures/default_brick.png");

  render_obj_attach_texture(app.tri1, tex, texcoords, sizeof(texcoords));
  render_obj_attach_texture(app.tri2, tex2, texcoords, sizeof(texcoords));

  window_run(&window, update);

  return 0;
}

void update(float delta_time, void* data) {
  app_t* app = (app_t*)data;
  app->tri1->transform.m30 += app->speed * delta_time;
  app->tri2->transform.m30 -= app->speed * delta_time;
  if (app->tri2->transform.m30 >= 1.0f || app->tri2->transform.m30 <= -1.0f) {
    app->speed = -app->speed;
  }
  render(app->renderer);
}
