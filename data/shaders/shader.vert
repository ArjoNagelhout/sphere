#version 450

vec3 colors[6] = vec3[](
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.5, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

layout(std140, binding = 0) uniform buf {
    vec4 position[3];
} ubuf;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubuf.position[gl_VertexIndex];
    fragColor = colors[gl_VertexIndex];
}