#include "button.hpp"

void Button::click(){
  if(onClick != nullptr)
    onClick(id);
}

Button::Button() : UIObject(){
  type = ty::BUTTON;
}
