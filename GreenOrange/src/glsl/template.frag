#define PRIMARY_STEPS 256
#define SHADOW_STEPS 32

#define EPSILON 0.001
#define MAX_DIST 100.

out vec4 fragColor;

uniform vec2 dimensions;

vec3 cam2world(vec3 v, vec3 pos, vec3 lookAt) {
    vec3 z = normalize(lookAt - pos);
    vec3 y = vec3(0, 1, 0);
    vec3 x = normalize(cross(z, y));
    y = normalize(cross(x, z));
    mat3 m = mat3(x, y, z);
    return normalize(m * v);
}

float sphere(vec3 p, float r) {
     return length(p) - r;
}

float box(vec3 p, vec3 b)  {
    vec3 d = abs(p) - b;
    return length(max(d, 0.)) + min(max(d.x, max(d.y, d.z)), 0.);
}

#GO_REPLACE_SCENES

float scene(vec3 p) {
    return Scene1(p);
}

float rm(int steps, vec3 ro, vec3 rd) {
    float t = 0.;
    for(int i = 0; i < steps && t < MAX_DIST; ++i) {
        vec3 p = ro + rd * t;
        float d = scene(p);
        if(d < EPSILON)
            return d;
        t += d;
    }
    return MAX_DIST;
}

vec3 normal(vec3 p, float d) {
    vec2 e = vec2(EPSILON, 0.);
    //float d = scene(p);
    float x = scene(p - e.xyy);
    float y = scene(p - e.yxy);
    float z = scene(p - e.yyx);
    return normalize(vec3(d) - vec3(x, y, z));
}

void main() {
    vec2 uv = (gl_FragCoord.xy - .5 * dimensions.xy) / dimensions.y;

    vec3 cameraPos = vec3(0., 0., 5.);
    vec3 lookAt = vec3(0.);
    
    vec3 rd = cam2world(vec3(uv, 1.), cameraPos, lookAt); 

    vec3 col = vec3(.2);
    float d = rm(PRIMARY_STEPS, cameraPos, rd);
    if(d < MAX_DIST) {
        col = vec3(.1, .9, .3);
    }

    fragColor = vec4(col, 1.);
}