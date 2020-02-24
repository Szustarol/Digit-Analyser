#ifndef CALCULATIONS_HPP
#define CALCULATIONS_HPP
#include <vector>
#include <algorithm>

unsigned strokeWidth(unsigned char * data, unsigned width, unsigned height){
  int area = 0;
  int perimeter = 0;
  for(unsigned r = 1; r < height - 1; r++){
    for(unsigned c = 1; c < width - 1; c++){
      if(data[r*width+c] == 255){
        area ++;
        if(data[r*width+c-1] == 0 || data[r*width+c+1] == 0 || data[(r-1)*width+c] == 0 || data[(r+1)*width+c] == 0)
          perimeter++;
      }
    }
  }
  return 1.8*area / perimeter;
}

#endif
