#ifndef __MATH_H__
#define __MATH_H__

#include <cmath>
#include <cassert>

template<class T>
inline const T& MIN(const T& a, const T& b) {
    return (a < b) ? a : b;
}

template<class T>
inline const T& MAX(const T& a, const T& b) {
    return (a > b) ? a : b;
}

inline uint32_t UpperPow2(uint32_t v) {
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return ++v;
}

class Vector3 {
public:
    float m_x;
    float m_y;
    float m_z;
    Vector3(): m_x(0.0f), m_y(0.0f), m_z(0.0f) {}
    Vector3(const float x, const float y, const float z): m_x(x), m_y(y), m_z(z) {}
    ~Vector3() {};
    void Set(const Vector3& in) {
        m_x = in.m_x;
        m_y = in.m_y;
        m_z = in.m_z;
    }
    void Multiply(const float scalar) {
        m_x *= scalar;
        m_y *= scalar;
        m_z *= scalar;
    }
    void Divide(const float scalar) {
        float divisor = 1.0f / scalar;
        m_x *= divisor;
        m_y *= divisor;
        m_z *= divisor;
    }
    void Add(const Vector3& in) {
        m_x += in.m_x;
        m_y += in.m_y;
        m_z += in.m_z;
    }
    void Subtract(const Vector3& in) {
        m_x -= in.m_x;
        m_y -= in.m_y;
        m_z -= in.m_z;
    }
    void Negate() {
        m_x = -m_x;
        m_y = -m_y;
        m_z = -m_z;
    }
    Vector3& operator=(const Vector3& in) {
        m_x = in.m_x;
        m_y = in.m_y;
        m_z = in.m_z;
        return *this;
    }
    float Length() const {
        return std::sqrt((m_x*m_x) + (m_y*m_y) + (m_z*m_z));
    }
    float LengthSquared() const {
        return (m_x*m_x) + (m_y*m_y) + (m_z*m_z);
    }
    void Normalize() {
        Divide(Length());
    }
    void GetNormal(Vector3& normal) {
        normal = *this;
        normal.Normalize();
    }
    float Dot(const Vector3& in) const {
        return (m_x * in.m_x) + (m_y * in.m_y) + (m_z * in.m_z);
    }
    Vector3 Cross(const Vector3& in) const {
        return Vector3(
                   (m_y * in.m_z) - (m_z * in.m_y),
                   (m_z * in.m_x) - (m_x * in.m_z),
                   (m_x * in.m_y) - (m_y * in.m_x));
    }
};

class Vector4 {
public:
    float m_x;
    float m_y;
    float m_z;
    float m_w;
    Vector4(): m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(0.0f) {}
    Vector4(const float x, const float y, const float z, const float w): m_x(x), m_y(y), m_z(z), m_w(w) {}
    ~Vector4() {}
    void Set(Vector4& in) {
        m_x = in.m_x;
        m_y = in.m_y;
        m_z = in.m_z;
        m_w = in.m_w;
    }
    void Multiply(float scalar) {
        m_x *= scalar;
        m_y *= scalar;
        m_z *= scalar;
        m_w *= scalar;
    }
    void Divide(float scalar) {
        float divisor = 1.0f / scalar;
        m_x *= divisor;
        m_y *= divisor;
        m_z *= divisor;
        m_w *= divisor;
    }
    void Add(Vector4& in)	{
        m_x += in.m_x;
        m_y += in.m_y;
        m_z += in.m_z;
        m_w += in.m_w;
    }
    void Subtract(Vector4& in) {
        m_x -= in.m_x;
        m_y -= in.m_y;
        m_z -= in.m_z;
        m_w -= in.m_w;
    }
    Vector4& operator=(const Vector4& in) {
        m_x = in.m_x;
        m_y = in.m_y;
        m_z = in.m_z;
        m_w = in.m_w;
        return *this;
    }
};

