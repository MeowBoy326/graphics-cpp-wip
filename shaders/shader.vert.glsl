#version 460


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;


layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;


layout(location = 0) out vec3 vpos;
layout(location = 1) out vec3 vnorm;
layout(location = 2) out vec3 vcolor;

void main() {
    const vec4 vpos0 = model*vec4(pos, 1.0);
    vpos = vpos0.xyz/vpos0.w;
    gl_Position = projection*view*model*vec4(pos, 1.0);
    vnorm = norm;
    vcolor = vec3(0.8, 0.5, 0.5);
}