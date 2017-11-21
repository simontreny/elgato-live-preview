#version 330

uniform sampler2D uyvyTex;
in vec2 texCoords;
out vec4 fragColor;

void main() {
    int frameWidth = textureSize(uyvyTex, 0).x * 2;
    int x = int(texCoords.x * frameWidth);
    float y = (x % 2 == 0) ? texture(uyvyTex, texCoords).g : texture(uyvyTex, texCoords).a;
    float u = texture(uyvyTex, texCoords).r;
    float v = texture(uyvyTex, texCoords).b;
    y -= 16.0 / 255.0;
    u -= 128.0 / 255.0;
    v -= 128.0 / 255.0;
    float r = 1.164383 * y + 1.596027 * v;
    float g = 1.164383 * y - 0.391762 * u - 0.812968 * v;
    float b = 1.164383 * y + 2.017232 * u;
    fragColor = vec4(r, g, b, 1.0);
}
