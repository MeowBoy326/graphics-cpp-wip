#version 460

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 vcolor;

void main() {
    // color = vec4(1.0, 1.0, 1.0, 1.0);   
    color = vec4(vcolor, 1.0);   
}