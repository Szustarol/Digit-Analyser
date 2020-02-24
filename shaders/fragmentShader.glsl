#version 330 core
in vec4 vertexColor;
in vec2 texPos;
out vec4 vCol;

uniform bool useTexture;

uniform sampler2D mainTexture;

void main(){
  if(useTexture){
    vCol = mix(vertexColor, texture(mainTexture, texPos), 0.8);
  }
  else{
    vCol = vertexColor;
  }
}
