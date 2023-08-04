#version 450

vec3 colors[6] = vec3[](
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.5, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

vec2 positions[6] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5),
    vec2(0, 0),
    vec2(1, 1),
    vec2(-1, 1)
);

layout(location = 0) in vec3 position;

//layout(binding = 0) uniform buf {
//    vec4 position[3];
//} ubuf;

layout(location = 0) out vec3 fragColor;

void main() {
    //gl_Position = ubuf.position[gl_VertexIndex];
    gl_Position = vec4(position, 1);
    //gl_Position = vec4(positions[gl_VertexIndex], 0, 1);
    fragColor = colors[gl_VertexIndex];
    //fragColor = vec3(1.0f - position.x, position.y, position.z);
}