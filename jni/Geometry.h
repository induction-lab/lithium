#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

/*  Basic geometry types, structures and routines */

#include <math.h>
#include <stdlib.h>

#define EPSILON (1e-6)
#define PI (3.14159265358979323846)
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
#define CLAMP(x, lo, hi) (MIN(hi, MAX(lo, x)))

struct Location {
    Location(): x(0.0f), y(0.0f) {};
    Location(float X, float Y): x(X), y(Y) {};
    float x;
    float y;
};

// Calculates radians by degrees
inline float Rad(float Degrees) { return Degrees * (PI / 180.0f); };

// Calculates degrees by radians
inline float Deg(float Radians) { return Radians * (180.0f / PI); };

// Square of x
inline float sqr(float x) { return x * x; };

//-----------------------------------------------------------------------------
//  A vector in 3-space.
//  Typically used to represent points and vectors in homogenous coordinates for
//  use with 4x4 matrices. This is less than ideal and in the future we intend
//  to templatize this class, optimize it with template metaprogramming and
//  offer a range of pre-fab vector classes Vector<2>, Vector<3>, Vector<4> etc.
//-----------------------------------------------------------------------------

class Vector {
public:
    float x, y, z;

    // Constructors
    inline Vector(void): x(0.0f), y(0.0f), z(0.0f) {}
    inline Vector(float X, float Y, float Z): x(X), y(Y), z(Z) {}
    inline Vector(const Vector &v): x(v.x), y(v.y), z(v.z) {}

    // Assignment operator
    inline Vector& operator = (const Vector &v) { this->x = v.x; this->y = v.y; this->z = v.z; return *this; };

    // Determine if two vectors are equal
    inline bool operator == (const Vector &v) const { return (abs(x - v.x) < EPSILON) && (abs(y - v.y) < EPSILON) && (abs(z - v.z) < EPSILON); }
    inline bool operator != (const Vector &v) const { return !(*this==v); }

    // Inverse vector around any axis
    friend inline Vector operator - (const Vector v) { return Vector(-v.x, -v.y, -v.z); }

    // Calculate the multiplication of vector and scalar
    friend inline Vector operator * (const Vector &v, const float s) { return Vector(v.x * s, v.y * s, v.z * s); }
    friend inline Vector operator * (const float s, const Vector &v) { return Vector(v * s); }

    // Calculate the divide of vector and scalar
    friend inline Vector operator / (const Vector &v, const float s) { return Vector(v.x / s, v.y / s, v.z / s); }