class Matrix3 {
public:
    float m_m[9];
    Matrix3() {}
    ~Matrix3() {}
    void Identify() {
        m_m[0] = 1.0f;
        m_m[1] = 0.0f;
        m_m[2] = 0.0f;
        m_m[3] = 0.0f;
        m_m[4] = 1.0f;
        m_m[5] = 0.0f;
        m_m[6] = 0.0f;
        m_m[7] = 0.0f;
        m_m[8] = 1.0f;
    }
    Vector3 Transform(const Vector3& in) const {
        return Vector3(	(m_m[0] * in.m_x) + (m_m[1] * in.m_y) + (m_m[2] * in.m_y),
                        (m_m[3] * in.m_x) + (m_m[4] * in.m_y) + (m_m[5] * in.m_y),
                        (m_m[6] * in.m_x) + (m_m[7] * in.m_y) + (m_m[8] * in.m_y));
    }
    Vector3 TransformTranspose(const Vector3& in) const {
        return Vector3((m_m[0] * in.m_x) + (m_m[3] * in.m_y) + (m_m[6] * in.m_y),
                       (m_m[1] * in.m_x) + (m_m[4] * in.m_y) + (m_m[7] * in.m_y),
                       (m_m[2] * in.m_x) + (m_m[5] * in.m_y) + (m_m[8] * in.m_y));
    }
    void RotateAroundX(const float radians) {
        m_m[0] = 1.0f;
        m_m[1] = 0.0f;
        m_m[2] = 0.0f;
        m_m[3] = 0.0f;
        m_m[4] = cos(radians);
        m_m[5] = sin(radians);
        m_m[6] = 0.0f;
        m_m[7] = -sin(radians);
        m_m[8] = cos(radians);
    }
    void RotateAroundY(const float radians) {
        m_m[0] = cos(radians);
        m_m[1] = 0.0f;
        m_m[2] = -sin(radians);
        m_m[3] = 0.0f;
        m_m[4] = 1.0f;
        m_m[5] = 0.0f;
        m_m[6] = sin(radians);
        m_m[7] = 0.0f;
        m_m[8] = cos(radians);
    }
    void RotateAroundZ(const float radians) {
        m_m[0] = cos(radians);
        m_m[1] = sin(radians);
        m_m[2] = 0.0f;
        m_m[3] = -sin(radians);
        m_m[4] = cos(radians);
        m_m[5] = 0.0f;
        m_m[6] = 0.0f;
        m_m[7] = 0.0f;
        m_m[8] = 1.0f;
    }
    Matrix3& operator=(const Matrix3& in) {
        m_m[0] = in.m_m[0];
        m_m[1] = in.m_m[1];
        m_m[2] = in.m_m[2];
        m_m[3] = in.m_m[3];
        m_m[4] = in.m_m[4];
        m_m[5] = in.m_m[5];
        m_m[6] = in.m_m[6];
        m_m[7] = in.m_m[7];
        m_m[8] = in.m_m[8];
        return *this;
    }
    Matrix3 Multiply(const Matrix3& input) const {
        Matrix3 result;
        result.m_m[0] = (m_m[0] * input.m_m[0]) + (m_m[1] * input.m_m[3]) + (m_m[2] * input.m_m[6]);
        result.m_m[1] = (m_m[0] * input.m_m[1]) + (m_m[1] * input.m_m[4]) + (m_m[2] * input.m_m[7]);
        result.m_m[2] = (m_m[0] * input.m_m[2]) + (m_m[1] * input.m_m[5]) + (m_m[2] * input.m_m[8]);
        result.m_m[3] = (m_m[3] * input.m_m[0]) + (m_m[4] * input.m_m[3]) + (m_m[5] * input.m_m[6]);
        result.m_m[4] = (m_m[3] * input.m_m[1]) + (m_m[4] * input.m_m[4]) + (m_m[5] * input.m_m[7]);
        result.m_m[5] = (m_m[3] * input.m_m[2]) + (m_m[4] * input.m_m[5]) + (m_m[5] * input.m_m[8]);
        result.m_m[6] = (m_m[6] * input.m_m[0]) + (m_m[7] * input.m_m[3]) + (m_m[8] * input.m_m[6]);
        result.m_m[7] = (m_m[6] * input.m_m[1]) + (m_m[7] * input.m_m[4]) + (m_m[8] * input.m_m[7]);
        result.m_m[8] = (m_m[6] * input.m_m[2]) + (m_m[7] * input.m_m[5]) + (m_m[8] * input.m_m[8]);
        return result;
    }
};

