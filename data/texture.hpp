#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include "../stb_image.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <string>
#include <vector>

class Texture;

extern std::vector <Texture *> textures;

class Texture{
  int width, height, nChan;
  unsigned int id;
public:
  unsigned char * data = nullptr;
  unsigned int getID();
  std::string name;
  void loadData(const char * path);
  Texture(std::string n = "");
  Texture(const char * path, std::string n = "");
  ~Texture();
  static void load(const char * path);
  static void freeAll();
  static Texture * findByPath(std::string path);
};



#endif
