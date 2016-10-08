#include <stdio.h>
#include <stdlib.h>
#include "renderer.h"
#include "window.h"
#define MATH_3D_IMPLEMENTATION
#include "math_3d.h"

void update(float delta_time, void* data);

typedef struct {
  window_t* window;
  renderer_t* renderer;
  render_obj_t* tri1;
  render_obj_t* tri2;
  double prev_x;
  double prev_y;
  float speed;
} app_t;

int main() {
  window_t window;
  window_init(&window, 640, 480);

  renderer_t* renderer = malloc(sizeof(renderer_t));
  renderer_init(renderer);

  app_t app;
  app.window = &window;
  window.data = &app;
  app.renderer = renderer;
  app.speed = 1.0f;

  float points[] = {
     0.0f,  0.5f, 0.0f, //top
     0.5f, -0.5f, 0.0f, //bot left
    -0.5f, -0.5f, 0.0f, //bot right
  };

  float uv[] = {
    0.5f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f
  };

  mesh_t mesh = renderer_buffer_mesh(points, NULL, uv, 3);
  app.tri1 = renderer_create_obj(renderer, mesh);
  app.tri1->quat = quat_axis_angle(renderer->camera.up, 0.78);
  app.tri1->transform = m4_from_quat(app.tri1->quat);
  app.tri2 = renderer_create_obj(renderer, mesh);
  app.tri2->transform.m32 = 4.0f;

  //create texture coords
  tex_t tex = renderer_buffer_texture("textures/default_grass.png");
  tex_t tex2 = renderer_buffer_texture("textures/default_brick.png");

  render_obj_attach_texture(app.tri1, tex);
  render_obj_attach_texture(app.tri2, tex2);

  mesh = renderer_buffer_mesh_from_file("spot.obj");
  render_obj_t* spot = renderer_create_obj(renderer, mesh);
  tex_t spot_tex = renderer_buffer_texture("spot_texture.png");
  render_obj_attach_texture(spot, spot_tex);

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
  int state = glfwGetKey(app->window->glfw_win, GLFW_KEY_A);
  if (state == GLFW_PRESS) {
    renderer_translate_camera(app->renderer, -1.0 * delta_time, 0, 0);
  }
  state = glfwGetKey(app->window->glfw_win, GLFW_KEY_D);
  if (state == GLFW_PRESS) {
    renderer_translate_camera(app->renderer, 1.0 * delta_time, 0, 0);
  }
  state = glfwGetKey(app->window->glfw_win, GLFW_KEY_W);
  if (state == GLFW_PRESS) {
    renderer_translate_camera(app->renderer, 0, 0, 1.0 * delta_time);
  }
  state = glfwGetKey(app->window->glfw_win, GLFW_KEY_S);
  if (state == GLFW_PRESS) {
    renderer_translate_camera(app->renderer, 0, 0, -1.0 * delta_time);
  }
  double xpos, ypos;
  glfwGetCursorPos(app->window->glfw_win, &xpos, &ypos);
  double yaw_delta = xpos - app->prev_x;
  double pitch_delta = ypos - app->prev_y;
  app->prev_x = xpos;
  app->prev_y = ypos;
  renderer_yaw_pitch_camera(app->renderer, 0.2f * yaw_delta, -0.2f * pitch_delta);
  render(app->renderer);
}

