#version 330

uniform sampler2D yTex;
uniform sampler2D uTex;
uniform sampler2D vTex;
in vec2 texCoords;
out vec4 fragColor;

void main() {
    float y = texture(yTex, texCoords).r;
    float u = texture(uTex, texCoords).r;
    float v = texture(vTex, texCoords).r;
    u = -0.436 + 0.872 * u;
    v = -0.615 + 1.23 * v;
    float r = y + 1.28033 * v;
    float g = y - 0.21482 * u - 0.38059 * v;
    float b = y + 2.12798 * u;
    fragColor = vec4(r, g, b, 1.0);
}
