#ifndef DRAWAREA_HPP
#define DRAWAREA_HPP
#include "uiObject.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>

struct rawData{
  int width;
  int height;
  unsigned char * imageDataGr = nullptr;
  ~rawData(){
    if(imageDataGr) free(imageDataGr);
  }
};

class DrawArea : public UIObject{
  std::vector <vertex> points;
  rawData r;
public:
  void putPoint(double posX, double posY);
  void draw(Shader & s);
  void crop();
  void resize(double offset);
  void saveAndClear(int digit);
  rawData * getData();
  int pensize = 2;
  DrawArea();
};

#endif
