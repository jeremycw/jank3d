#include <GL/glew.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "renderer.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void render_obj_create(render_obj_t* render_obj, mesh_t mesh);
void render_objs(render_obj_t* render_obj, int count, renderer_t* renderer);

void print_error(char* message) {
  GLuint error = glGetError();
  if (error != GL_NO_ERROR) {
    printf("%s: %X ", message, error);
    while ((error = glGetError()) != GL_NO_ERROR) {
      printf("%X ", error);
    }
    printf("\n");
  }
}

void render_objs(render_obj_t* render_objs, int count, renderer_t* renderer) {
  for (int i = 0; i < count; i++) {
    glUseProgram(renderer->program);
    glBindTexture(GL_TEXTURE_2D, render_objs[i].tex);
    GLuint mat_location = glGetUniformLocation(renderer->program, "model");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, render_objs[i].transform.buf);
    mat_location = glGetUniformLocation(renderer->program, "view");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, renderer->camera.view.buf);
    mat_location = glGetUniformLocation(renderer->program, "projection");
    glUniformMatrix4fv(mat_location, 1, GL_FALSE, renderer->camera.projection.buf);
    glBindVertexArray(render_objs[i].mesh.vao);
    glDrawArrays(GL_TRIANGLES, 0, render_objs[i].mesh.vert_count);
    print_error("drawing");
  }
}

void renderer_init(renderer_t* renderer) {
  glewExperimental = GL_TRUE;
  glewInit();

  const GLubyte* gpu = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", gpu);
  printf("Version: %s\n", version);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glActiveTexture(GL_TEXTURE0);
  print_error("initializing");

  const char* vert_shader =
    "#version 150\n"

    "in vec3 vposition;"
    "in vec2 vtexcoords;"

    "out vec2 texcoords;"

    "uniform mat4 model;"
    "uniform mat4 view;"
    "uniform mat4 projection;"

    "void main() {"
    "  gl_Position = projection * view * model * vec4(vposition, 1.0);"
    "  texcoords = vtexcoords;"
    "}";

  const char* frag_shader =
    "#version 150\n"

    "in vec2 texcoords;"

    "uniform sampler2D tex;"
    "out vec4 frag_colour;"

    "void main() {"
    "  frag_colour = texture(tex, texcoords);"
    "}";

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
  glBindAttribLocation(program, 1, "vtexcoords");
  glLinkProgram(program);
  renderer->program = program;

  GLsizei len;
  char log[1024];
  glGetProgramInfoLog(program, 1024, &len, log);
  printf("%s\n", log);
  print_error("compiling shaders");

  camera_t camera;
  camera.position = vec3(2.0f, 0.0f, -2.0f);
  camera.up = vec3(0,1,0);
  camera.look = vec3(0,0,0);
  camera.view = m4_look_at(camera.position, camera.look, camera.up);
  camera.projection = m4_perspective(67.0f, 1.333, 0.1f, 1000);
  renderer->camera = camera;
}

void render_obj_create(render_obj_t* render_obj, mesh_t mesh) {
  render_obj->transform = m4_identity();
  render_obj->quat = quat_identity();
  render_obj->mesh = mesh;
}

void render(renderer_t* renderer) {
  renderer->camera.view = m4_look_at(renderer->camera.position, renderer->camera.look, renderer->camera.up);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  render_objs(renderer->render_objs, renderer->obj_count, renderer);
}

render_obj_t* renderer_create_obj(renderer_t* renderer, mesh_t mesh) {
  render_obj_create(renderer->render_objs + renderer->obj_count, mesh);
  renderer->obj_count++;
  return &renderer->render_objs[renderer->obj_count-1];
}

mesh_t renderer_buffer_mesh(float* vertices, float* normals, float* uv, int vert_count) {
  mesh_t mesh;
  mesh.vert_count = vert_count;
  glGenVertexArrays(1, &mesh.vao);
  glBindVertexArray(mesh.vao);
  print_error("creating vao");

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(float) * 3, vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(0);
  print_error("buffering verts");

  if (uv) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(float) * 2, uv, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    print_error("buffering uv");
  }

  if (normals) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vert_count * sizeof(float) * 3, normals, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
    print_error("buffering normals");
  }

  return mesh;
}

void renderer_translate_camera_x(renderer_t* renderer, float magnitude) {
  vec3_t look_dir =
    v3_sub(renderer->camera.look, renderer->camera.position);
  vec3_t direction =
    v3_norm(v3_cross(look_dir, renderer->camera.up));
  vec3_t delta = v3_muls(direction, magnitude);
  renderer->camera.position = v3_add(renderer->camera.position, delta);
  renderer->camera.look = v3_add(renderer->camera.look, delta);
}

void renderer_translate_camera_y(renderer_t* renderer, float magnitude) {
  vec3_t direction = v3_norm(renderer->camera.up);
  vec3_t delta = v3_muls(direction, magnitude);
  renderer->camera.position = v3_add(renderer->camera.position, delta);
  renderer->camera.look = v3_add(renderer->camera.look, delta);
}

mesh_t renderer_buffer_mesh_from_file(char* filename) {
  const struct aiScene* scene = aiImportFile(filename, aiProcess_Triangulate);
  const struct aiMesh* mesh = scene->mMeshes[0];
  size_t size = sizeof(float) * 3 * mesh->mNumVertices;
  float* vertices = malloc(size);
  memcpy(vertices, mesh->mVertices, size);

  float* normals = NULL;
  if (mesh->mNormals) {
    normals = malloc(size);
    memcpy(normals, mesh->mNormals, size);
  }
  float* uvs = NULL;
  if (mesh->mNumUVComponents[0] > 0) {
    uvs = malloc(sizeof(float) * 2 * mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      uvs[i*2] = mesh->mTextureCoords[0][i].x;
      uvs[i*2+1] = mesh->mTextureCoords[0][i].y;
    }
  }
  mesh_t m = renderer_buffer_mesh(vertices, normals, uvs, mesh->mNumVertices);
  aiReleaseImport(scene);
  free(vertices);
  if (normals) free(normals);
  if (uvs) free(uvs);
  return m;
}

tex_t renderer_buffer_texture(const char* filename) {
  tex_t t;
  stbi_set_flip_vertically_on_load(1);
  unsigned char* image = stbi_load(filename, &t.x, &t.y, &t.n, 0);

  GLuint tex_type;
  switch (t.n) {
    case 3:
      tex_type = GL_RGB;
      break;
    case 4:
      tex_type = GL_RGBA;
      break;
  }

  //create texture
  glGenTextures(1, &t.tex);
  glBindTexture(GL_TEXTURE_2D, t.tex);
  glTexImage2D(GL_TEXTURE_2D, 0, tex_type, t.x, t.y, 0, tex_type, GL_UNSIGNED_BYTE, image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  print_error("loading texture");
  free(image);
  return t;
}

void render_obj_attach_texture(render_obj_t* render_obj, tex_t tex) {
  render_obj->tex = tex.tex;
}
