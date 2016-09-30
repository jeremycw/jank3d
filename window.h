#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

typedef struct {
  GLFWwindow* glfw_win;
  void* data;
  int width;
  int height;
  float prev_time;
} window_t;

void window_run(window_t* window, void (*f)(float delta_time, void* data));
int window_init(window_t* window, int width, int height);

#endif
