#include <GL/glew.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <stdio.h>
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

  renderer_t renderer;
  renderer_init(&renderer);

  GLfloat points[] = {
     0.0f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
  };

  render_obj_t tri;
  render_obj_create(&tri, points, 3);

  float speed = 1.0f;
  double prev_time = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    double elapsed_time = time - prev_time;
    tri.transform.m30 += speed * elapsed_time;
    if (tri.transform.m30 >= 1.0f || tri.transform.m30 <= -1.0f) {
      speed = -speed;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render_objs(&tri, 1, &renderer);
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
