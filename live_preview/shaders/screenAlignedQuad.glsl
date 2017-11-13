#version 330

uniform bool invertY = false;
layout(location = 0) in vec2 pos;
out vec2 texCoords;

void main() {
    float yFactor = invertY ? -1.0 : 1.0;
    gl_Position = vec4(pos.x, yFactor * pos.y, 0.0, 1.0);
    texCoords = pos * vec2(0.5, -0.5) + 0.5;
}
