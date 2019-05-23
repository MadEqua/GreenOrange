out vec4 fragColor;
uniform vec2 dimensions;

#define DIR_LIGHT_MAX 4
uint dirLightCount = 1;
vec3 dirLightPositions[DIR_LIGHT_MAX];
uint dirLightColors[DIR_LIGHT_MAX];

#GO_REPLACE_TRANSFORMS

#GO_REPLACE_OBJECTS

#GO_REPLACE_SCENES

//TODO
float scene(vec3 p) {
    return Scene1(p);
}

#GO_REPLACE_RAYMARCHER

vec3 cam2world(vec3 v, vec3 pos, vec3 lookAt) {
    vec3 z = normalize(lookAt - pos);
    vec3 y = vec3(0, 1, 0);
    vec3 x = normalize(cross(z, y));
    y = normalize(cross(x, z));
    return normalize(mat3(x, y, z) * v);
}

void initConstants() {
    dirLightPositions[0] = normalize(vec3(0.5, -1.0, -0.5));
    dirLightColors[0] = packUnorm4x8(vec4(.1, .9, .3, 1.));
}

void main() {
    initConstants();

    vec2 uv = (gl_FragCoord.xy - .5 * dimensions.xy) / dimensions.y;

    vec3 cameraPos = vec3(0., 0., 5.);
    vec3 lookAt = vec3(0.);

    vec3 rd = cam2world(vec3(uv, 1.), cameraPos, lookAt); 

    vec3 col = vec3(.2);
    float t = rm(PRIMARY_STEPS, cameraPos, rd);
    if(t < MAX_DIST) {
        col = shade(cameraPos + rd * t);
    }

    fragColor = vec4(col, 1.);
}