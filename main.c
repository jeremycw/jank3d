#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "renderer.h"
#define MATH_3D_IMPLEMENTATION
#include "math_3d.h"

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
     0.0f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
  };

  render_obj_t* tri = renderer_create_obj(renderer, points, 3);
  render_obj_t* tri2 = renderer_create_obj(renderer, points, 3);
  tri2->transform.m32 = 4.0f;

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
