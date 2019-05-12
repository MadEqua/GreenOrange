out vec4 fragColor;
uniform vec2 dimensions;

void main() {
    fragColor = vec4(gl_FragCoord.xy / dimensions, 0.0f, 1.0f);
}