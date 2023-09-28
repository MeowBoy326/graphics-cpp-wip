#version 460


layout(location = 0) in vec3 pos;


layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;

layout(location = 0) out vec3 vcolor;

void main() {
    gl_Position = projection*view*model*vec4(pos, 1.0);
    vcolor = vec3(0.8, 0.5, 0.5);
}