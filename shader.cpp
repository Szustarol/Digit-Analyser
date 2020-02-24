#include "shader.hpp"
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <iostream>


Shader::Shader(){
  created = false;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
}

Shader::Shader(const char * vertexPath, const char * fragmentPath){
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  Compile(vertexPath, fragmentPath);
  Link();
}

int Shader::Compile(const char * vertexPath, const char * fragmentPath){
  std::fstream vFile, fFile;
  vFile.open(vertexPath, std::ios_base::in);
  fFile.open(fragmentPath, std::ios_base::in);
  std::string line;
  while(std::getline(vFile, line)){
    vertexSource += line + '\n';
  }

  while(std::getline(fFile, line)){
    fragmentSource += line + '\n';
  }

  vID = glCreateShader(GL_VERTEX_SHADER);
  fID = glCreateShader(GL_FRAGMENT_SHADER);
  ID = glCreateProgram();

  const char * vP = vertexSource.c_str();
  const char * fP = fragmentSource.c_str();
  glShaderSource(vID, 1, &vP, NULL);
  glShaderSource(fID, 1, &fP, NULL);

  int success;
  int * sh[2] = {&vID, &fID};
  for(unsigned i = 0; i < 2; i++){
    glCompileShader(*sh[i]);
    glGetShaderiv(*sh[i], GL_COMPILE_STATUS, &success);
    if(!success){
      std::cerr << "Error while compiling shader no. " << i << std::endl;
      char infoLog[512];
      glGetShaderInfoLog(*sh[i], 512, NULL, infoLog);
      std::cerr << infoLog << std::endl;
      return -1;
    }
  }

  vFile.close();
  fFile.close();

  created = true;
  return 1;
}

void Shader::use(){
  glBindVertexArray(VAO);
  glUseProgram(ID);
}

void Shader::useTexture(bool use){
  glUniform1i(glGetUniformLocation(getID(), "useTexture"), use);
}

int Shader::Link(){
  glAttachShader(ID, vID);
  glAttachShader(ID, fID);
  glLinkProgram(ID);
  int success;
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if(!success){
    std::cerr << "Error while linking shader program." << std::endl;
    char infoLog[512];
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cerr << infoLog << std::endl;
    return -1;
  }
  use();
  glUniform1i(glGetUniformLocation(getID(), "mainTexture"), 0);
  return 1;
}

int Shader::getID(){
  return this->ID;
}

Shader::~Shader(){
  if(created){
    glDeleteShader(vID);
    glDeleteShader(fID);
    glDeleteProgram(ID);
  }
  glDeleteVertexArrays(1, &VAO);
}


void Shader::setTexture(Texture * t){
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, t->getID());
}

void Shader::setAttribf(unsigned location, size_t size, size_t span, unsigned long long offset){
  use();
  glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, span, (void*)offset);
  glEnableVertexAttribArray(location);
}
