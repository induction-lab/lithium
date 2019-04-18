#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "Tween.h"

class SpriteBatch;

class Sprite: public Tweenable {
public:
    struct Vertex {
        GLfloat x, y, u, v;
    };
    Sprite(const char* texturePath, int width, int height):
        order(0),
        location(),
        angle(0.0f),
        scale(Vector2(1.0f, 1.0f)),
        color(Vector(1.0f, 1.0f, 1.0f)), opaque(1.0f),
        texturePath(texturePath), textureId(0),
        sheetWidth(0), sheetHeight(0),
        spriteWidth(width), spriteHeight(height),
        frameCount(0), frameXCount(0), frameYCount(0),
        currentFrame(0) {
        LOG_DEBUG("Create sprite.");
    };
    ~Sprite() {
        LOG_DEBUG("Delete sprite.");
    };
    void setFrame(int frame) {
        currentFrame = frame;
    };
    int getFrame() {
        return currentFrame;
    };
    int getValues(int tweenType, float* returnValues) {
        switch (tweenType) {
            case TweenType::POSITION_X:
                returnValues[0] = location.x;
                return 1;
            case TweenType::POSITION_Y:
                returnValues[0] = location.y;
                return 1;
            case TweenType::POSITION_XY:
                returnValues[0] = location.x;
                returnValues[1] = location.y;
                return 2;
            case TweenType::ROTATION_CW:
            case TweenType::ROTATION_CCW:
                angle = returnValues[0] = angle;
                return 1;
            case TweenType::SCALE_X:
                returnValues[0] = scale.x;
                return 1;
            case TweenType::SCALE_Y:
                returnValues[0] = scale.y;
                return 1;
            case TweenType::SCALE_XY:
                returnValues[0] = scale.x;
                returnValues[1] = scale.y;
                return 2;
            case TweenType::OPAQUE:
                returnValues[0] = opaque;
                return 1;
            case TweenType::COLOR:
                returnValues[0] = color.x; // r
                returnValues[1] = color.y; // g
                returnValues[2] = color.z; // b
                return 3;
            case TweenType::FRAME:
                returnValues[0] = currentFrame;
                return 1;
        }
        return 0;
    };
    void setValues(int tweenType, float* newValues) {
        switch (tweenType) {
            case TweenType::POSITION_X:
                location.x = newValues[0];
                break;
            case TweenType::POSITION_Y:
                location.y = newValues[0];
                break;
            case TweenType::POSITION_XY:
                location.x = newValues[0];
                location.y = newValues[1];
                break;
            case TweenType::ROTATION_CW:
            case TweenType::ROTATION_CCW:
                angle = newValues[0];
                break;
            case TweenType::SCALE_X:
                scale.x = newValues[0];
                break;
            case TweenType::SCALE_Y:
                scale.y = newValues[0];
                break;
            case TweenType::SCALE_XY:
                scale = Vector2(newValues[0], newValues[1]);
                break;
            case TweenType::OPAQUE:
                opaque = newValues[0];
                break;
            case TweenType::COLOR:
                color = Vector(newValues[0], newValues[1], newValues[2]);
                break;
            case TweenType::FRAME:
                currentFrame = (int)round(newValues[0]);
                break;
        }
    };
    int getWidth() {
        return spriteWidth;
    };
    int getHeight() {
        return spriteHeight;
    };
    bool pointInSprite(int x, int y) {
        Vector points[4];  // sprite polygon with 4 points
        transform(points); // tansform it ...
        // This method counts the number of times a ray starting from a point (x, y) crosses
        // a polygon boundary edge separating it's inside and outside.
        std::swap(points[2], points[3]);
        // Before   After
        // 3---2    2---3
        // | / |    |·/·|
        // 1---4    1---4
        bool inside = false;
        int size = sizeof(points)/sizeof(points[0]); // just for support more points poly
        for (int i = 0, j = size - 1; i < size; j = i++) {
            if (((points[i].y > y) != (points[j].y >= y)) && (x < (points[j].x - points[i].x) * (y - points[i].y) / (points[j].y - points[i].y) + points[i].x)) inside = !inside;
        }
        return inside;
    };
protected:
    friend class SpriteBatch;
    status load() {
        Texture* texture = GraphicsManager::getInstance()->loadTexture(texturePath, GL_LINEAR, GL_CLAMP_TO_EDGE);
        textureId = texture->getTextureId();
        sheetWidth = texture->getWidth();
        sheetHeight = texture->getHeight();
        frameXCount = sheetWidth / spriteWidth;
        frameYCount = sheetHeight / spriteHeight;
        frameCount = (sheetHeight / spriteHeight) * (sheetWidth / spriteWidth);
        return STATUS_OK;
    };
    void draw(Vertex vertices[4]) {
        if (sheetWidth == 0 || sheetHeight == 0) return;
        int currentFrameX, currentFrameY;
        // Computes frame X and Y indexes from its id.
        currentFrameX = currentFrame % frameXCount;
        // currentFrameY is converted from OpenGL coordinates to top-left coordinates.
        currentFrameY = frameYCount - 1 - (currentFrame / frameXCount);
        // Draws selected frame.
        GLfloat u1 = GLfloat(currentFrameX * spriteWidth) / GLfloat(sheetWidth);
        GLfloat u2 = GLfloat((currentFrameX + 1) * spriteWidth) / GLfloat(sheetWidth);
        GLfloat v1 = GLfloat(currentFrameY * spriteHeight) / GLfloat(sheetHeight);
        GLfloat v2 = GLfloat((currentFrameY + 1) * spriteHeight) / GLfloat(sheetHeight);
        Vector points[4];
        transform(points);
        // Fill sprite vertices.
        vertices[0].x = points[0].x; vertices[0].y = points[0].y; vertices[0].u = u1; vertices[0].v = v1;
        vertices[1].x = points[1].x; vertices[1].y = points[1].y; vertices[1].u = u1; vertices[1].v = v2;
        vertices[2].x = points[2].x; vertices[2].y = points[2].y; vertices[2].u = u2; vertices[2].v = v1;
        vertices[3].x = points[3].x; vertices[3].y = points[3].y; vertices[3].u = u2; vertices[3].v = v2;
    };
public:
    // Tratsormations.
    int order;    
    Vector2 location;
    Vector2 scale;
    float angle;
    Vector color;
    float opaque;
private:
    void transform(Vector points[4]) {
        // Apply transformations.
        Matrix matrix = IdentityMatrix;
        matrix.Translate(location.x, location.y, 0.0f);
        matrix.Rotate(angle, AxisZ);
        matrix.Scale(scale.x, scale.y, 1.0f);
        float halfWidth = (float)spriteWidth * 0.5f;
        float halfHeight = (float)spriteHeight * 0.5f;
        points[0] = matrix * Vector(-halfWidth, -halfHeight, 0.0f);
        points[1] = matrix * Vector(-halfWidth,  halfHeight, 0.0f);
        points[2] = matrix * Vector( halfWidth, -halfHeight, 0.0f);
        points[3] = matrix * Vector( halfWidth,  halfHeight, 0.0f);
    };
    const char* texturePath;
    GLuint textureId;
    // Frame.
    int spriteWidth, spriteHeight;
    int sheetWidth, sheetHeight;
    int frameXCount, frameYCount, frameCount;
    int currentFrame;
};

#endif // __SPRITE_H__