#include "math_utils.h"

void identity33(Matrix m)
{
    m[0][0] = m[1][1] = m[2][2] = 1.0f;
    m[0][1] = m[0][2] = m[1][0] = m[1][2] = m[2][0] = m[2][1] = 0.0f;
}

void yy_transform(const Vec rotate, Matrix a)
{
    if (rotate[0] != 0.0f) {
        Flt ct = cos(rotate[0]), st = sin(rotate[0]);
        Flt t10 = ct*a[1][0] - st*a[2][0];
        Flt t11 = ct*a[1][1] - st*a[2][1];
        Flt t12 = ct*a[1][2] - st*a[2][2];
        Flt t20 = st*a[1][0] + ct*a[2][0];
        Flt t21 = st*a[1][1] + ct*a[2][1];
        Flt t22 = st*a[1][2] + ct*a[2][2];
        a[1][0] = t10; a[1][1] = t11; a[1][2] = t12;
        a[2][0] = t20; a[2][1] = t21; a[2][2] = t22;
        return;
    }
    if (rotate[1] != 0.0f) {
        Flt ct = cos(rotate[1]), st = sin(rotate[1]);
        Flt t00 = ct*a[0][0] - st*a[2][0];
        Flt t01 = ct*a[0][1] - st*a[2][1];
        Flt t02 = ct*a[0][2] - st*a[2][2];
        Flt t20 = st*a[0][0] + ct*a[2][0];
        Flt t21 = st*a[0][1] + ct*a[2][1];
        Flt t22 = st*a[0][2] + ct*a[2][2];
        a[0][0] = t00; a[0][1] = t01; a[0][2] = t02;
        a[2][0] = t20; a[2][1] = t21; a[2][2] = t22;
        return;
    }
    if (rotate[2] != 0.0f) {
        Flt ct = cos(rotate[2]), st = sin(rotate[2]);
        Flt t00 = ct*a[0][0] - st*a[1][0];
        Flt t01 = ct*a[0][1] - st*a[1][1];
        Flt t02 = ct*a[0][2] - st*a[1][2];
        Flt t10 = st*a[0][0] + ct*a[1][0];
        Flt t11 = st*a[0][1] + ct*a[1][1];
        Flt t12 = st*a[0][2] + ct*a[1][2];
        a[0][0] = t00; a[0][1] = t01; a[0][2] = t02;
        a[1][0] = t10; a[1][1] = t11; a[1][2] = t12;
        return;
    }
}

void trans_vector(Matrix mat, const Vec in, Vec out)
{
    Flt f0 = mat[0][0]*in[0] + mat[1][0]*in[1] + mat[2][0]*in[2];
    Flt f1 = mat[0][1]*in[0] + mat[1][1]*in[1] + mat[2][1]*in[2];
    Flt f2 = mat[0][2]*in[0] + mat[1][2]*in[1] + mat[2][2]*in[2];
    out[0] = f0; out[1] = f1; out[2] = f2;
}

void matrixFromAxisAngle(const Vec v, Flt ang, Matrix m)
{
    struct Axisangle { Flt angle, x, y, z; } a1;
    a1.x = v[0]; a1.y = v[1]; a1.z = v[2]; a1.angle = ang;
    Flt c = cos(a1.angle), s = sin(a1.angle), t = 1.0 - c;
    m[0][0] = c + a1.x*a1.x*t;
    m[1][1] = c + a1.y*a1.y*t;
    m[2][2] = c + a1.z*a1.z*t;
    Flt tmp1 = a1.x*a1.y*t, tmp2 = a1.z*s;
    m[1][0] = tmp1 + tmp2; m[0][1] = tmp1 - tmp2;
    tmp1 = a1.x*a1.z*t; tmp2 = a1.y*s;
    m[2][0] = tmp1 - tmp2; m[0][2] = tmp1 + tmp2;
    tmp1 = a1.y*a1.z*t; tmp2 = a1.x*s;
    m[2][1] = tmp1 + tmp2; m[1][2] = tmp1 - tmp2;
}

Flt vecNormalize(Vec vec)
{
    Flt len = vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];
    if (len == 0.0) { VecMake(0.0, 0.0, 1.0, vec); return 1.0; }
    len = sqrt(len);
    Flt tlen = 1.0 / len;
    vec[0] *= tlen; vec[1] *= tlen; vec[2] *= tlen;
    return len;
}
