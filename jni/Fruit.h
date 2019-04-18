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

enum class FruitMoveType {
    UNKNOWN, SWAP, SWAP_BACK, DROP, DROP_EXTRA
};

enum class FruitKillType {
    UNKNOWN, DEAD, REPLACE
};

// Fruit.
class Fruit: public InputListener {
public:
    Fruit(int type):
        type(type),                                            // fruit type
        alive(true), dead(false),                              // alive/dead state
        selected(false), animated(false),                      // selected state
        dropped(false),                                        // dropped state
        xScaleTween(NULL), yScaleTween(NULL),                  // select animation tweens
        moveTween(NULL),                                       // move animation tween
        index(Vector2()),                                      // index on board
        prevIndex(Vector2()),                                  // prevision index on board
        // Callback functions.
        clickFunction(NULL),
        moveFunction(NULL),
        movedFunction(NULL),        
        killFunction(NULL),
        dyingFunction(NULL),
        deadFunction(NULL),
        moveType(FruitMoveType::UNKNOWN),
        killType(FruitKillType::UNKNOWN) {
        // LOG_DEBUG("Create Fruit.");
    };
    ~Fruit() {
        // LOG_DEBUG("Delete Fruit.");
    };
    int gestureTapEvent(int x, int y) {
        if (!alive) return 0;
        Vector2 point = GraphicsManager::getInstance()->screenToRender(x, y);
        if (sprite->pointInSprite(point.x, point.y)) {
            if (!animated && !selected) {
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
            sprite->scale = Vector2(0.9f, 0.9f);
            animated = false;
        }
    };
    void kill(float delay = 0.0f, FruitKillType fruitkillType = FruitKillType::DEAD) {
        killType = fruitkillType;
        if (alive) {
            alive = false;
            if (killFunction != NULL) killFunction(index.x, index.y, killType);
        }
        if (type >= FRUITS_COUNT) {
            onDead();
            return;
        }
        if (!dead) {
            Tween* t1 = TweenManager::getInstance()->addTween(sprite, TweenType::FRAME, 0.5f, Ease::Linear)
                ->onStart(std::bind(&Fruit::onDying, this))
                ->target(4.0f)->remove(true);
            Tween* t2 = TweenManager::getInstance()->addTween(sprite, TweenType::OPAQUE, 0.15f, Ease::Sinusoidal::InOut)
                ->target(0.0f)->remove(true)
                ->onComplete(std::bind(&Fruit::onDead, this));
            t1->addChain(t2)->start(delay);
        }
    };
    void onMoved() {
        if (movedFunction != NULL) movedFunction(index.x, index.y, moveType);
    };
    void onDying() {
        if (dyingFunction != NULL) dyingFunction(index.x, index.y, killType);
    };
    void onDead() {
        dead = true;
        if (deadFunction != NULL) deadFunction(index.x, index.y, killType);
    };
    void moveTo(int x, int y, float delay = 0.0f, FruitMoveType fruitMoveType = FruitMoveType::UNKNOWN) {
        moveType = fruitMoveType;
        if (moveFunction != NULL) moveFunction(index.x, index.y, moveType);
        prevIndex = index;
        index = Vector2(x, y);
        Vector2 location = getSkrewedLocation(x, y);
        moveTween = TweenManager::getInstance()->addTween(sprite, TweenType::POSITION_XY, 0.35f, Ease::Back::Out)
            ->target(location.x, location.y)->remove(true)
            ->onComplete(std::bind(&Fruit::onMoved, this))
            ->start(delay);
        selected = false;
    };
    void setClickFunction(std::function<void(int, int)> callback) {
        clickFunction = callback;
    };
    void setMoveFunction(std::function<void(int, int, FruitMoveType)> callback) {
        moveFunction = callback;
    };    
    void setMovedFunction(std::function<void(int, int, FruitMoveType)> callback) {
        movedFunction = callback;
    };
    void setKillFunction(std::function<void(int, int, FruitKillType)> callback) {
        killFunction = callback;
    };
    void setDyingFunction(std::function<void(int, int, FruitKillType)> callback) {
        dyingFunction = callback;
    };
    void setDeadFunction(std::function<void(int, int, FruitKillType)> callback) {
        deadFunction = callback;
    };
    int type;
    bool alive, dead;
    bool animated, selected, dropped;
    FruitMoveType moveType;
    FruitKillType killType;
    Tween* xScaleTween;
    Tween* yScaleTween;
    Tween* moveTween;
    Vector2 index, prevIndex;
    Sprite* sprite;    
private:
    std::function<void(int, int)> clickFunction;
    std::function<void(int, int, FruitMoveType)> moveFunction;
    std::function<void(int, int, FruitMoveType)> movedFunction;
    std::function<void(int, int, FruitKillType)> killFunction;
    std::function<void(int, int, FruitKillType)> dyingFunction;
    std::function<void(int, int, FruitKillType)> deadFunction;
};

#endif