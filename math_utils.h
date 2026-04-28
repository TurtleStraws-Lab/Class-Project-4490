#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <math.h>

typedef float Flt;
typedef Flt Vec[3];
typedef Flt Matrix[4][4];

#define VecMake(a,b,c,d) d[0]=a; d[1]=b; d[2]=c

void identity33(Matrix m);
void yy_transform(const Vec rotate, Matrix a);
void trans_vector(Matrix mat, const Vec in, Vec out);
void matrixFromAxisAngle(const Vec v, Flt ang, Matrix m);
Flt  vecNormalize(Vec vec);

class Myvec {
public:
    Flt x, y, z;
    Myvec(Flt a, Flt b, Flt c) { x = a; y = b; z = c; }
    void make(Flt a, Flt b, Flt c) { x = a; y = b; z = c; }
    void negate() { x = -x; y = -y; z = -z; }
    void zero()   { x = y = z = 0.0; }
    Flt dot(Myvec v)      { return (x*v.x + y*v.y + z*v.z); }
    Flt lenNoSqrt()       { return (x*x + y*y + z*z); }
    Flt len()             { return sqrtf(lenNoSqrt()); }
    void copy(Myvec b)    { b.x = x; b.y = y; b.z = z; }
    void add(Myvec b)     { x = x + b.x; y = y + b.y; z = z + b.z; }
    void sub(Myvec b)     { x = x - b.x; y = y - b.y; z = z - b.z; }
    void scale(Flt s)     { x *= s; y *= s; z *= s; }
    void addS(Myvec b, Flt s) {
        x = x + b.x * s;
        y = y + b.y * s;
        z = z + b.z * s;
    }
};

#endif // MATH_UTILS_H
