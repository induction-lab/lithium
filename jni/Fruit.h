#ifndef __FRUIT_H__
#define __FRUIT_H__

#include <functional>

#include "InputManager.h"
#include "GraphicsManager.h"
#include "SpriteBatch.h"

// Fruit.
class Fruit: public InputListener {
public:
    Fruit(int type):
        type(type),                                            // fruit type
        alive(true), dead(false),                              // dead state
        animated(false), selected(false),                      // selected state
        xScaleTween(NULL), yScaleTween(NULL),                  // select animation tweens
        index(Vector2()),                                      // index on board
        clickFunction(NULL),
        deadFunction(NULL) {
        // LOG_DEBUG("Create Fruit.");
    };
    ~Fruit() {
        // LOG_DEBUG("Delete Fruit.");
    };
    int gestureTapEvent(int x, int y) {
        if (!alive) return false;
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
            Tween* t2 = TweenManager::getInstance()->addTween(sprite, TweenType::OPAQUE, 0.5f, Ease::Sinusoidal::InOut)
                    ->target(0.0f)->remove(true);
            t2->onComplete(std::bind(&Fruit::onDead, this));
            t1->addChain(t2)->start(0.3f);
        } 
    };
    void onDead() {
        dead = true;
        if (deadFunction != NULL) deadFunction(index.x, index.y);
    };
    void moveTo(Vector2 location, float delay = 0.0f) {
        TweenManager::getInstance()->addTween(sprite, TweenType::POSITION_XY, 0.35f, Ease::Back::Out)
            ->target(location.x, location.y)->remove(true)->start(delay);
    };
    int type;
    bool alive, dead;
    bool animated, selected;
    Tween* xScaleTween;
    Tween* yScaleTween;
    Vector2 index;
    Sprite* sprite;
    void setClickFunction(std::function<void(int, int)> callback) { clickFunction = callback; };
    void setDeadFunction(std::function<void(int, int)> callback) { deadFunction = callback; };
private:
    std::function<void(int, int)> clickFunction;
    std::function<void(int, int)> deadFunction;
};

#endif