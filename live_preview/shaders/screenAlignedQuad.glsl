#version 330

layout(location = 0) in vec2 pos;
out vec2 texCoords;

void main() {
    texCoords = pos * vec2(0.5, -0.5) + 0.5;
    gl_Position = vec4(pos, 0.0, 1.0);
}