class Matrix4 {
public:
    enum Rows { X, Y, Z, W, NUM_ROWS };
    float m_m[16];
    Matrix4() {}
    ~Matrix4() {}
    void Identify() {
        m_m[0] = 1.0f;
        m_m[1] = 0.0f;
        m_m[2] = 0.0f;
        m_m[3] = 0.0f;
        m_m[4] = 0.0f;
        m_m[5] = 1.0f;
        m_m[6] = 0.0f;
        m_m[7] = 0.0f;
        m_m[8] = 0.0f;
        m_m[9] = 0.0f;
        m_m[10] = 1.0f;
        m_m[11] = 0.0f;
        m_m[12] = 0.0f;
        m_m[13] = 0.0f;
        m_m[14] = 0.0f;
        m_m[15] = 1.0f;
    }
    Vector3 Transform(const Vector3& in) const {
        return Vector3((m_m[0] * in.m_x) + (m_m[1] * in.m_y) + (m_m[2] * in.m_z),
                       (m_m[4] * in.m_x) + (m_m[5] * in.m_y) + (m_m[6] * in.m_z),
                       (m_m[6] * in.m_x) + (m_m[7] * in.m_y) + (m_m[8] * in.m_z));
    }
    Vector3 TransformTranspose(const Vector3& in) const {
        return Vector3((m_m[0] * in.m_x) + (m_m[3] * in.m_y) + (m_m[6] * in.m_z),
                       (m_m[1] * in.m_x) + (m_m[4] * in.m_y) + (m_m[7] * in.m_z),
                       (m_m[2] * in.m_x) + (m_m[5] * in.m_y) + (m_m[8] * in.m_z));
    }
    Vector4 Multiply(const Vector4& in) const {
        return Vector4((m_m[0]  * in.m_x) + (m_m[1]  * in.m_y) + (m_m[2]  * in.m_z) + (m_m[3]  * in.m_w),
                       (m_m[4]  * in.m_x) + (m_m[5]  * in.m_y) + (m_m[6]  * in.m_z) + (m_m[7]  * in.m_w),
                       (m_m[8]  * in.m_x) + (m_m[9]  * in.m_y) + (m_m[10] * in.m_z) + (m_m[11] * in.m_w),
                       (m_m[12] * in.m_x) + (m_m[13] * in.m_y) + (m_m[14] * in.m_z) + (m_m[15] * in.m_w));
    }
    void RotateAroundX(float radians) {
        m_m[0] = 1.0f;
        m_m[1] = 0.0f;
        m_m[2] = 0.0f;
        m_m[4] = 0.0f;
        m_m[5] = cos(radians);
        m_m[6] = sin(radians);
        m_m[8] = 0.0f;
        m_m[9] = -sin(radians);
        m_m[10] = cos(radians);
    }
    void RotateAroundY(float radians) {
        m_m[0] = cos(radians);
        m_m[1] = 0.0f;
        m_m[2] = -sin(radians);
        m_m[4] = 0.0f;
        m_m[5] = 1.0f;
        m_m[6] = 0.0f;
        m_m[8] = sin(radians);
        m_m[9] = 0.0f;
        m_m[10] = cos(radians);
    }
    void RotateAroundZ(float radians) {
        m_m[0] = cos(radians);
        m_m[1] = sin(radians);
        m_m[2] = 0.0f;
        m_m[4] = -sin(radians);
        m_m[5] = cos(radians);
        m_m[6] = 0.0f;
        m_m[8] = 0.0f;
        m_m[9] = 0.0f;
        m_m[10] = 1.0f;
    }
    void Multiply(const Matrix4& in, Matrix4& out) const {
        assert(this != &in && this != &out && &in != &out);
        out.m_m[0]  = (m_m[0]  * in.m_m[0]) + (m_m[1]  * in.m_m[4]) + (m_m[2]  * in.m_m[8])  + (m_m[3]  * in.m_m[12]);
        out.m_m[1]  = (m_m[0]  * in.m_m[1]) + (m_m[1]  * in.m_m[5]) + (m_m[2]  * in.m_m[9])  + (m_m[3]  * in.m_m[13]);
        out.m_m[2]  = (m_m[0]  * in.m_m[2]) + (m_m[1]  * in.m_m[6]) + (m_m[2]  * in.m_m[10]) + (m_m[3]  * in.m_m[14]);
        out.m_m[3]  = (m_m[0]  * in.m_m[3]) + (m_m[1]  * in.m_m[7]) + (m_m[2]  * in.m_m[11]) + (m_m[3]  * in.m_m[15]);
        out.m_m[4]  = (m_m[4]  * in.m_m[0]) + (m_m[5]  * in.m_m[4]) + (m_m[6]  * in.m_m[8])  + (m_m[7]  * in.m_m[12]);
        out.m_m[5]  = (m_m[4]  * in.m_m[1]) + (m_m[5]  * in.m_m[5]) + (m_m[6]  * in.m_m[9])  + (m_m[7]  * in.m_m[13]);
        out.m_m[6]  = (m_m[4]  * in.m_m[2]) + (m_m[5]  * in.m_m[6]) + (m_m[6]  * in.m_m[10]) + (m_m[7]  * in.m_m[14]);
        out.m_m[7]  = (m_m[4]  * in.m_m[3]) + (m_m[5]  * in.m_m[7]) + (m_m[6]  * in.m_m[11]) + (m_m[7]  * in.m_m[15]);
        out.m_m[8]  = (m_m[8]  * in.m_m[0]) + (m_m[9]  * in.m_m[4]) + (m_m[10] * in.m_m[8])  + (m_m[11] * in.m_m[12]);
        out.m_m[9]  = (m_m[8]  * in.m_m[1]) + (m_m[9]  * in.m_m[5]) + (m_m[10] * in.m_m[9])  + (m_m[11] * in.m_m[13]);
        out.m_m[10] = (m_m[8]  * in.m_m[2]) + (m_m[9]  * in.m_m[6]) + (m_m[10] * in.m_m[10]) + (m_m[11] * in.m_m[14]);
        out.m_m[11] = (m_m[8]  * in.m_m[3]) + (m_m[9]  * in.m_m[7]) + (m_m[10] * in.m_m[11]) + (m_m[11] * in.m_m[15]);
        out.m_m[12] = (m_m[12] * in.m_m[0]) + (m_m[13] * in.m_m[4]) + (m_m[14] * in.m_m[8])  + (m_m[15] * in.m_m[12]);
        out.m_m[13] = (m_m[12] * in.m_m[1]) + (m_m[13] * in.m_m[5]) + (m_m[14] * in.m_m[9])  + (m_m[15] * in.m_m[13]);
        out.m_m[14] = (m_m[12] * in.m_m[2]) + (m_m[13] * in.m_m[6]) + (m_m[14] * in.m_m[10]) + (m_m[15] * in.m_m[14]);
        out.m_m[15] = (m_m[12] * in.m_m[3]) + (m_m[13] * in.m_m[7]) + (m_m[14] * in.m_m[11]) + (m_m[15] * in.m_m[15]);
    }
    Matrix4 Transpose() const {
        Matrix4 out;
        out.m_m[0]	= m_m[0];
        out.m_m[1]	= m_m[4];
        out.m_m[2]	= m_m[8];
        out.m_m[3]	= m_m[12];
        out.m_m[4]	= m_m[1];
        out.m_m[5]	= m_m[5];
        out.m_m[6]	= m_m[9];
        out.m_m[7]	= m_m[13];
        out.m_m[8]	= m_m[2];
        out.m_m[9]	= m_m[6];
        out.m_m[10] = m_m[10];
        out.m_m[11] = m_m[14];
        out.m_m[12]	= m_m[3];
        out.m_m[13]	= m_m[7];
        out.m_m[14] = m_m[11];
        out.m_m[15] = m_m[15];
    }
    Matrix4& operator=(const Matrix3& in) {
        m_m[0] = in.m_m[0];
        m_m[1] = in.m_m[1];
        m_m[2] = in.m_m[2];
        m_m[3] = 0.0f;
        m_m[4] = in.m_m[3];
        m_m[5] = in.m_m[4];
        m_m[6] = in.m_m[5];
        m_m[7] = 0.0f;
        m_m[8] = in.m_m[6];
        m_m[9] = in.m_m[7];
        m_m[10] = in.m_m[8];
        m_m[11] = 0.0f;
        m_m[12] = 0.0f;
        m_m[13] = 0.0f;
        m_m[14] = 0.0f;
        m_m[12] = 1.0f;
        return *this;
    }
    Matrix4& operator=(const Matrix4& in) {
        m_m[0] = in.m_m[0];
        m_m[1] = in.m_m[1];
        m_m[2] = in.m_m[2];
        m_m[3] = in.m_m[3];
        m_m[4] = in.m_m[4];
        m_m[5] = in.m_m[5];
        m_m[6] = in.m_m[6];
        m_m[7] = in.m_m[7];
        m_m[8] = in.m_m[8];
        m_m[9] = in.m_m[9];
        m_m[10] = in.m_m[10];
        m_m[11] = in.m_m[11];
        m_m[12] = in.m_m[12];
        m_m[13] = in.m_m[13];
        m_m[14] = in.m_m[14];
        m_m[15] = in.m_m[15];
        return *this;
    }
    Vector4	GetRow(const Rows row) const {
        const float* pFirst = m_m + (4 * row);
        return Vector4(pFirst[0], pFirst[1], pFirst[2], pFirst[3]);
    }
};

