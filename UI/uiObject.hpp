#ifndef UIOBJECT_HPP
#define UIOBJECT_HPP
#include <vector>
#include <iostream>
#include "../shader.hpp"
#include "../data/texture.hpp"

struct vertex{
  float pos[3];
  float color[4];
  float texPos[2];
};

class UIObject{
protected:
  std::vector<vertex> vertices;
  std::vector<unsigned> indices;
  unsigned VBO, EBO;
  int width, height;
public:
  enum ty{
    NONE,
    BUTTON,
    DRAWAREA
  };
  ty type = NONE;
  void (UIObject::*onClick)(int, int);
  UIObject();
  ~UIObject();
  unsigned scrW = -1, scrH = -1;
  double posX = 200, posY = 150;
  double realWidth = 200, realHeight= 100;
  unsigned stick = 0;
  bool stretchX = 0, stretchY = 0;
  void setColor(float r, float g, float b);
  void updateGlue(int width, int height);
  void draw(Shader & s);
  Texture * tex = nullptr;
  std::string name;
};


#endif
