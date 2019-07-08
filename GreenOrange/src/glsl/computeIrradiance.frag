#define PI 3.14159265359

out vec3 fragColor;
uniform samplerCube radianceMap;
uniform int radianceMapSize;

//(Band.Base) order -> (0.0, 1.-1, 1.0, 1.1, 2.-2, 2.-1, 2.0, 2.1, 2.2)

vec3 sphericalToCartesian(float theta, float phi) {
    return vec3(sin(phi) * cos(theta), sin(theta) * sin(phi), cos(phi));
}

float[9] getSHBase(vec3 dir) {
    float base[9];
    base[0] = 0.282095;
    base[1] = -0.488603 * dir.y;
    base[2] = 0.488603 * dir.z;
    base[3] = -0.488603 * dir.x;
    base[4] = 1.092548 * dir.x * dir.y;
    base[5] = -1.092548 * dir.y * dir.z;
    base[6] = 0.315392 * (3.0 * dir.z * dir.z - 1.0);
    base[7] = -1.092548 * dir.x * dir.z;
    base[8] = 0.546274 * (dir.x * dir.x - dir.y * dir.y);
    return base;
}

vec3 radianceMapToSHCoefficient(int coeffIndex, int size) {
    vec3 coeff = vec3(0.0);

    float hf = PI / float(size);
    float wf = (2.0  * PI) / float(size);
    for (int j = 0; j < size; ++j) {
        float phi = hf * float(j);
        float sinPhi = sin(phi);
        for (int i = 0; i < size; ++i) {
            float theta =  wf * float(i);
            vec3 dir = sphericalToCartesian(theta, phi);
            float base = getSHBase(dir)[coeffIndex];
            vec3 radiance = texture(radianceMap, dir).rgb;
            coeff += base * sinPhi * radiance;
        }
    }
    return coeff * hf * wf;
}

void main() {
    vec3 Lcoeff = radianceMapToSHCoefficient(int(gl_FragCoord.x), radianceMapSize);

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

    fragColor = Lcoeff * kCoeffs[int(gl_FragCoord.x)];
}