class Transform {
private:
    Matrix3		m_rotation;
    Vector3		m_translation;
    float		m_scale;
    Matrix4		m_matrix;
public:
    Transform() {
        m_rotation.Identify();
        m_translation = Vector3(0.0f, 0.0f, 0.0f);
        m_scale = 1.0f;
        m_matrix.Identify();
    }
    ~Transform() {}
    void Clone(const Transform& transform) {
        m_rotation = transform.m_rotation;
        m_translation = transform.m_translation;
        m_scale = transform.m_scale;
        UpdateMatrix();
    }
    void SetRotation(const Matrix3& rotation) {
        m_rotation = rotation;
    }
    const Matrix3& GetRotation() const {
        return m_rotation;
    }
    void SetTranslation(const Vector3& translation) {
        m_translation = translation;
    }
    const Vector3& GetTranslation() const {
        return m_translation;
    }
    void SetScale(const float scale) {
        m_scale = scale;
    }
    const float GetScale() const {
        return m_scale;
    }
    void ApplyForward(const Vector3& in, Vector3& out) const {
        out.Set(in);
        out.Multiply(m_scale);
        m_rotation.Transform(out);
        out.Add(m_translation);
    }

    void ApplyInverse(const Vector3& in, Vector3& out) const {
        out.Set(in);
        out.Subtract(m_translation);
        m_rotation.TransformTranspose(out);
        out.Divide(m_scale);
    }
    void UpdateMatrix() {
        m_matrix.m_m[0] = m_rotation.m_m[0] * m_scale;
        m_matrix.m_m[1] = m_rotation.m_m[1];
        m_matrix.m_m[2] = m_rotation.m_m[2];
        m_matrix.m_m[3] = 0.0f;
        m_matrix.m_m[4] = m_rotation.m_m[3];
        m_matrix.m_m[5] = m_rotation.m_m[4] * m_scale;
        m_matrix.m_m[6] = m_rotation.m_m[5];
        m_matrix.m_m[7] = 0.0f;
        m_matrix.m_m[8] = m_rotation.m_m[6];
        m_matrix.m_m[9] = m_rotation.m_m[7];
        m_matrix.m_m[10] = m_rotation.m_m[8] * m_scale;
        m_matrix.m_m[11] = 0.0f;
        m_matrix.m_m[12] = m_translation.m_x;
        m_matrix.m_m[13] = m_translation.m_y;
        m_matrix.m_m[14] = m_translation.m_z;
        m_matrix.m_m[15] = 1.0f;
    }
    const Matrix4& GetMatrix() const {
        return m_matrix;
    }
    void GetInverseMatrix(Matrix4& out) const {
        float invScale = 1.0f / m_scale;
        out.m_m[0] = m_rotation.m_m[0] * invScale;
        out.m_m[1] = m_rotation.m_m[3];
        out.m_m[2] = m_rotation.m_m[6];
        out.m_m[3] = 0.0f;
        out.m_m[4] = m_rotation.m_m[1];
        out.m_m[5] = m_rotation.m_m[4] * invScale;
        out.m_m[6] = m_rotation.m_m[7];
        out.m_m[7] = 0.0f;
        out.m_m[8] = m_rotation.m_m[2];
        out.m_m[9] = m_rotation.m_m[5];
        out.m_m[10] = m_rotation.m_m[8] * invScale;
        out.m_m[11] = 0.0f;
        out.m_m[12] = -m_translation.m_x;
        out.m_m[13] = -m_translation.m_y;
        out.m_m[14] = -m_translation.m_z;
        out.m_m[15] = 1.0f;
    }
    void GetInverseTransposeMatrix(Matrix4& out) const {
        float invScale = 1.0f / m_scale;
        out.m_m[0] = m_rotation.m_m[0] * invScale;
        out.m_m[1] = m_rotation.m_m[1];
        out.m_m[2] = m_rotation.m_m[2];
        out.m_m[3] = 0.0f;
        out.m_m[4] = m_rotation.m_m[3];
        out.m_m[5] = m_rotation.m_m[4] * invScale;
        out.m_m[6] = m_rotation.m_m[5];
        out.m_m[7] = 0.0f;
        out.m_m[8] = m_rotation.m_m[6];
        out.m_m[9] = m_rotation.m_m[7];
        out.m_m[10] = m_rotation.m_m[8] * invScale;
        out.m_m[11] = 0.0f;
        out.m_m[12] = -m_translation.m_x;
        out.m_m[13] = -m_translation.m_y;
        out.m_m[14] = -m_translation.m_z;
        out.m_m[15] = 1.0f;
    }
    void GetInverseTransposeMatrix(Matrix3& out) const {
        float invScale = 1.0f / m_scale;
        out.m_m[0] = m_rotation.m_m[0] * invScale;
        out.m_m[1] = m_rotation.m_m[1];
        out.m_m[2] = m_rotation.m_m[2];
        out.m_m[3] = m_rotation.m_m[3];
        out.m_m[4] = m_rotation.m_m[4] * invScale;
        out.m_m[5] = m_rotation.m_m[5];
        out.m_m[6] = m_rotation.m_m[6];
        out.m_m[7] = m_rotation.m_m[7];
        out.m_m[8] = m_rotation.m_m[8] * invScale;
    }
    Transform Multiply(const Transform& input) const {
        Transform result;
        result.m_rotation = m_rotation.Multiply(input.GetRotation());
        result.m_translation.Set(result.m_rotation.Transform(input.GetTranslation()));
        result.m_translation.Multiply(m_scale);
        result.m_translation.Add(m_translation);
        result.m_scale = m_scale * input.GetScale();
        return result;
    }
};

class Plane {
private:
    Vector3		m_normal;
    float		m_d;
public:
    Plane(): m_d(0.0f) {}
    Plane(const Vector3& point, const Vector3& normal) {
        BuildPlane(point, normal);
    }
    ~Plane() {}
    void BuildPlane(const Vector3& point, const Vector3& normal) {
        m_normal	= normal;
        m_normal.Normalize();
        m_d			= m_normal.Dot(point);
    }
    bool IsInFront(const Vector4& point) const {
        return IsInFront(Vector3(point.m_x, point.m_y, point.m_z));
    }
    bool IsInFront(const Vector3& point) const {
        return m_normal.Dot(point) >= m_d;
    }
};


#endif
