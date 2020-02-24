#include "drawArea.hpp"
#include <iostream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <ctime>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

DrawArea::DrawArea() : UIObject(){
  type = ty::DRAWAREA;
}

void DrawArea::saveAndClear(int digit){
  if(points.size() > 10){
    int pos1[2] = {0, 0};
    int pos2[2] = {(int)realWidth, (int)realHeight};
    for(unsigned i = 0; i < points.size(); i++){
      vertex * v = &points.at(i);
      if(v->pos[0] > pos1[0]) pos1[0] = v->pos[0];
      if(v->pos[0] < pos2[0]) pos2[0] = v->pos[0];
      if(v->pos[1] > pos1[1]) pos1[1] = v->pos[1];
      if(v->pos[1] < pos2[1]) pos2[1] = v->pos[1];
    }

    int width, height;
    width = pos1[0]-pos2[0]+2;
    height = pos1[1]-pos2[1]+2;
    unsigned char * image_data_gr = (unsigned char *)malloc(sizeof(unsigned char)*width*height);
    memset(image_data_gr, 0, sizeof(unsigned char)*width*height);

    for(unsigned i = 0; i < points.size(); i++){
      vertex * v = &points.at(i);
      image_data_gr[((int)round(v->pos[1] - pos2[1]))*width+(unsigned)round(v->pos[0]-pos2[0])] = 255;
    }

    std::time_t t= std::time(0);
    std::tm* now = std::localtime(&t);
    std::string fname = "data/" + std::to_string(digit) + "/"  + std::to_string(now->tm_year + 1900) + "-" + std::to_string(now->tm_mon + 1) + "-" + std::to_string(now->tm_mday)
      + "-" + std::to_string(now->tm_hour) + "-" + std::to_string(now->tm_min) + "-" + std::to_string(now->tm_sec) + ".png";

    stbi_write_png(fname.c_str(), width, height, 1, image_data_gr, width);

    free(image_data_gr);

  }

  points.clear();
}

rawData * DrawArea::getData(){
  int pos1[2] = {0, 0};
  int pos2[2] = {(int)realWidth, (int)realHeight};
  for(unsigned i = 0; i < points.size(); i++){
    vertex * v = &points.at(i);
    if(v->pos[0] > pos1[0]) pos1[0] = v->pos[0];
    if(v->pos[0] < pos2[0]) pos2[0] = v->pos[0];
    if(v->pos[1] > pos1[1]) pos1[1] = v->pos[1];
    if(v->pos[1] < pos2[1]) pos2[1] = v->pos[1];
  }

  int width, height;
  width = pos1[0]-pos2[0]+2;
  height = pos1[1]-pos2[1]+2;
  if(r.imageDataGr != nullptr){
    free(r.imageDataGr);
  }
  r.imageDataGr = (unsigned char *)malloc(sizeof(unsigned char)*width*height);
  memset(r.imageDataGr, 0, sizeof(unsigned char)*width*height);
  r.width = width;
  r.height = height;

  for(unsigned i = 0; i < points.size(); i++){
    vertex * v = &points.at(i);
    r.imageDataGr[((int)round(v->pos[1] - pos2[1]))*width+(unsigned)round(v->pos[0]-pos2[0])] = 255;
  }

  return &r;
}


void DrawArea::putPoint(double posX, double posY){
  vertex v;
  for(int r = -pensize/2; r <= pensize/2; r++){
    for(int ry = -pensize/2; ry <= pensize/2; ry++){
      /*for(unsigned i = 0; i < points.size(); i++){
        if(points.at(i).pos[0] == round(posX+r) && points.at(i).pos[1] == round(posY+ry))
          continue;
      }*/
      if(sqrt(ry*ry + r*r) <= pensize/2 && r+posX > 1 && r+posX < realWidth && ry+posY>0 && ry+posY < realHeight){
          v.pos[0] = round(posX+r);
          v.pos[1] = round(posY+ry);
          v.pos[2] = 1.0f;
          v.color[0] = 0;
          v.color[1] = 0.0f;
          v.color[2] = 1.0f;
          v.color[3] = 1.0f;
          points.push_back(v);
      }
    }
  }
}

void DrawArea::crop(){
  for(int i = points.size()-1; i >= 0; i--){
    if(points.at(i).pos[0] > realWidth ||
    points.at(i).pos[0] < 0 ||
    points.at(i).pos[1] > realHeight ||
    points.at(i).pos[1] < 0)
      points.erase(points.begin()+i);
  }
}

void DrawArea::resize(double offset){
  pensize += offset;
  if(pensize < 1) pensize = 1;
  if(pensize > 40) pensize = 40;
}


void DrawArea::draw(Shader & s){
  UIObject::draw(s);

  GLuint displacement = glGetUniformLocation(s.getID(), "relocate");
  GLuint divisor = glGetUniformLocation(s.getID(), "divisor");
  GLuint remap = glGetUniformLocation(s.getID(), "from2");
  glUniform3f(displacement, posX, posY, 0);
  glUniform2f(divisor, scrW/(double)2, scrH/(double)2);
  glUniform1i(remap, true);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex)*points.size(), points.data(), GL_DYNAMIC_DRAW);
  s.setAttribf(0, 3, sizeof(vertex), offsetof(vertex, pos));
  s.setAttribf(1, 4, sizeof(vertex), offsetof(vertex, color));
  s.setAttribf(2, 2, sizeof(vertex), offsetof(vertex, texPos));
  glDrawArrays(GL_POINTS, 0, points.size());
  glUniform2f(divisor, 1, 1);
  glUniform1i(remap, false);
  glUniform3f(displacement, 0, 0, 0);
}
