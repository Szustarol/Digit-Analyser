#include "texture.hpp"
#include <iostream>

std::vector <Texture *> textures;

void Texture::loadData(const char * path){
  data = stbi_load(path, &width, &height, &nChan, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  if(data)
    stbi_image_free(data);
  glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(std::string n){
  glGenTextures(1, &id);
  name = n;
}

Texture::Texture(const char * path, std::string n){
  glGenTextures(1, &id);
  loadData(path);
  name = n;
}

Texture::~Texture(){
  glDeleteTextures(1, &id);
}

unsigned int Texture::getID(){
  return id;
}

void Texture::load(const char * path){
  Texture * t = new Texture(path, std::string(path));
  textures.push_back(t);
}

void Texture::freeAll(){
  for(int i = textures.size() - 1; i >= 0; i--){
    delete textures.at(i);
    textures.erase(textures.begin()+i);
  }
}

Texture * Texture::findByPath(std::string path){
  for(unsigned i = 0; i < textures.size(); i++){
    if(textures.at(i)->name == path) return textures.at(i);
  }
  return nullptr;
}
