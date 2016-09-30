#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "renderer.h"
#define MATH_3D_IMPLEMENTATION
#include "math_3d.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main() {
  if (!glfwInit()) {
    printf("error initializing glfw!\n");
    return 1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
  if (!window) {
    printf("couldnt open window\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  renderer_t* renderer = malloc(sizeof(renderer_t));
  renderer_init(renderer);

  GLfloat points[] = {
     0.0f,  0.5f, 0.0f, //top
     0.5f, -0.5f, 0.0f, //bot left
    -0.5f, -0.5f, 0.0f, //bot right
  };

  render_obj_t* tri = renderer_create_obj(renderer, points, 3);
  tri->quat = quat_axis_angle(renderer->camera.up, 0.78);
  tri->transform = m4_from_quat(tri->quat);
  render_obj_t* tri2 = renderer_create_obj(renderer, points, 3);
  tri2->transform.m32 = 4.0f;

  tex_t tex = renderer_buffer_texture(renderer, "textures/default_grass.png");
  GLuint error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    printf("%X\n", error);
  }

  //create texture coords
  GLfloat texcoords[] = {
    0.5f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f
  };

  render_obj_attach_texture(tri, tex, texcoords, sizeof(texcoords));

  float speed = 1.0f;
  double prev_time = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    double elapsed_time = time - prev_time;
    tri->transform.m30 += speed * elapsed_time;
    tri2->transform.m30 -= speed * elapsed_time;
    if (tri2->transform.m30 >= 1.0f || tri2->transform.m30 <= -1.0f) {
      speed = -speed;
    }
    render(renderer);
    glfwPollEvents();
    glfwSwapBuffers(window);
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window, 1);
    }
    prev_time = time;
  }

  glfwTerminate();
  return 0;
}
