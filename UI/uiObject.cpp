#include "uiObject.hpp"
#include <cstring>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

UIObject::UIObject(){
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  vertices.resize(4);

  for(unsigned i = 0; i < 4; i++){
    vertices.at(i).color[0] = 0.8f;
    vertices.at(i).color[1] = 1.0f;
    vertices.at(i).color[2] = 0.5f;
    vertices.at(i).color[3] = 1.0f;
    vertices.at(i).pos[2] = 1.0f;
  }

  vertices.at(0).texPos[0] = 0;
  vertices.at(0).texPos[1] = 0;
  vertices.at(1).texPos[0] = 1;
  vertices.at(1).texPos[1] = 1;
  vertices.at(2).texPos[0] = 1;
  vertices.at(2).texPos[1] = 0;
  vertices.at(3).texPos[0] = 0;
  vertices.at(3).texPos[1] = 1;

  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);
  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(3);
}

void UIObject::setColor(float r, float g, float b){
  for(unsigned i = 0; i < vertices.size(); i++){
    vertices.at(i).color[0] = r;
    vertices.at(i).color[1] = g;
    vertices.at(i).color[2] = b;
  }
}

float roundup(float f){
  if(f >= 1)
    f = 1;
  else
    f = 0;
  return f;
}

UIObject::~UIObject(){
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void UIObject::updateGlue(int width, int height){

  if(stretchX && scrW != (unsigned)-1){
    this->realWidth *= (double)width / (double)scrW;
    this->posX *= (double)width / (double)scrW;
  }

  if(stretchY && scrH != (unsigned)-1){
    this->realHeight *= (double)height / (double)scrH;
    this->posY *= (double)height / (double)scrH;
  }

  scrW = width;
  scrH = height;

  this->width = (int)realWidth;
  this->height = (int)realHeight;


  if(stick == 0){
    vertices.at(0).pos[0] = ((float)posX)/width;
    vertices.at(0).pos[1] = ((float)posY)/height;
  }
  else if(stick == 1){ // top right
    vertices.at(0).pos[0] = ((float)width - posX - this->width)/width;
    vertices.at(0).pos[1] = ((float)posY)/height;
  }
  else if(stick == 2){
    vertices.at(0).pos[0] = ((float)posX)/width;
    vertices.at(0).pos[1] = ((float)height - this->height - posY)/height;
  }
  else if(stick == 3){//bottom right
    vertices.at(0).pos[0] = ((float)width - posX - this->width)/width;
    vertices.at(0).pos[1] = ((float)height- posY - this->height)/height;
  }

  vertices.at(0).pos[0] *= 2.0f;
  vertices.at(0).pos[0] -= 1.0f;
  vertices.at(0).pos[1] *= -2.0f;
  vertices.at(0).pos[1] += 1.0f;

  for(unsigned i = 1; i < 4; i++){
    vertices.at(i).pos[1] = vertices.at(0).pos[1] - 2 * ((float)this->height/height) * roundup(i % 2);
    vertices.at(i).pos[0] = vertices.at(0).pos[0] + 2 * ((float)this->width/width) * roundup(i % 3);
  }
}

void UIObject::draw(Shader & s){
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)*indices.size(), indices.data(), GL_DYNAMIC_DRAW);

  s.setAttribf(0, 3, sizeof(vertex), offsetof(vertex, pos));
  s.setAttribf(1, 4, sizeof(vertex), offsetof(vertex, color));
  s.setAttribf(2, 2, sizeof(vertex), offsetof(vertex, texPos));

  if(tex != nullptr){
    s.useTexture(true);
    s.setTexture(tex);
  }
  else{
    s.useTexture(false);
  }

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
