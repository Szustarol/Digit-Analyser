#ifndef BUTTON_HPP
#define BUTTON_HPP
#include "uiObject.hpp"

class Button : public UIObject{
public:
  void (*onClick)(int) = nullptr;
  void click();
  Button();
  int id = -1;
};

#endif
