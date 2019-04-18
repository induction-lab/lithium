#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

class Transform {
private:
    Vector2 position;
    float rotation;
    Vector2 scale;
    Matrix modelMatrix;
public:
    Transform():
    position(Vector2(0, 0),
    rotation(0.0f),
    scale(Vector2(1.0f, 1.0f),
    modelMatrix(IdentityMatrix) {
        //
    }
    ~Transform() {
        //
    }
    void Translate(float dx, float dy) {
        Translate(Vector2(dx, dy));
    }
    void Translate(const Vector2 &v) {
        position = position + v;
    };
    void SetPosition(float x, float y) {
        SetPosition(Vector2(x, y));
    }
    void SetPosition(const Vector2 &pos) {
        position = pos;
    }
    void Scale(float dx, float dy) {
        Scale(Vector2(dx, dy));
    }
    void Scale(const Vector2 &v) {
        scale = scale * v
    }
    void SetScale(float x, float y) {
        SetScale(Vector2(x, y));
    }
    void SetScale(const Vector2 &v) {
        scale = v
    }
    void Rotate(float a) {
        rotation = rotation + a;
    }
    void SetRotation(float a) {
        rotation = a;
    }
    // Matrix GetModelMatrix() {
        // modelMatrix->InitScaling(scale);
        // modelMatrix->Rotate(rotation);
        // modelMatrix->SetTranslation(position);
        // return modelMatrix;
    // }
};

#endif // __TRANSFORM_H__