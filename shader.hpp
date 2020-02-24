#ifndef SHADER_HPP
#define SHADER_HPP
#include <string>
#include "data/texture.hpp"

class Shader{
  std::string vertexSource;
  std::string fragmentSource;
  int ID, vID, fID;
  bool created = false;


public:
  unsigned VAO;
  int Compile(const char * vertexPath, const char * fragmentPath);
  int Link();
  Shader(const char * vertexPath, const char * fragmentPath);
  Shader();
  void use();
  ~Shader();
  int getID();
  void useTexture(bool use);
  void setTexture(Texture * t);
  void setAttribf(unsigned location, size_t size, size_t span, unsigned long long offset);
};

#endif
