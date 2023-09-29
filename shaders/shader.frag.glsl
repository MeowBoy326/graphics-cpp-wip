#version 460

layout(location = 0) out vec4 color;

layout(location = 0) in vec3 vpos;
layout(location = 1) in vec3 vnorm;
layout(location = 2) in vec3 vcolor;

const vec3 light_pos = vec3(2, 5, 10);
const float light_intensity = 50.0;

layout(location = 3) uniform vec3 view_pos;

void main() {
    const vec3 light_dir = normalize(light_pos - vpos);
    const vec3 view_dir = normalize(view_pos - vpos);

    // vec3 halfway = (light_dir + view_dir);
    // halfway /= normalize(halfway);

    float intensity = clamp(dot(vnorm, light_dir), 0.0, 1.0);
    color = vec4(intensity*vcolor, 1.0);   
}