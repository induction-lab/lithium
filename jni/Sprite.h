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
        location(),
        angle(0.0f),
        scale(Vector2(1.0f, 1.0f)),
        color(Vector(1.0f, 1.0f, 1.0f)), opaque(1.0f),
        texturePath(texturePath), textureId(0),
        sheetWidth(0), sheetHeight(0),
        spriteWidth(width), spriteHeight(height),
        frameCount(0), frameXCount(0), frameYCount(0),
        animStartFrame(0), animFrameCount(1),
        animSpeed(0.0f), animFrame(0.0f), animLoop(false) {
        //
    };
    void setFrames(int startFrame, int frameCount, float speed, bool loop) {
        animStartFrame = startFrame;
        animFrame = 0.0f, animSpeed = speed, animLoop = loop;
        animFrameCount = frameCount;
    };
    int getValues(int tweenType, float *returnValues) {
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
        }
        return 0;
    };
    void setValues(int tweenType, float *newValues) {
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
        }
    };
    bool animationEnded() {
        return animFrame > (animFrameCount-1);
    };
    void setLocation(float x, float y) {
        location = Location(x, y);
    };
    Location getLocation() {
        return location;
    };
    int getWidth() {
        return spriteWidth;
    };
    int getHeight() {
        return spriteHeight;
    };
    bool pointInSprite(int x, int y) {
        Vector points[4];
        transform(points);
        std::swap(points[2], points[3]);
        bool inside = false;
        int size = sizeof(points)/sizeof(points[0]);
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
    void draw(Vertex vertices[4], float timeStep) {
        if (sheetWidth == 0 || sheetHeight == 0) return;
        int currentFrame, currentFrameX, currentFrameY;
        // Updates animation in loop mode.
        animFrame += timeStep * animSpeed;
        if (animLoop) {
            currentFrame = (animStartFrame + int(animFrame) % animFrameCount);
        } else {
            // Updates animation in one-shot mode.
            if (animationEnded()) {
                currentFrame = animStartFrame + (animFrameCount-1);
            } else {
                currentFrame = animStartFrame + int(animFrame);
            }
        };
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
    Location location;
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
    }
    const char* texturePath;
    GLuint textureId;
    // Frame.
    int spriteWidth, spriteHeight;
    int sheetWidth, sheetHeight;
    int frameXCount, frameYCount, frameCount;
    // Animation.
    int animStartFrame, animFrameCount;
    float animSpeed, animFrame;
    bool animLoop;
};

#endif // __SPRITE_H__