    // Calculate the addition of two vectors
    friend inline Vector operator + (const Vector &v1, const Vector &v2) { return Vector(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }

    // Calculate the subtraction of two vector
    friend inline Vector operator - (const Vector &v1, const Vector &v2) { return Vector(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }

    // Calculate the addition of vector and scalar
    friend inline Vector operator + (const Vector &v1, const float s) { return Vector(v1.x + s, v1.y + s, v1.z + s); }
    friend inline Vector operator + (const float s, const Vector &v1) { return Vector(v1 + s); }

    // Calculate the dot product (angle) between 2 vectors
    friend inline float  operator | (const Vector &v1, const Vector &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

    // Calculate a perpendicular vector (cross product) from 2 vectors
    friend inline Vector operator ^ (const Vector A, const Vector B) { return Vector(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x); }

    // Data accessor for easy conversion to float* for OpenGL
    float* data() { return &x; }
};

const Vector EmptyVector    (0.0f, 0.0f, 0.0f);
const Vector IdentityVector (1.0f, 1.0f, 1.0f);
const Vector AxisX          (1.0f, 0.0f, 0.0f);
const Vector AxisY          (0.0f, 1.0f, 0.0f);
const Vector AxisZ          (0.0f, 0.0f, 1.0f);
const Vector InvalidVector  (1e20, 1e20, 1e20);

// Calculate vector length (distance to the origin), using Pythagoras in 3D
inline float Length(Vector v) { return sqrt(v.x * v.x + v.y * v.y + v.z * v.z); };
inline float LengthSquared(Vector v) { return v.x * v.x + v.y * v.y + v.z * v.z; };

// Calculate normalize vector by dividing its components by its length
inline Vector Normalize(Vector v) { float Magnitude = Length(v); if (Magnitude > EPSILON) { v = v * (1 / Magnitude); }; return v; }

// Reflect vector around normal n
inline Vector Reflect(const Vector v, const Vector n) { return ((n * (2 * (-Normalize(v) | n))) + Normalize(v)) * Length(v); }

// Returns the direction vector between two points
inline Vector DirectionVector(const Vector p1, const Vector p2) { return Normalize(p2 - p1); }

// Returns the normal vector of two vectors (the normalized cross product)
inline Vector NormalVector(const Vector v1, const Vector v2) { return Normalize(v1 ^ v2); }

// Returns the normal vector of a triangle or 3 points on a plane (assumes counter-clockwise orientation)
inline Vector NormalVector(const Vector p1, const Vector p2, const Vector p3) { return NormalVector(p2 - p1, p3 - p1); }

// Returns the direction vector between two points

// Performs linear interpolation between two vectors.
inline Vector Lerp(Vector v1, Vector v2, float a) {
    return Vector(
        ((1.0f - a) * v1.x) + (a * v2.x),
        ((1.0f - a) * v1.y) + (a * v2.y),
        ((1.0f - a) * v1.z) + (a * v2.z));
}

inline void PrintVector(Vector v) {
    LOG_DEBUG("%f\t%f\t%f\n", v.x, v.y, v.z);
}

//-----------------------------------------------------------------------------
//  More vector dimensions.
//-----------------------------------------------------------------------------
class Vector2 {
public:
    float x, y;
    
    // Constructors
    inline Vector2(void): x(0.0f), y(0.0f) {}
    inline Vector2(float X, float Y): x(X), y(Y) {}
    inline Vector2(const Vector2 &v): x(v.x), y(v.y) {}
    
    // Assignment operator
    inline Vector2& operator = (const Vector2 &v) { this->x = v.x; this->y = v.y; return *this; };

    // Determine if two vectors are equal
    inline bool operator == (const Vector2 &v) const { return (abs(x - v.x) < EPSILON) && (abs(y - v.y) < EPSILON); }
    inline bool operator != (const Vector2 &v) const { return !(*this==v); }

    // Inverse vector around any axis
    friend inline Vector2 operator - (const Vector2 v) { return Vector2(-v.x, -v.y); }

    // Calculate the multiplication of vector and scalar
    friend inline Vector2 operator * (const Vector2 &v, const float s) { return Vector2(v.x * s, v.y * s); }
    friend inline Vector2 operator * (const float s, const Vector2 &v) { return Vector2(v * s); }

    // Calculate the divide of vector and scalar
    friend inline Vector2 operator / (const Vector2 &v, const float s) { return Vector2(v.x / s, v.y / s); }

    // Calculate the addition of two vectors
    friend inline Vector2 operator + (const Vector2 &v1, const Vector2 &v2) { return Vector2(v1.x + v2.x, v1.y + v2.y); }

    // Calculate the subtraction of two vector
    friend inline Vector2 operator - (const Vector2 &v1, const Vector2 &v2) { return Vector2(v1.x - v2.x, v1.y - v2.y); }

    // Calculate the addition of vector and scalar
    friend inline Vector2 operator + (const Vector2 &v1, const float s) { return Vector2(v1.x + s, v1.y + s); }
    friend inline Vector2 operator + (const float s, const Vector2 &v1) { return Vector2(v1 + s); }

    // Calculate the dot product (angle) between 2 vectors
    friend inline float operator | (const Vector2 &v1, const Vector2 &v2) { return v1.x * v2.x + v1.y * v2.y; }

    // Data accessor for easy conversion to float* for OpenGL
    float* data() { return &x; }    
};

class Vector4 {
public:
    float x, y, z, w;
    
    // Constructors
    inline Vector4(void): x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    inline Vector4(float X, float Y, float Z, float W): x(X), y(Y), z(Z), w(W) {}
    inline Vector4(const Vector4 &v): x(v.x), y(v.y), z(v.z), w(v.w) {}
    
    // Assignment operator
    inline Vector4& operator = (const Vector4 &v) { this->x = v.x; this->y = v.y; this->z = v.z; this->w = v.w; return *this; };    
};


//-----------------------------------------------------------------------------
//  4x4 matrix class.
//
//  The convention here is post-multiplication by a column vector.
//  ie. x = Ab, where x and b are column vectors.
//
//  Please note that in cases where a matrix is pre-multiplied by a
//  vector, we then assume that the vector is a row vector.
//  This operation is then equivalent to post multiplying the column
//  vector by the transpose of the actual matrix.
//
//  If you wish to think of this matrix in terms of basis vectors,
//  then by convention, the rows of this matrix form the set of
//  basis vectors.
//
//  When composing matrix transforms A * B * C * D, note that the
//  actual order of operations as visible in the resultant matrix
//  is D, C, B, A. Alternatively, you can view transforms as changing
//  coordinate system, then the coordinate systems are changed
//  in order A, B, C, D.
//  All matrices are row major. (OpenGL uses column-major matrix)
//-----------------------------------------------------------------------------

class Matrix;
inline Matrix TranslateMatrix(const Vector &v);
inline Matrix ScaleMatrix(const Vector &v);
inline Matrix DiagonalMatrix(float a, float b, float c, float d);
inline Matrix RotateMatrix(float angle, const Vector &axis);
inline Matrix LookAtMatrix(const Vector &eye, const Vector &at, const Vector &up);
inline Matrix OrthographicMatrix(float l, float r, float b, float t, float n, float f);

#define m11 m[ 0]
#define m12 m[ 1]
#define m13 m[ 2]
#define m14 m[ 3]
#define m21 m[ 4]
#define m22 m[ 5]
#define m23 m[ 6]
#define m24 m[ 7]
#define m31 m[ 8]
#define m32 m[ 9]
#define m33 m[10]
#define m34 m[11]
#define m41 m[12]
#define m42 m[13]
#define m43 m[14]
#define m44 m[15]

class Matrix
{
public:

    // float m11,m12,m13,m14;
    // float m21,m22,m23,m24;
    // float m31,m32,m33,m34;
    // float m41,m42,m43,m44;

    float m[16];

    // Default constructor
    inline Matrix(void) {
        m11 = 0.0f; m12 = 0.0f; m13 = 0.0f; m14 = 0.0f;
        m21 = 0.0f; m22 = 0.0f; m23 = 0.0f; m24 = 0.0f;
        m31 = 0.0f; m32 = 0.0f; m33 = 0.0f; m34 = 0.0f;
        m41 = 0.0f; m42 = 0.0f; m43 = 0.0f; m44 = 0.0f;
    }

    // Construct a matrix from explicit values for the 3x3 sub matrix.
    Matrix(float M11, float M12, float M13,
           float M21, float M22, float M23,
           float M31, float M32, float M33) {
        m11 =  M11; m12 =  M12; m13 =  M13; m14 = 0.0f;
        m21 =  M21; m22 =  M22; m23 =  M23; m24 = 0.0f;
        m31 =  M31; m32 =  M32; m33 =  m33; m34 = 0.0f;
        m41 = 0.0f; m42 = 0.0f; m43 = 0.0f; m44 = 1.0f;
    }

    // Construct a matrix from explicit entry values for the whole 4x4 matrix.
    Matrix(float M11, float M12, float M13, float M14,
           float M21, float M22, float M23, float M24,
           float M31, float M32, float M33, float M34,
           float M41, float M42, float M43, float M44) {
        m11 = M11; m12 = M12; m13 = M13; m14 = M14;
        m21 = M21; m22 = M22; m23 = M23; m24 = M24;
        m31 = M31; m32 = M32; m33 = M33; m34 = M34;
        m41 = M41; m42 = M42; m43 = M43; m44 = M44;
    }

    // Assignment operator
    inline Matrix& operator = (const Matrix &m) {
        this->m11 = m.m11; this->m12 = m.m12; this->m13 = m.m13; this->m14 = m.m14;
        this->m21 = m.m21; this->m22 = m.m22; this->m23 = m.m23; this->m24 = m.m24;
        this->m31 = m.m31; this->m32 = m.m32; this->m33 = m.m33; this->m34 = m.m34;
        this->m41 = m.m41; this->m42 = m.m42; this->m43 = m.m43; this->m44 = m.m44;
        return *this;
    };

    // Multiply two matrices
    friend inline Matrix operator * (const Matrix &a, const Matrix &b) {
        return Matrix(
            a.m11 * b.m11 + a.m12 * b.m21 + a.m13 * b.m31 + a.m14 * b.m41,
            a.m11 * b.m12 + a.m12 * b.m22 + a.m13 * b.m32 + a.m14 * b.m42,
            a.m11 * b.m13 + a.m12 * b.m23 + a.m13 * b.m33 + a.m14 * b.m43,
            a.m11 * b.m14 + a.m12 * b.m24 + a.m13 * b.m34 + a.m14 * b.m44,
            a.m21 * b.m11 + a.m22 * b.m21 + a.m23 * b.m31 + a.m24 * b.m41,
            a.m21 * b.m12 + a.m22 * b.m22 + a.m23 * b.m32 + a.m24 * b.m42,
            a.m21 * b.m13 + a.m22 * b.m23 + a.m23 * b.m33 + a.m24 * b.m43,
            a.m21 * b.m14 + a.m22 * b.m24 + a.m23 * b.m34 + a.m24 * b.m44,
            a.m31 * b.m11 + a.m32 * b.m21 + a.m33 * b.m31 + a.m34 * b.m41,
            a.m31 * b.m12 + a.m32 * b.m22 + a.m33 * b.m32 + a.m34 * b.m42,
            a.m31 * b.m13 + a.m32 * b.m23 + a.m33 * b.m33 + a.m34 * b.m43,
            a.m31 * b.m14 + a.m32 * b.m24 + a.m33 * b.m34 + a.m34 * b.m44,
            a.m41 * b.m11 + a.m42 * b.m21 + a.m43 * b.m31 + a.m44 * b.m41,
            a.m41 * b.m12 + a.m42 * b.m22 + a.m43 * b.m32 + a.m44 * b.m42,
            a.m41 * b.m13 + a.m42 * b.m23 + a.m43 * b.m33 + a.m44 * b.m43,
            a.m41 * b.m14 + a.m42 * b.m24 + a.m43 * b.m34 + a.m44 * b.m44
        );
    }

    // Multiply matrix and vector
    friend inline Vector operator * (const Matrix &matrix, const Vector &vector) {
        float x = vector.x * matrix.m11 + vector.y * matrix.m12 + vector.z * matrix.m13 + matrix.m14;
        float y = vector.x * matrix.m21 + vector.y * matrix.m22 + vector.z * matrix.m23 + matrix.m24;
        float z = vector.x * matrix.m31 + vector.y * matrix.m32 + vector.z * matrix.m33 + matrix.m34;
        return Vector(x, y, z);
    }

    // When we premultiply x * A we assume the vector is a row vector
    friend inline Vector operator * (const Vector &vector, const Matrix &matrix) {
        float x = vector.x * matrix.m11 + vector.y * matrix.m21 + vector.z * matrix.m31 + matrix.m41;
        float y = vector.x * matrix.m12 + vector.y * matrix.m22 + vector.z * matrix.m32 + matrix.m42;
        float z = vector.x * matrix.m13 + vector.y * matrix.m23 + vector.z * matrix.m33 + matrix.m43;
        return Vector(x, y, z);
    }

    // Multiply matrix and scalar
    friend inline Matrix operator * (const Matrix &m, float s) {
        return Matrix(
            s * m.m11, s * m.m12, s * m.m13, s * m.m14,
            s * m.m21, s * m.m22, s * m.m23, s * m.m24,
            s * m.m31, s * m.m32, s * m.m33, s * m.m34,
            s * m.m41, s * m.m42, s * m.m43, s * m.m44
        );
    }

    // Matrix transformation (translate)
    inline void Translate(float x, float y, float z) {
        m14 += m11 * x + m12 * y + m13 * z;
        m24 += m21 * x + m22 * y + m23 * z;
        m34 += m31 * x + m32 * y + m33 * z;
        m44 += m41 * x + m42 * y + m43 * z;
    }

    // Matrix transformation (scale)
    inline void Scale(float x, float y, float z) {
        m11 *= x; m12 *= y; m13 *= z;
        m12 *= x; m22 *= y; m23 *= z;
        m13 *= x; m32 *= y; m33 *= z;
        m14 *= x; m42 *= y; m43 *= z;
    }

    // Matrix transformation (rotate)
    inline void Rotate(float angle, const Vector &axis) {
        Matrix m;
        float r = Rad(angle);
        float c = cosf(r);
        float s = sinf(r);
        m.m11 = axis.x * axis.x * (1.0f - c) + c;
        m.m12 = axis.x * axis.y * (1.0f - c) - axis.z * s;
        m.m13 = axis.x * axis.z * (1.0f - c) + axis.y * s;
        m.m21 = axis.y * axis.x * (1.0f - c) + axis.z * s;
        m.m22 = axis.y * axis.y * (1.0f - c) + c;
        m.m23 = axis.y * axis.z * (1.0f - c) - axis.x * s;
        m.m31 = axis.x * axis.z * (1.0f - c) - axis.y * s;
        m.m32 = axis.y * axis.z * (1.0f - c) + axis.x * s;
        m.m33 = axis.z * axis.z * (1.0f - c) + c;
        m.m41 = m.m42 = m.m43 = 0.0f;
        m.m44 = 1.0f;
        *this = *this * m;
    }

    // Cute access to matrix elements via overloaded () operator.
    inline float& operator () (int i, int j) {
        float *data = &m11;
        return data[(i<<2) + j];
    }

    // Const version of element access above
    const float& operator () (int i, int j) const {
        const float *data = &m11;
        return data[(i<<2) + j];
    }

    // Data accessor for easy conversion to float* for OpenGL
    float* data() { return &m11; }

};

const Matrix EmptyMatrix(
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
);

const Matrix IdentityMatrix(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
);

// Set to a translation matrix
inline Matrix TranslateMatrix(const Vector &v) {
    return Matrix (
        1.0f, 0.0f, 0.0f,  v.x,
        0.0f, 1.0f, 0.0f,  v.y,
        0.0f, 0.0f, 1.0f,  v.z,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

// Set to a scale matrix
inline Matrix ScaleMatrix(const Vector &v) {
    return Matrix (
         v.x, 0.0f, 0.0f, 0.0f,
        0.0f,  v.y, 0.0f, 0.0f,
        0.0f, 0.0f,  v.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

// Set to a diagonal matrix
inline Matrix DiagonalMatrix(float a, float b, float c, float d = 1.0f) {
    return Matrix (
           a, 0.0f, 0.0f, 0.0f,
        0.0f,    b, 0.0f, 0.0f,
        0.0f, 0.0f,    c, 0.0f,
        0.0f, 0.0f, 0.0f,    d
    );
}

// Set to a rotation matrix about a specified axis and angle.
inline Matrix RotateMatrix(float angle, const Vector &axis) {
    Matrix m = IdentityMatrix;
    if (sqr(Length(axis)) < sqr(EPSILON)) return IdentityMatrix;
    float r = Rad(angle);
    float c = cosf(r);
    float s = sinf(r);
    m.m11 = axis.x * axis.x * (1.0f - c) + c;
    m.m12 = axis.x * axis.y * (1.0f - c) - axis.z * s;
    m.m13 = axis.x * axis.z * (1.0f - c) + axis.y * s;
    m.m21 = axis.y * axis.x * (1.0f - c) + axis.z * s;
    m.m22 = axis.y * axis.y * (1.0f - c) + c;
    m.m23 = axis.y * axis.z * (1.0f - c) - axis.x * s;
    m.m31 = axis.x * axis.z * (1.0f - c) - axis.y * s;
    m.m32 = axis.y * axis.z * (1.0f - c) + axis.x * s;
    m.m33 = axis.z * axis.z * (1.0f - c) + c;
    return m;
}

// Set to an orthographic projection m.matrix. ???
inline Matrix OrthographicMatrix(float l, float r, float b, float t, float n, float f) {
    float sx = 1 / (r - l);
    float sy = 1 / (t - b);
    float sz = 1 / (f - n);
    return Matrix(
        sx * 2.0f,      0.0f,     0.0f, -(r+l) * sx,
             0.0f, sy * 2.0f,     0.0f, -(t+b) * sy,
             0.0f,      0.0f,sz * 2.0f, -(n+f) * sz,
             0.0f,      0.0f,     0.0f,        1.0f
        );
}

// Set to a look at matrix
inline Matrix LookAtMatrix(const Vector &eye, const Vector &at, const Vector &up) {
    Vector z_axis = Normalize(at - eye);
    Vector x_axis = Normalize(up ^ z_axis);
    Vector y_axis = Normalize(z_axis ^ x_axis);
    return Matrix(
        x_axis.x, x_axis.y, x_axis.z, -(x_axis | eye),
        y_axis.x, y_axis.y, y_axis.z, -(y_axis | eye),
        z_axis.x, z_axis.y, z_axis.z, -(z_axis | eye),
            0.0f,     0.0f,     0.0f,            1.0f
    );
}

// Set to a frustum projection matrix
inline Matrix FrustumMatrix(float l, float r, float t, float b, float n, float f) {
    return Matrix(
        2 * n / (r-l),          0.0f,      0.0f,          0.0f,
                 0.0f, 2 * n / (t-b),      0.0f,          0.0f,
                 0.0f,          0.0f, f / (f-n), n * f / (n-f),
                 0.0f,          0.0f,      1.0f,          0.0f
        );
}

// Set to a perspective projection m.matrix specified in term.ms of field of view and aspect ratio.
inline Matrix PerspectiveMatrix(float fov, float aspect, float n, float f) {
    const float t = tan(fov * 0.5f) * n;
    const float b = -t;
    const float l = aspect * b;
    const float r = aspect * t;
    return FrustumMatrix(l, r, t, b, n, f);
}

// Calculate inverse of matrix
inline Matrix Inverse(Matrix m) {
    Matrix inv;
    inv.m11 = m.m22 * m.m33 * m.m44 - m.m22 * m.m34 * m.m43 - m.m32 * m.m23 * m.m44 + m.m32 * m.m24 * m.m43 + m.m42 * m.m23 * m.m34 - m.m42 * m.m24 * m.m33;
    inv.m21 =-m.m21 * m.m33 * m.m44 + m.m21 * m.m34 * m.m43 + m.m31 * m.m23 * m.m44 - m.m31 * m.m24 * m.m43 - m.m41 * m.m23 * m.m34 + m.m41 * m.m24 * m.m33;
    inv.m31 = m.m21 * m.m32 * m.m44 - m.m21 * m.m34 * m.m42 - m.m31 * m.m22 * m.m44 + m.m31 * m.m24 * m.m42 + m.m41 * m.m22 * m.m34 - m.m41 * m.m24 * m.m32;
    inv.m41 =-m.m21 * m.m32 * m.m43 + m.m21 * m.m33 * m.m42 + m.m31 * m.m22 * m.m43 - m.m31 * m.m23 * m.m42 - m.m41 * m.m22 * m.m33 + m.m41 * m.m23 * m.m32;
    inv.m12 =-m.m12 * m.m33 * m.m44 + m.m12 * m.m34 * m.m43 + m.m32 * m.m13 * m.m44 - m.m32 * m.m14 * m.m43 - m.m42 * m.m13 * m.m34 + m.m42 * m.m14 * m.m33;
    inv.m22 = m.m11 * m.m33 * m.m44 - m.m11 * m.m34 * m.m43 - m.m31 * m.m13 * m.m44 + m.m31 * m.m14 * m.m43 + m.m41 * m.m13 * m.m34 - m.m41 * m.m14 * m.m33;
    inv.m32 =-m.m11 * m.m32 * m.m44 + m.m11 * m.m34 * m.m42 + m.m31 * m.m12 * m.m44 - m.m31 * m.m14 * m.m42 - m.m41 * m.m12 * m.m34 + m.m41 * m.m14 * m.m32;
    inv.m42 = m.m11 * m.m32 * m.m43 - m.m11 * m.m33 * m.m42 - m.m31 * m.m12 * m.m43 + m.m31 * m.m13 * m.m42 + m.m41 * m.m12 * m.m33 - m.m41 * m.m13 * m.m32;
    inv.m13 = m.m12 * m.m23 * m.m44 - m.m12 * m.m24 * m.m43 - m.m22 * m.m13 * m.m44 + m.m22 * m.m14 * m.m43 + m.m42 * m.m13 * m.m24 - m.m42 * m.m14 * m.m23;
    inv.m23 =-m.m11 * m.m23 * m.m44 + m.m11 * m.m24 * m.m43 + m.m21 * m.m13 * m.m44 - m.m21 * m.m14 * m.m43 - m.m41 * m.m13 * m.m24 + m.m41 * m.m14 * m.m23;
    inv.m33 = m.m11 * m.m22 * m.m44 - m.m11 * m.m24 * m.m42 - m.m21 * m.m12 * m.m44 + m.m21 * m.m14 * m.m42 + m.m41 * m.m12 * m.m24 - m.m41 * m.m14 * m.m22;
    inv.m43 =-m.m11 * m.m22 * m.m43 + m.m11 * m.m23 * m.m42 + m.m21 * m.m12 * m.m43 - m.m21 * m.m13 * m.m42 - m.m41 * m.m12 * m.m23 + m.m41 * m.m13 * m.m22;
    inv.m14 =-m.m12 * m.m23 * m.m34 + m.m12 * m.m24 * m.m33 + m.m22 * m.m13 * m.m34 - m.m22 * m.m14 * m.m33 - m.m32 * m.m13 * m.m24 + m.m32 * m.m14 * m.m23;
    inv.m24 = m.m11 * m.m23 * m.m34 - m.m11 * m.m24 * m.m33 - m.m21 * m.m13 * m.m34 + m.m21 * m.m14 * m.m33 + m.m31 * m.m13 * m.m24 - m.m31 * m.m14 * m.m23;
    inv.m34 =-m.m11 * m.m22 * m.m34 + m.m11 * m.m24 * m.m32 + m.m21 * m.m12 * m.m34 - m.m21 * m.m14 * m.m32 - m.m31 * m.m12 * m.m24 + m.m31 * m.m14 * m.m22;
    inv.m44 = m.m11 * m.m22 * m.m33 - m.m11 * m.m23 * m.m32 - m.m21 * m.m12 * m.m33 + m.m21 * m.m13 * m.m32 + m.m31 * m.m12 * m.m23 - m.m31 * m.m13 * m.m22;
    float det = 1.0f / (m.m11 * inv.m11 + m.m12 * inv.m21 + m.m13 * inv.m31 + m.m14 * inv.m41);
    return inv * det;
}

// Calculate transpose of matrix
inline Matrix Transpose(Matrix m) {
    return Matrix(
        m.m11, m.m21, m.m31, m.m41,
        m.m12, m.m22, m.m32, m.m42,
        m.m13, m.m23, m.m33, m.m43,
        m.m14, m.m24, m.m34, m.m44
    );
}

// Transforms a 3D vector by a given matrix, projecting the result back into w = 1
inline Vector TransformCoord(const Vector &v, const Matrix &m) {

    float x = v.x * m.m11 + v.y * m.m12 + v.z * m.m13 + m.m14;
    float y = v.x * m.m21 + v.y * m.m22 + v.z * m.m23 + m.m24;
    float z = v.x * m.m31 + v.y * m.m32 + v.z * m.m33 + m.m34;
    float w = m.m41 * v.x + m.m42 * v.y + m.m43 * v.z + m.m44;
    if (w != 0.0f) return Vector(x, y, z) / w;
    else return EmptyVector;
}

// Transforms a 3D vector by a given matrix, projecting the result back into w = 1
inline Vector TransformNormal(const Vector &v, const Matrix &m) {

    float x = v.x * m.m11 + v.y * m.m12 + v.z * m.m13;
    float y = v.x * m.m21 + v.y * m.m22 + v.z * m.m23;
    float z = v.x * m.m31 + v.y * m.m32 + v.z * m.m33;
    float w = m.m41 * v.x + m.m42 * v.y + m.m43 * v.z + m.m44;
    if (w != 0.0f) return Vector(x, y, z) / w;
    else return EmptyVector;
}

inline void PrintMatrix(Matrix m) {
    LOG_DEBUG("%f\t%f\t%f\t%f\n", m.m11, m.m12, m.m13, m.m14);
    LOG_DEBUG("%f\t%f\t%f\t%f\n", m.m21, m.m22, m.m23, m.m24);
    LOG_DEBUG("%f\t%f\t%f\t%f\n", m.m31, m.m32, m.m33, m.m34);
    LOG_DEBUG("%f\t%f\t%f\t%f\n", m.m41, m.m42, m.m43, m.m44);
}

//-----------------------------------------------------------------------------
//  A quaternion.
//  This quaternion class is generic and may be non-unit, however most
//  anticipated uses of quaternions are typically unit cases representing
//  a rotation 2 * acos(w) about the axis (x,y,z).
//-----------------------------------------------------------------------------

class Quaternion
{
public:

    float w;        // w component of quaternion
    float x;        // x component of quaternion
    float y;        // y component of quaternion
    float z;        // z component of quaternion

    // Default constructor.
    inline Quaternion(void): w(0.0f), x(0.0f), y(0.0f), z(0.0f) {}

    // Construct quaternion from real component w and imaginary x,y,z.
    inline Quaternion(float W, float X, float Y, float Z): w(W), x(X), y(Y), z(Z) {}


    // Construct quaternion from angle-axis
    inline Quaternion(float angle, const Vector &axis)
    {
        const float a = angle * 0.5f;
        const float s = (float) sin(a);
        const float c = (float) cos(a);
        w = c;
        x = axis.x * s;
        y = axis.y * s;
        z = axis.z * s;
    }

    // Negate quaternion
    friend inline Quaternion operator - (const Quaternion &a) { return Quaternion(-a.w, -a.x, -a.y, -a.z); }

    // Calculate the subtraction of two quaternions
    friend inline Quaternion operator - (const Quaternion &a, const Quaternion &b)  {
        return Quaternion(a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z);
    }

    // Calculate the addition of two quaternions
    friend inline Quaternion operator + (const Quaternion &a, const Quaternion &b) {
        return Quaternion(a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z);
    }

    // Calculate the multiply of two quaternions
    friend inline Quaternion operator * (const Quaternion &a, const Quaternion &b) {
        return Quaternion(
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w
        );
    }

    // Calculate the multiply of scalar and quaternion
    friend inline Quaternion operator * (float s, const Quaternion &a) {
        return Quaternion(a.w * s, a.x * s, a.y * s, a.z * s);
    }

    // Calculate the multiply of quaternion and scalar
    friend inline Quaternion operator * (const Quaternion &a, float s) {
        return Quaternion(a.w * s, a.x * s, a.y * s, a.z * s);
    }

    // Dot product of two quaternions
    friend inline Quaternion operator | (const Quaternion &a, const Quaternion &b) {
        return Quaternion(a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z, 0, 0, 0);
    }

    // Convert quaternion to angle-axis
    inline void AngleAxis(float &angle, Vector &axis) const {
        const float SquareLength = x * x + y * y + z * z;
        if (SquareLength > sqr(EPSILON)) {
            angle = 2.0f * (float) acos(w);
            const float InverseLength = 1.0f / (float) pow(SquareLength, 0.5f);
            axis.x = x * InverseLength;
            axis.y = y * InverseLength;
            axis.z = z * InverseLength;
        } else {
            angle  = 0.0f;
            axis.x = 1.0f;
            axis.y = 0.0f;
            axis.z = 0.0f;
        }
    }
};

const Quaternion EmptyQuaternion    (0.0f, 0.0f, 0.0f, 0.0f);
const Quaternion IdentityQuaternion (1.0f, 0.0f, 0.0f, 0.0f);

// Convert quaternion to matrix
inline Matrix matrix(const Quaternion &q) {
    float x  = 2.0f * q.x;
    float y  = 2.0f * q.y;
    float z  = 2.0f * q.z;
    float wx = 2.0f * q.x * q.w;
    float wy = 2.0f * q.y * q.w;
    float wz = 2.0f * q.z * q.w;
    float xx = 2.0f * q.x * q.x;
    float xy = 2.0f * q.y * q.x;
    float xz = 2.0f * q.z * q.x;
    float yy = 2.0f * q.y * q.y;
    float yz = 2.0f * q.z * q.y;
    float zz = 2.0f * q.z * q.z;
    return Matrix(1.0f - (yy + zz),         xy - wz ,         xz + wy,
                          xy + wz , 1.0f - (xx + zz),         yz - wx,
                          xz - wy ,         yz + wx , 1.0f - (xx + yy));
}

// Convert rotation matrix to quaternion
inline Quaternion quaternion(const Matrix &m) {
    float w = 0, x = 0, y = 0, z = 0;
    float root;
    float trace = 1 + m.m11 + m.m22 + m.m33;
    if (trace > EPSILON)
    {
        root = (float) sqrt(trace * 2);
        w = 0.25f * root;
        x = (m.m23 - m.m32) / root;
        y = (m.m31 - m.m13) / root;
        z = (m.m12 - m.m21) / root;
    } else {
        if (m.m11 > m.m22 && m.m11 > m.m33) {
            // Column 0:
            root = (float) sqrt(1.0f + m.m11 - m.m22 - m.m33) * 2;
            w = (m.m23 - m.m32) / root;
            x = 0.25f * root;
            y = (m.m12 + m.m21) / root;
            z = (m.m31 + m.m13) / root;
        } else if (m.m22 > m.m33) {
            // Column 1:
            root = (float) sqrt(1.0f + m.m22 - m.m11 - m.m33) * 2;
            w = (m.m31 - m.m13) / root;
            x = (m.m12 + m.m21) / root;
            y = 0.25f * root;
            z = (m.m23 + m.m32) / root;
        } else {
            // Column 2:
            root = (float) sqrt(1.0f + m.m33 - m.m11 - m.m22) * 2;
            w = (m.m12 - m.m21) / root;
            x = (m.m31 + m.m13) / root;
            y = (m.m23 + m.m32) / root;
            z = 0.25f * root;
        }
    }
    return Quaternion(w, x, y, z);
}

// Calculate length of quaternion
inline float Length(const Quaternion &q) {
    return sqrt(sqr(q.w) + sqr(q.x) + sqr(q.y) + sqr(q.z));
}

// Calculate conjugate of quaternion
inline Quaternion Conjugate(Quaternion q) {
    return Quaternion(q.w, -q.x, -q.y, -q.z);
}

// Normalize quaternion.
inline Quaternion Normalize(const Quaternion &q) {
    const float length = Length(q);
    if (length == 0) {
        return IdentityQuaternion;
    } else {
        float inv = 1.0f / length;
        return Quaternion(q.w * inv, q.x * inv, q.y * inv, q.z * inv);
    }
}

// Slerp of two quaternions
inline Quaternion Slerp(const Quaternion &a, const Quaternion &b, float t) {
    float flip = 1.0f;
    float cosine = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
    if (cosine < 0.0f) {
        cosine = -cosine;
        flip = -1.0f;
    }
    if ((1.0f - cosine) < EPSILON) return a * (1-t) + b * (t*flip);
    float theta = (float)acos(cosine);
    float sine  = (float)sin(theta);
    float beta  = (float)sin((1.0f-t)*theta) / sine;
    float alpha = (float)sin(t*theta) / sine * flip;
    return a * beta + b * alpha;
}

inline void PrintQuaternion(Quaternion q) {
    LOG_DEBUG("%f\t%f\t%f\t%f\n", q.w, q.x, q.y, q.z);
}

// ----------------------------------------------------------------------------
// Describes a plane form of the general plane equation. They fit into the
// general plane equation so that ax + by + cz + dw = 0.
// ----------------------------------------------------------------------------
class Plane
{
public:
    float a, b, c, d;

    // Constructors
    inline Plane(void): a(0.0f), b(0.0f), c(0.0f), d(0.0f) {}
    inline Plane(float A, float B, float C, float D): a(A), b(B), c(C), d(D) {}
    inline Plane(const Vector &normal, float distance): a(normal.x), b(normal.y), c(normal.z), d(distance) {}
    inline Plane(const Vector &point, const Vector &normal) { *this = Plane(normal, point | normal); }
    inline Vector GetNormal() { return Vector(a, b, c); }
};

// Normalizes the plane coefficients so that the plane normal has unit length.
Plane Normalize(Plane plane) {
    float Distance = sqrtf(plane.a * plane.a + plane.b * plane.b + plane.c * plane.c);
    float a = plane.a / Distance;
    float b = plane.b / Distance;
    float c = plane.c / Distance;
    float d = plane.d / Distance;
    return Plane(a, b, c, d);
}

// Determining the plane's relationship with a coordinate in 3D space
// ax + by + cz + d
inline float PlaneDotCoord(const Plane &p, const Vector &d) {
    return p.a * d.x + p.b * d.y + p.c * d.z + p.d * 1.0f;
}

// Calculating the angle between the normal of the plane, and another normal
// ax + by + cz + 0
inline float PlaneDotNormal(const Plane &p, const Vector &d) {
    return p.a * d.x + p.b * d.y + p.c * d.z;
}

// Get the intersection between a line and a plane
Vector PlaneIntersectLine(Plane p, Vector v1, Vector v2) {
    Vector diff = v1 - v2;
    float Denominator = PlaneDotNormal(p, diff);
    if(Denominator == 0.0f) return (v1 + v2) * 0.5f;
    float u = PlaneDotCoord(p, v1) / Denominator;
    return (v1 + u * (v2 - v1));
}

// ----------------------------------------------------------------------------
// Texture coordinates specify the point in the texture image.
// ----------------------------------------------------------------------------
class TexCoord {
public:
    float u, v;
    TexCoord(void) {}
    TexCoord(float U, float V): u(U), v(V) {}
};

// ----------------------------------------------------------------------------
// Structure defines the coordinates of the upper-left and lower-right corners of a rectangle.
// ----------------------------------------------------------------------------
class Rect {
public:
    float left, top, right, bottom;

    // Constructors
    inline Rect(void): left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
    inline Rect(float Left, float Top, float Right, float Bottom): left(Left), top(Top), right(Right), bottom(Bottom) {}
};

#endif // __GEOMETRY_H__