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

  glewExperimental = GL_TRUE;
  glewInit();

  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", renderer);
  printf("Version: %s\n", version);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  GLfloat points[] = {
     0.0f,  0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
  };

  GLfloat colours[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
  };

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  render_obj_t tri;
  tri.transform = m4_identity();
  tri.vao = vao;
  tri.vert_count = sizeof(points) / 3;

  const char* vert_shader =
    "#version 150\n"

    "in vec3 vposition;"
    "in vec3 vcolour;"

    "uniform mat4 model;"
    "uniform mat4 view;"
    "uniform mat4 projection;"

    "out vec3 colour;"

    "void main() {"
    "  colour = vcolour;"
    "  gl_Position = projection * view * model * vec4(vposition, 1.0);"
    "}";

  const char* frag_shader =
    "#version 150\n"

    "in vec3 colour;"

    "out vec4 frag_colour;"

    "void main() {"
    "  frag_colour = vec4(colour, 1.0);"
    "}";

  vec3_t camera_position = vec3(2.0f, 0.0f, -2.0f);
  mat4_t view_matrix = m4_look_at(camera_position, vec3(0,0,0), vec3(0,1,0));
  mat4_t projection_matrix = m4_perspective(67.0f, 1.333, 0.1f, 1000);
  m4_print(view_matrix);

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vert_shader, NULL);
  glCompileShader(vs);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &frag_shader, NULL);
  glCompileShader(fs);

  GLuint program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glBindAttribLocation(program, 0, "vposition");
  glBindAttribLocation(program, 1, "vcolour");
  glLinkProgram(program);
  
  GLsizei len;
  char log[1024];
  glGetProgramInfoLog(program, 1024, &len, log);
  printf("%s\n", log);
  GLuint error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    printf("%X\n", error);
  }

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
    glUseProgram(program);
    GLuint mat_location = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, tri.transform.buf);
    mat_location = glGetUniformLocation(program, "view");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, view_matrix.buf);
    mat_location = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, projection_matrix.buf);
    glBindVertexArray(tri.vao);
    glDrawArrays(GL_TRIANGLES, 0, tri.vert_count);
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
