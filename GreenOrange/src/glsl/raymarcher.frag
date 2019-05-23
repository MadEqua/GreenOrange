#define PRIMARY_STEPS 256
#define SHADOW_STEPS 32

#define EPSILON 0.001
#define MAX_DIST 100.


float rm(int steps, vec3 ro, vec3 rd) {
    float t = 0.;
    for(int i = 0; i < steps && t < MAX_DIST; ++i) {
        vec3 p = ro + rd * t;
        float d = scene(p);
        if(d < EPSILON)
            return t;
        t += d;
    }
    return MAX_DIST;
}

vec3 normal(vec3 p) {
    vec2 e = vec2(EPSILON, 0.);
    float d = scene(p);
    float x = scene(p - e.xyy);
    float y = scene(p - e.yxy);
    float z = scene(p - e.yyx);
    return normalize(vec3(d) - vec3(x, y, z));
}

vec3 shade(vec3 p) {
    vec3 sum = vec3(0.0);
    for(int i = 0; i < dirLightCount; ++i) {
        vec3 L = dirLightPositions[i];
        vec3 N = normal(p);
        float diff = max(0.0, dot(-L, N));
        sum += diff * unpackUnorm4x8(dirLightColors[i]).xyz;
    }
    return sum;
}