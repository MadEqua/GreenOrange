float sphere(vec3 p, float r) {
     return length(p) - r;
}

float box(vec3 p, vec3 b)  {
    vec3 d = abs(p) - b;
    return length(max(d, 0.)) + min(max(d.x, max(d.y, d.z)), 0.);
}


//TODO: move to own file
//vec2 are (dist, materialIndex)
vec2 goMin(vec2 a, vec2 b) {
    if(a.x < b.x) return a;
    else return b;
}

vec2 goMax(vec2 a, vec2 b) {
    if(a.x > b.x) return a;
    else return b;
}

vec2 union_(vec2 a, vec2 b) {
   return goMin(a, b);
}

vec2 intersection(vec2 a, vec2 b) {
   return goMax(a, b);
}

vec2 subtraction(vec2 a, vec2 b) {
    vec2 m = goMax(a, -b);
    return vec2(m.x, a.y);
}

// The "Round" variant uses a quarter-circle to join the two objects smoothly:
vec2 unionRound(vec2 a, vec2 b, float r) {
    vec2 u = max(vec2(r - a.x, r - b.x), vec2(0));
    vec2 m = goMin(a, b);
    return vec2(max(r, m.x) - length(u), m.y);
}