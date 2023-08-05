#version 450

vec3 colors[6] = vec3[](
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.5, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

layout(location = 0) in vec3 position;

layout(binding = 0) uniform cameraBuf {
    mat4 MVP;
} Camera;

layout(location = 0) out vec3 fragColor;

void main() {
    //gl_Position = cameraData.position[gl_VertexIndex];

    // matrix multiplication using model view projection
    gl_Position = Camera.MVP * vec4(position, 1);

    //gl_Position = vec4(positions[gl_VertexIndex], 0, 1);
    fragColor = colors[gl_VertexIndex % 6];
    //fragColor = vec3(1.0f - position.x, position.y, position.z);
}