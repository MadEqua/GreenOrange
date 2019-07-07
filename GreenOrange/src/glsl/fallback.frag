out vec3 fragColor;
uniform vec2 dimensions;

void main() {
    fragColor = vec3(gl_FragCoord.xy / dimensions, 0.0f);
}