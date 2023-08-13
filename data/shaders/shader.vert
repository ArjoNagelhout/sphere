#version 450

vec3 colors[6] = vec3[](
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.5, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

// ver tex attributes:
layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec2 v_UV;
layout(location = 2) in vec3 v_Normal;

layout(binding = 0) uniform cameraBuf {
    mat4 MVP;
} Camera;

// output
layout(location = 0) out vec2 out_UV;

void main() {
    gl_Position = Camera.MVP * vec4(v_Position, 1);
    out_UV = v_UV;
}