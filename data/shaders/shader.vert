#version 450

vec3 colors[6] = vec3[](
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.5, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

// vertex attributes:
layout(location = 0) in vec3 positionAttribute;
layout(location = 1) in vec2 uvAtrribute;
layout(location = 2) in vec3 normalAttribute;

layout(binding = 0) uniform cameraBuf {
    mat4 MVP;
} Camera;

layout(location = 0) out vec3 fragColor;

void main() {
    // matrix multiplication using model view projection
    gl_Position = Camera.MVP * vec4(positionAttribute, 1);
    //gl_Position = vec4(positionAttribute, 1);
    //fragColor = colors[gl_VertexIndex % 6];
    fragColor = vec3(1, 1, 1) - normalAttribute;
}