#ifndef __SCENE_H__
#define __SCENE_H__

#include "SpriteBatch.h"

class Scene {
public:
    virtual ~Scene() {};
    virtual status start() = 0;
    virtual void update() = 0;
    virtual void pause() {}
    virtual void resume() {}
};

class MainMenu : public Scene {
    status start() {
        LOG_INFO("MainScene create.");
        SpriteBatch *spriteBatch = new SpriteBatch();
        Sprite* sprite = spriteBatch->registerSprite("textures/Play.png", 104, 100);
        float halfWidth = (float) GraphicsManager::getInstance()->getRenderWidth() / 2;
        float halfHeight = (float) GraphicsManager::getInstance()->getRenderHeight() / 2;
        sprite->setLocation(halfWidth, halfHeight);
        Tween* t0 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_X, 1.0f, Ease::Sinusoidal::InOut)
            ->target(1.2f)->remove(false)->loop()->reverse()->start();
        Tween* t1 = TweenManager::getInstance()->addTween(sprite, TweenType::SCALE_Y, 1.0f, Ease::Sinusoidal::InOut)
            ->target(1.2f)->remove(false)->loop()->reverse()->start(0.5f);
        return STATUS_OK;
    }
    void update() {
        //
    }
};

#endif // __SCENE_H__
