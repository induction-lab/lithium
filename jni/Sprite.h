#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "Tween.h"

class SpriteBatch;

class Sprite : public Tweenable {
public:
    struct Vertex {
        GLfloat x, y, u, v;
    };
    Sprite(const char* texturePath, int32_t width, int32_t height):
        location(),
        angle(0.0f),
        scale(Vector2(1.0f, 1.0f)),
        texturePath(texturePath), textureId(0),
        sheetWidth(0), sheetHeight(0),
        spriteWidth(width), spriteHeight(height),
        frameCount(0), frameXCount(0), frameYCount(0),
        animStartFrame(0), animFrameCount(1),
        animSpeed(0.0f), animFrame(0.0f), animLoop(false) {
        //
    }
    void setFrames(int32_t startFrame, int32_t frameCount, float speed, bool loop) {
        animStartFrame = startFrame;
        animFrame = 0.0f, animSpeed = speed, animLoop = loop;
        animFrameCount = frameCount;
    }
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
            return 1;
        }
        return 0;
    }
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
        }
    }
    bool animationEnded() {
        return animFrame > (animFrameCount-1);
    }
    void setLocation(float x, float y) {
        location = Location(x, y);
    }
    Location getLocation() {
        return location;
    }
    void setAngle(float a) {
        angle = a;
    }
    float getAngle() {
        return angle;
    }
    void setScale(float x, float y) {
        scale = Vector2(x, y);
    }
    Vector2 getScale() {
        return scale;
    }
    int32_t getSpriteHeight() {
        return spriteHeight;
    }
    int32_t getSpriteWidth() {
        return spriteWidth;
    }
    bool pointInSprite(Location point) {
        float halfWidth = spriteWidth * 0.5f;
        float halfHeight = spriteHeight * 0.5f;
        return ((point.x > location.x - halfWidth ) &&
                (point.x < location.x + halfWidth ) &&
                (point.y > location.y - halfHeight) &&
                (point.y < location.y + halfHeight));
    }
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
    }
    void draw(Vertex vertices[4], float timeStep) {
        if (sheetWidth == 0 || sheetHeight == 0) return;
        int32_t currentFrame, currentFrameX, currentFrameY;
        // Updates animation in loop mode.
        animFrame += timeStep * animSpeed;
        if (animLoop) {
            currentFrame = (animStartFrame + int32_t(animFrame) % animFrameCount);
        } else {
            // Updates animation in one-shot mode.
            if (animationEnded()) {
                currentFrame = animStartFrame + (animFrameCount-1);
            } else {
                currentFrame = animStartFrame + int32_t(animFrame);
            }
        }
        // Computes frame X and Y indexes from its id.
        currentFrameX = currentFrame % frameXCount;
        // currentFrameY is converted from OpenGL coordinates to top-left coordinates.
        currentFrameY = frameYCount - 1 - (currentFrame / frameXCount);
        // Draws selected frame.
        GLfloat u1 = GLfloat(currentFrameX * spriteWidth) / GLfloat(sheetWidth);
        GLfloat u2 = GLfloat((currentFrameX + 1) * spriteWidth) / GLfloat(sheetWidth);
        GLfloat v1 = GLfloat(currentFrameY * spriteHeight) / GLfloat(sheetHeight);
        GLfloat v2 = GLfloat((currentFrameY + 1) * spriteHeight) / GLfloat(sheetHeight);
        // Apply transformations.
        Vector translate = Vector(location.x, location.y, 0.0f);
        Vector n1 = Vector(-float(spriteWidth / 2), -float(spriteHeight / 2), 0.0f);
        n1 = ScaleMatrix(Vector(scale.x, scale.y, 1.0f)) * n1;
        n1 = RotateMatrix(angle, AxisZ) * n1;
        n1 = TranslateMatrix(translate) * n1;
        Vector n2 = Vector(-float(spriteWidth / 2),  float(spriteHeight / 2), 0.0f);
        n2 = ScaleMatrix(Vector(scale.x, scale.y, 1.0f)) * n2;
        n2 = RotateMatrix(angle, AxisZ) * n2;
        n2 = TranslateMatrix(translate) * n2;
        Vector n3 = Vector( float(spriteWidth / 2), -float(spriteHeight / 2), 0.0f);
        n3 = ScaleMatrix(Vector(scale.x, scale.y, 1.0f)) * n3;
        n3 = RotateMatrix(angle, AxisZ) * n3;
        n3 = TranslateMatrix(translate) * n3;
        Vector n4 = Vector( float(spriteWidth / 2),  float(spriteHeight / 2), 0.0f);
        n4 = ScaleMatrix(Vector(scale.x, scale.y, 1.0f)) * n4;
        n4 = RotateMatrix(angle, AxisZ) * n4;
        n4 = TranslateMatrix(translate) * n4;
        // Fill sprite vertices.
        vertices[0].x = n1.x;
        vertices[0].y = n1.y;
        vertices[0].u = u1;
        vertices[0].v = v1;
        vertices[1].x = n2.x;
        vertices[1].y = n2.y;
        vertices[1].u = u1;
        vertices[1].v = v2;
        vertices[2].x = n3.x;
        vertices[2].y = n3.y;
        vertices[2].u = u2;
        vertices[2].v = v1;
        vertices[3].x = n4.x;
        vertices[3].y = n4.y;
        vertices[3].u = u2;
        vertices[3].v = v2;
    }
private:
    const char* texturePath;
    GLuint textureId;
    Location location;
    Vector2 scale;
    float angle;
    // Frame.
    int32_t sheetHeight, sheetWidth;
    int32_t spriteHeight, spriteWidth;
    int32_t frameXCount, frameYCount, frameCount;
    // Animation.
    int32_t animStartFrame, animFrameCount;
    float animSpeed, animFrame;
    bool animLoop;
};

#endif