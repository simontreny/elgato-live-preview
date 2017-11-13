#version 330

uniform sampler2D tex;
in vec2 texCoords;
out vec4 fragColor;

void main() {
    const ivec2 offset[5] = ivec2[](ivec2(0, -1), ivec2(-1, 0), ivec2(0, 0), ivec2(1, 0), ivec2(0, 1));
    const float kernel[5] = float[](-0.75, -0.75, 4.0, -0.75, -0.75);

    vec4 sum = vec4(0.0);
    sum += kernel[0] * textureOffset(tex, texCoords, offset[0]);
    sum += kernel[1] * textureOffset(tex, texCoords, offset[1]);
    sum += kernel[2] * textureOffset(tex, texCoords, offset[2]);
    sum += kernel[3] * textureOffset(tex, texCoords, offset[3]);
    sum += kernel[4] * textureOffset(tex, texCoords, offset[4]);

    fragColor = sum;
}
