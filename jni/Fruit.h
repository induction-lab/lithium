#ifndef __FRUIT_H__
#define __FRUIT_H__

#include <functional>

#include "InputManager.h"
#include "GraphicsManager.h"
#include "SpriteBatch.h"

// Some black magic.
Vector2 getSkrewedLocation(int X, int Y) {
    float renderWidth = (float)GraphicsManager::getInstance()->getRenderWidth();
    float renderHeight = (float)GraphicsManager::getInstance()->getRenderHeight();
    float halfWidth = renderWidth / 2;
    float halfHeight = renderHeight / 2;
    float dy = (X * 2.0f) - (0.2f * GRID_SIZE * Y);
    float dx = (Y * 2.0f);
    return Vector2(halfWidth + X * 42 - 100 - dx, halfHeight - Y * 42 + 110 - dy);
};

// Fruit.
class Fruit: public InputListener {
public:
    Fruit(int type):
        type(type),                                            // fruit type
        alive(true), dead(false),                              // alive/dead state
        animated(false), selected(false),                      // selected state
        xScaleTween(NULL), yScaleTween(NULL),                  // select animation tweens
        moveTween(NULL),                                       // move animation tween
        index(Vector2()),                                      // index on board
        prevIndex(Vector2()),                                  // prevision index on board
        // Callback functions.
        clickFunction(NULL),
        killFunction(NULL),
        deadFunction(NULL),
        movedFunction(NULL) {
        // LOG_DEBUG("Create Fruit.");
    };
    ~Fruit() {
        // LOG_DEBUG("Delete Fruit.");
    };
    int gestureTapEvent(int x, int y) {
        if (!alive) return 0;
        Vector2 point = GraphicsManager::getInstance()->screenToRender(x, y);
        if (sprite->pointInSprite(point.x, point.y)) {
            if (!animated) {
                xScaleTween = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_X, 0.25f, Ease::Sinusoidal::InOut)
                              ->target(1.1f)->remove(false)->loop()->reverse()->start();
                yScaleTween = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_Y, 0.25f, Ease::Sinusoidal::InOut)
                              ->target(1.1f)->remove(false)->loop()->reverse()->start(0.2f);
                animated = true;
            }
            if (clickFunction != NULL) clickFunction((int)index.x, (int)index.y);
            return 1;
        }
        return 0;
    };
    int gestureLongTapEvent(int x, int y, float time) {
        return gestureTapEvent(x, y);
    };
    void update() {
        if (animated && !selected) {
            if (xScaleTween != NULL) TweenManager::getInstance()->remove(xScaleTween);
            if (yScaleTween != NULL) TweenManager::getInstance()->remove(yScaleTween);
            xScaleTween = NULL;
            yScaleTween = NULL;
            sprite->scale = Vector2(0.9f, 0.9f);
            animated = false;
        }
    };
    void kill() {
        if (!dead) {
            alive = false;
            Tween* t1 = TweenManager::getInstance()->addTween(sprite, TweenType::FRAME, 0.5f, Ease::Sinusoidal::InOut)
                ->target(4.0f)->remove(true);
            Tween* t2 = TweenManager::getInstance()->addTween(sprite, TweenType::OPAQUE, 0.15f, Ease::Sinusoidal::InOut)
                ->target(0.0f)->remove(true);
            t2->onComplete(std::bind(&Fruit::onDead, this));
            t1->addChain(t2)->onStart(std::bind(&Fruit::onKill, this))->start(0.3f);
        }
    };
    void onKill() {
        if (killFunction != NULL) killFunction(index.x, index.y);
    }
    void onDead() {
        dead = true;
        if (deadFunction != NULL) deadFunction(index.x, index.y);
    };
    void onMoved() {
        if (movedFunction != NULL) movedFunction(index.x, index.y);
    };
    void moveTo(int x, int y, float delay = 0.0f) {
        index = Vector2(x, y);
        Vector2 location = getSkrewedLocation(x, y);
        moveTween = TweenManager::getInstance()->addTween(sprite, TweenType::POSITION_XY, 0.35f, Ease::Back::Out)
                    ->target(location.x, location.y)->remove(true)->start(delay)
                    ->onComplete(std::bind(&Fruit::onMoved, this));
        selected = false;
    };
    void moveBack(float delay = 0.0f) {
        Vector2 location = getSkrewedLocation((int)prevIndex.x, (int)prevIndex.y);
        moveTween = TweenManager::getInstance()->addTween(sprite, TweenType::POSITION_XY, 0.35f, Ease::Back::Out)
            ->target(location.x, location.y)->remove(true)->start(delay);
    };
    int type;
    bool alive, dead;
    bool animated, selected;
    Tween* xScaleTween;
    Tween* yScaleTween;
    Tween* moveTween;
    Vector2 index, prevIndex;
    Sprite* sprite;
    void setClickFunction(std::function<void(int, int)> callback) {
        clickFunction = callback;
    };    
    void setKillFunction(std::function<void(int, int)> callback) {
        killFunction = callback;
    };
    void setDeadFunction(std::function<void(int, int)> callback) {
        deadFunction = callback;
    };
    void setMovedFunction(std::function<void(int, int)> callback) {
        movedFunction = callback;
    };
private:
    std::function<void(int, int)> clickFunction;
    std::function<void(int, int)> killFunction;
    std::function<void(int, int)> deadFunction;
    std::function<void(int, int)> movedFunction;
};

#endif