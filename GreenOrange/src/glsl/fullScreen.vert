void main() {
    vec2 c = vec2(gl_VertexID << 1 & 2, gl_VertexID & 2);
    gl_Position = vec4(c * vec2(2, -2) + vec2(-1, 1), 0.0f, 1.0f);
}