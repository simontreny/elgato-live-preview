uniform sampler2D yTex;
uniform sampler2D uTex;
uniform sampler2D vTex;
varying vec2 uv;

void main() {
    float y = texture2D(yTex, uv).r;
    float u = texture2D(uTex, uv).r;
    float v = texture2D(vTex, uv).r;
    u = -0.436 + 0.872*u;
    v = -0.615 + 1.23*v;
    float r = y + 1.28033 * v;
    float g = y - 0.21482 * u - 0.38059 * v;
    float b = y + 2.12798 * u;
    gl_FragData[0] = vec4(r, g, b, 0.0);
}
