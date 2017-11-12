#version 150

uniform sampler2D texture;
varying vec2 uv;
varying vec2 screenSize;

vec2 stepX = 1.0 / screenSize.x;
vec2 stepY = 1.0 / screenSize.y;

void main() {
    vec2 offset[5];
    offset[0] = vec2(uv.x, uv.y - stepY);
    offset[1] = vec2(uv.x - stepX, uv.y);
    offset[2] = vec2(uv.x, uv.y);
    offset[3] = vec2(uv.x + stepX, uv.y);
    offset[4] = vec2(uv.x, uv.y + stepY);

    float kernel[5];
    kernel[0] = -0.75;
    kernel[1] = -0.75;
    kernel[2] = 4.;
    kernel[3] = -0.75;
    kernel[4] = -0.75;

    vec4 sum = vec4(0.0);
    int i;
    for (i = 0; i < 5; i++) {
        vec4 color = texture2D(texture, offset[i]);
        sum += color * kernel[i];
    }

    gl_FragData[0] = sum;
}
