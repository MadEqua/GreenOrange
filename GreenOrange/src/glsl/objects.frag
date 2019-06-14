float sphere(vec3 p, float r) {
     return length(p) - r;
}

float box(vec3 p, vec3 b)  {
    vec3 d = abs(p) - b;
    return length(max(d, 0.)) + min(max(d.x, max(d.y, d.z)), 0.);
}