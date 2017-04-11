#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "window.h"

int window_init(window_t* window, int width, int height) {
  if (!glfwInit()) {
    printf("error initializing glfw!\n");
    return 1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window->glfw_win = glfwCreateWindow(width, height, "Hello Triangle", NULL, NULL);
  glfwSetInputMode(window->glfw_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  window->width = width;
  window->height = height;
  if (!window->glfw_win) {
    printf("couldnt open window\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window->glfw_win);
  return 0;
}

void window_run(window_t* window, void (*f)(float delta_time, void* data)) {
  window->prev_time = glfwGetTime();
  while (!glfwWindowShouldClose(window->glfw_win)) {
    double time = glfwGetTime();
    double elapsed_time = time - window->prev_time;
    f(elapsed_time, window->data);
    glfwPollEvents();
    glfwSwapBuffers(window->glfw_win);
    if (GLFW_PRESS == glfwGetKey(window->glfw_win, GLFW_KEY_ESCAPE)) {
      glfwSetWindowShouldClose(window->glfw_win, 1);
    }
    window->prev_time = time;
  }
  glfwTerminate();
}
