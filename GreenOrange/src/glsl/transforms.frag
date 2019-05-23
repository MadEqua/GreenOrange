//TODO: this may be improved
mat3 rotateEuler(float x, float y, float z) {
    float sx = sin(radians(-x));
    float sy = sin(radians(-y));
    float sz = sin(radians(-z));

    float cx = cos(radians(-x));
    float cy = cos(radians(-y));
    float cz = cos(radians(-z));

    mat3 rotX = mat3(1, 0, 0, 0, cx, -sx, 0, sx, cx);
    mat3 rotY = mat3(cy, 0, -sy, 0, 1, 0, sy, 0, cy);
    mat3 rotZ = mat3(cz, -sz, 0, sz, cz, 0, 0, 0, 1);
    return rotZ * rotX * rotY;
}