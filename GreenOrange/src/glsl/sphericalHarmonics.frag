#define PI 3.14159265359

out vec3 fragColor;
uniform samplerCube radianceMap;

//(Band.Base) order -> (0.0, 1.-1, 1.0, 1.1, 2.-2, 2.-1, 2.0, 2.1, 2.2)

vec3 sphericalToCartesian(float theta, float phi) {
    return vec3(sin(phi) * cos(theta), sin(theta) * sin(phi), cos(phi));
}

float[9] mulSpHarBaseScalar(float base[9], float v) {
    float res[9];
    res[0] = base[0] * v;
    res[1] = base[1] * v;
    res[2] = base[2] * v;
    res[3] = base[3] * v;
    res[4] = base[4] * v;
    res[5] = base[5] * v;
    res[6] = base[6] * v;
    res[7] = base[7] * v;
    res[8] = base[8] * v;
    return res;
}

vec3[9] mulSpHarBaseVec3(float base[9], vec3 v) {
    vec3 res[9];
    res[0] = base[0] * v;
    res[1] = base[1] * v;
    res[2] = base[2] * v;
    res[3] = base[3] * v;
    res[4] = base[4] * v;
    res[5] = base[5] * v;
    res[6] = base[6] * v;
    res[7] = base[7] * v;
    res[8] = base[8] * v;
    return res;
}

vec3[9] sumCoefficients(vec3 base1[9], vec3 base2[9]) {
    vec3 res[9];
    res[0] = base1[0] + base2[0];
    res[1] = base1[1] + base2[1];
    res[2] = base1[2] + base2[2];
    res[3] = base1[3] + base2[3];
    res[4] = base1[4] + base2[4];
    res[5] = base1[5] + base2[5];
    res[6] = base1[6] + base2[6];
    res[7] = base1[7] + base2[7];
    res[8] = base1[8] + base2[8];
    return res;
}

vec3[9] getZeroCoefficients() {
    vec3 res[9];
    res[0] = vec3(0.0);
    res[1] = vec3(0.0);
    res[2] = vec3(0.0);
    res[3] = vec3(0.0);
    res[4] = vec3(0.0);
    res[5] = vec3(0.0);
    res[6] = vec3(0.0);
    res[7] = vec3(0.0);
    res[8] = vec3(0.0);
    return res;
}

float[9] getSphHarBaseConstants() {
    float base[9];
    base[0] = 0.282095;
    base[1] = -0.488603;
    base[2] = 0.488603;
    base[3] = -0.488603;
    base[4] = 1.092548;
    base[5] = -1.092548;
    base[6] = 0.315392;
    base[7] = -1.092548;
    base[8] = 0.546274;
    return base;
}

float[9] getSphHarBase(vec3 dir) {
    float base[9] = getSphHarBaseConstants();
    base[1] *= dir.y;
    base[2] *= dir.z;
    base[3] *= dir.x;
    base[4] *= dir.x * dir.y;
    base[5] *= dir.y * dir.z;
    base[6] *= 3.0 * dir.z * dir.z - 1.0;
    base[7] *= dir.x * dir.z;
    base[8] *= dir.x * dir.x - dir.y * dir.y;
    return base;
}

vec3[9] radianceMapToSphHarCoefficients(int size) {
    vec3 coefficients[9] = getZeroCoefficients();

    float hf = PI / float(size);
    float wf = (2.0  * PI) / float(size);
    for (int j = 0; j < size; ++j) {
        float phi = hf * float(j);
        float sinPhi = sin(phi);
        for (int i = 0; i < size; ++i) {
            float theta =  wf * float(i);
            vec3 dir = sphericalToCartesian(theta, phi);
            float base[9] = getSphHarBase(dir);
            vec3 radiance = texture(radianceMap, dir).rgb;

            base = mulSpHarBaseScalar(base, sinPhi);
            vec3 mul[9] = mulSpHarBaseVec3(base, radiance);
            coefficients = sumCoefficients(coefficients, mul);
        }
    }

    return coefficients;
}


//Every fragment will compute the 9 coeffs... :(
void main() {
    vec3 Lcoeffs[9] = radianceMapToSphHarCoefficients(64); //TODO: uniform size

    //Coming from the SH of the Lambertian term and the constant of the convolution in SH formula.
    float kCoeffs[9];
    kCoeffs[0] = PI;
    kCoeffs[1] = 2.094395f;
    kCoeffs[2] = 2.094395f;
    kCoeffs[3] = 2.094395f;
    kCoeffs[4] = 0.785398f;
    kCoeffs[5] = 0.785398f;
    kCoeffs[6] = 0.785398f;
    kCoeffs[7] = 0.785398f;
    kCoeffs[8] = 0.785398f;

    fragColor = Lcoeffs[int(gl_FragCoord.x)] * kCoeffs[int(gl_FragCoord.x)];
}