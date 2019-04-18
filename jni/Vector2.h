#ifndef __VECTOR2_H__
#define __VECTOR2_H__

#include <math.h>

class Vector2 {
public:
    // member variables
    float x, y;

    // constructor
    Vector2(float m = 0, float n = 0):x(m), y(n) {}
    Vector2():x(0), y(0) {}
    Vector2(const Vector2& v) { // copy constructor
        x = v.x;
        y = v.y;
    }

    // getters and setters
    void set(float m, float n) {
        x = m;
        y = n;
    }
    void setX(float m) {
        x = m;
    }
    void setY(float n) {
        y = n;
    }
    void setLength(float length) {
        float angle = getAngle() * M_PI / 180;
        x = cosf(angle) * length;
        y = sinf(angle) * length;
    }
    void setAngle(float angle) {
        float length = getLength();
        angle *= M_PI / 180;
        x = cosf(angle) * length;
        y = sinf(angle) * length;
    }
    float getX() {
        return x;
    }
    float getY() {
        return y;
    }
    float getLengthSQR() {
        return *this * *this;
    };
    float getLength() {
        return sqrtf(getLengthSQR());
    }
    float getAngle() {
        return atan2(y, x) * 180 / M_PI;
    }

    // methods
    Vector2& operator =(const Vector2& v) {
        x = v.x;
        y = v.y;
        return *this;
    }
    Vector2 operator +(const Vector2& v) {
        return Vector2(x + v.x, y + v.y);
    }
    Vector2& operator +=(const Vector2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    Vector2 operator -(const Vector2& v) {
        return Vector2(x - v.x, y - v.y);
    }
    Vector2& operator -=(const Vector2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    Vector2 operator *(float c) {
        return Vector2(x * c, y * c);
    }
    Vector2& operator *=(float c) {
        x *= c;
        y *= c;
        return *this;
    };
    Vector2 operator /(float c) {
        //if (c == 0) throw (1 / c);
        return Vector2(x / c, y / c);
    }
    Vector2& operator /=(float c) {
        x /= c;
        y /= c;
        return *this;
    }
    bool operator ==(const Vector2& v) {
        return (x == v.x && y == v.y);
    }
    bool operator !=(const Vector2& v) {
        return (x != v.x && y != v.y);
    }
    float operator *(const Vector2& v) {
        return (x * v.x + y * v.y);
    }
    void reverse() {
        *this *= -1;
    }
    Vector2 reflect(const Vector2& normal) {
        Vector2 tmp(normal);
        tmp.normalize();
        tmp = *this - tmp * 2 * (tmp * *this);
        tmp.normalize();
        return tmp;
    }
    void truncate(float limit) {
        float length = getLength();
        length = fminf(length, limit);
        setLength(length);
    }
    void normalize() {
        if (getLength() == 0) {
            x = 1;
            y = 0;
        } else {
            *this /= getLength();
        }
    }
    bool isNormalized() {
        return getLength() == 1;
    }
    void rotate(float dA) {
        float angle = getAngle();
        setAngle(angle + dA);
    }
    float angleBetween(const Vector2 v) {
        Vector2 tmp1 = Vector2(*this);
        tmp1.normalize();
        Vector2 tmp2 = Vector2(v);
        tmp2.normalize();
        return acosf(tmp1 * tmp2) * 180 / M_PI;
    }
    float distanceBetween(const Vector2 v) {
        Vector2 tmp = Vector2(*this);
        tmp -= v;
        return tmp.getLength();
    }
};

#endif // __VECTOR2_H__
