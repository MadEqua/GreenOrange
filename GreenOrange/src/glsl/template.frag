#define PRIMARY_STEPS 256
#define SHADOW_STEPS 32

#define EPSILON 0.01
#define MAX_DIST 100.

out vec4 fragColor;
uniform vec2 dimensions;

/////////////////////////////////////////////
// Transforms
/////////////////////////////////////////////
#GO_REPLACE_TRANSFORMS

/////////////////////////////////////////////
// Objects
/////////////////////////////////////////////
#GO_REPLACE_OBJECTS

/////////////////////////////////////////////
// Scene SDFs
/////////////////////////////////////////////
struct SceneQuery {
    float dist; //Distance of the input point to the scene
    int matIdx; //Material of the closest point of the scene to the input point
};

#GO_REPLACE_SCENE_SDFS

//TODO
int currentSceneIndex = 0;

//Returns the distance from p to the closest scene point and the material of that point.
SceneQuery scene(vec3 p) {
    //TODO: switch on current currentSceneIndex
    return Scene1(p);
}

/////////////////////////////////////////////
// Lights
/////////////////////////////////////////////
#if #GO_REPLACE_DIR_LIGHT_COUNT
struct DirLight {
    vec3 color;
    vec3 direction;
};
DirLight dirLights[#GO_REPLACE_DIR_LIGHT_COUNT];

ivec2 getDirLightSetForCurrentScene() {
    #GO_REPLACE_DIR_LIGHTS_SET
}
#endif

#if #GO_REPLACE_POINT_LIGHT_COUNT
struct PointLight {
    vec3 color;
    vec3 position;
};
PointLight pointLights[#GO_REPLACE_POINT_LIGHT_COUNT];

ivec2 getPointLightSetForCurrentScene() {
    #GO_REPLACE_POINT_LIGHTS_SET
}
#endif

void initLights() {
    #GO_REPLACE_LIGHTS_INIT
}

/////////////////////////////////////////////
// Materials
/////////////////////////////////////////////
struct Material {
    vec3 color;
};
Material materials[#GO_REPLACE_MATERIAL_COUNT];

void initMaterials() {
    #GO_REPLACE_MATERIALS_INIT
}

/////////////////////////////////////////////
// Raymarcher
/////////////////////////////////////////////
struct RayPoint {
    vec3 point; //World space point on the ray
    float t; //Distance travelled along the ray to get to 'point'
    SceneQuery sq; //Data from closest point of the scene to 'point'
};

vec3 cam2world(vec3 v, vec3 pos, vec3 lookAt) {
    vec3 z = normalize(lookAt - pos);
    vec3 y = vec3(0, 1, 0);
    vec3 x = normalize(cross(z, y));
    y = normalize(cross(x, z));
    return normalize(mat3(x, y, z) * v);
}

//Should fill rp.point and rp.t, rp.sq is delegated to scene()
bool rm(int steps, vec3 ro, vec3 rd, out RayPoint rp) {
    rp.t = 0.;
    for(int i = 0; i < steps && rp.t < MAX_DIST; ++i) {
        rp.point = ro + rd * rp.t;
        rp.sq = scene(rp.point);
        if(rp.sq.dist < EPSILON) {
            return true;
        }
        rp.t += rp.sq.dist;
    }
    return false;
}

vec3 normal(RayPoint rp) {
    vec2 e = vec2(EPSILON, 0.);
    //float d = scene(p).dist;
    float d = rp.sq.dist;
    float x = scene(rp.point - e.xyy).dist;
    float y = scene(rp.point - e.yxy).dist;
    float z = scene(rp.point - e.yyx).dist;
    return normalize(vec3(d) - vec3(x, y, z));
}

vec3 shade(RayPoint rp) {
    vec3 sum = vec3(0.0);
    vec3 N = normal(rp);
    vec3 matColor = materials[rp.sq.matIdx].color;

    #if #GO_REPLACE_DIR_LIGHT_COUNT
    ivec2 dirLightSet = getDirLightSetForCurrentScene();
    for(int i = dirLightSet.x; i <= dirLightSet.y; ++i) {
        vec3 L = -normalize(dirLights[i].direction);
        float diff = max(0.0, dot(L, N));
        sum += diff * dirLights[i].color * matColor;
    }
    #endif

    #if #GO_REPLACE_POINT_LIGHT_COUNT
    ivec2 pointLightSet = getPointLightSetForCurrentScene();
    for(int i = pointLightSet.x; i <= pointLightSet.y; ++i) {
        vec3 L = normalize(pointLights[i].position - p);
        float diff = max(0.0, dot(L, N));
        sum += diff * pointLights[i].color * matColor;
    }
    #endif

    return sum;
}

void main() {
    initLights();
    initMaterials();

    vec2 uv = (gl_FragCoord.xy - .5 * dimensions.xy) / dimensions.y;

    vec3 cameraPos = vec3(0., 0., 5.);
    vec3 lookAt = vec3(0.);

    vec3 rd = cam2world(vec3(uv, 1.), cameraPos, lookAt); 

    vec3 col = vec3(.2);
    RayPoint rp;
    bool hit = rm(PRIMARY_STEPS, cameraPos, rd, rp);
    if(hit) {
        col = shade(rp);
    }

    fragColor = vec4(col, 1.);
}