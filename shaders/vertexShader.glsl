#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vCol;
layout (location = 2) in vec2 tPos;

uniform vec3 relocate = vec3(0.0, 0.0, 0.0);
uniform vec2 divisor = vec2(1.0, 1.0);
uniform bool from2 = false;

out vec4 vertexColor;
out vec2 texPos;

void main(){
  vec4 tmp = vec4((vPos.x+relocate.x)/divisor.x, (vPos.y+relocate.y)/divisor.y, (vPos.z+relocate.z), 1.0f);
  if(from2 == true){
    tmp.x -= 1.0;
    tmp.y = 1.0-tmp.y;
  }
  gl_Position = tmp;
  vertexColor = vCol;
  texPos = tPos;
}
