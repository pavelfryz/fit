#version 330

uniform mat4 mvp;

layout(location=0)in vec3 position;
layout(location=1)in vec3 normal;
layout(location=2)in vec3 color;

out vec3 fPosition;
out vec3 fNormal;
out vec3 fColor;

void main(){
  gl_Position=mvp*vec4(position,1);
  fPosition = position;
  fNormal = normal;
  fColor = color;
}
