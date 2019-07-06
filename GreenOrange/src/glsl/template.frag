#define PRIMARY_STEPS 256
#define SHADOW_STEPS 32

#define EPSILON 0.01
#define MAX_DIST 100.

#define PI 3.14159265359

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
// Operators
/////////////////////////////////////////////
#GO_REPLACE_OPERATORS

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
    float intensity;
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
    float intensity;
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
    vec3 baseColor;
    float metallic;
    float roughness;
    vec3 emissiveColor;
    float emissiveIntensity;
};
Material materials[#GO_REPLACE_MATERIAL_COUNT];

void initMaterials() {
    #GO_REPLACE_MATERIALS_INIT
}

/////////////////////////////////////////////
// Cameras
/////////////////////////////////////////////
struct Camera {
    vec3 pos;
    mat3 axis;
    float fovy; //In radians
};
Camera cameras[#GO_REPLACE_CAMERA_COUNT];

int getCameraForCurrentTime() {
    #GO_REPLACE_CAMERA_CURRENT
}

void initCameras() {
    #GO_REPLACE_CAMERAS_INIT
}

vec3 cam2world(vec3 v, Camera cam) {
    return normalize((cam.axis * v));
}

/////////////////////////////////////////////
// Raymarcher
/////////////////////////////////////////////
struct RayPoint {
    vec3 point; //World space point on the ray
    float t; //Distance travelled along the ray to get to 'point'
    SceneQuery sq; //Data from closest point of the scene to 'point'
};

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

vec3 fresnelSchlick(float HdotV, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - HdotV, 5.0);
}

float distributionGGX(float NdotH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH2 = NdotH * NdotH;
    
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return a2 / denom;
}

float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

float geometrySmith(float NdotV, float NdotL, float roughness) {
    float ggx2  = geometrySchlickGGX(NdotV, roughness);
    float ggx1  = geometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 cookTorrance(vec3 N, vec3 L, vec3 V, Material mat, vec3 light) {
    vec3 H = normalize(V + L);
        
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float HdotV = max(dot(H, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
        
    vec3 F0 = mix(vec3(0.04), mat.baseColor, mat.metallic); //Hardcoded reflectance for dielectrics
    vec3 F = fresnelSchlick(HdotV, F0);

    float G = geometrySmith(NdotV, NdotL, mat.roughness);
    float NDF = distributionGGX(NdotH, mat.roughness);
        
    vec3 specular = (NDF * G * F) / max((4.0 * NdotV * NdotL), 0.001);
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (vec3(1.0) - mat.metallic);

    vec3 Lout = (kD * mat.baseColor / PI + specular) * light * NdotL;
    Lout += vec3(0.01) * mat.baseColor; //some ambient
    
    return Lout;
}

vec3 shade(vec3 cameraPos, RayPoint rp) {
    vec3 Lout = vec3(0.0);
    vec3 N = normal(rp);
    vec3 V = normalize(cameraPos - rp.point);
    Material mat = materials[rp.sq.matIdx];

    #if #GO_REPLACE_DIR_LIGHT_COUNT
    ivec2 dirLightSet = getDirLightSetForCurrentScene();
    for(int i = dirLightSet.x; i <= dirLightSet.y; ++i) {
        DirLight light = dirLights[i];
        vec3 L = -normalize(light.direction);
        Lout += mat.emissiveColor * mat.emissiveIntensity + cookTorrance(N, L, V, mat, light.color * light.intensity);
    }
    #endif

    #if #GO_REPLACE_POINT_LIGHT_COUNT
    ivec2 pointLightSet = getPointLightSetForCurrentScene();
    for(int i = pointLightSet.x; i <= pointLightSet.y; ++i) {
        PointLight light = pointLights[i];
        vec3 Lunorm = light.position - rp.point;
        float Llength = length(Lunorm);
        vec3 L = Lunorm / Llength;
        float attenuation = 1.0 / (Llength * Llength);
        Lout += mat.emissiveColor * mat.emissiveIntensity + cookTorrance(N, L, V, mat, light.color * light.intensity * attenuation);
    }
    #endif 
    
    return Lout;
}

void main() {
    initLights();
    initMaterials();
    initCameras();

    vec2 uv = (gl_FragCoord.xy / dimensions.xy) * 2.0 - 1.0;
    Camera cam = cameras[getCameraForCurrentTime()];
    float maxRdy = tan(cam.fovy * 0.5);
    float maxRdx = maxRdy * (dimensions.x / dimensions.y);
    vec3 rdCam = vec3(uv.x * maxRdx, uv.y * maxRdy, 1.0);
    
    vec3 rd = cam2world(rdCam, cam); 

    vec3 col = vec3(.2);
    RayPoint rp;
    bool hit = rm(PRIMARY_STEPS, cam.pos, rd, rp);
    if(hit) {
        col = shade(cam.pos, rp);
    }

#if #GO_REPLACE_TONE_MAPPING
    //Tone mapping
    col = col / (col + vec3(1.0));
#endif

    fragColor = vec4(col, 1.);
}