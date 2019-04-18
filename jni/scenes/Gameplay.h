#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "Line.h"

class Gameplay: public Scene {
private:
    Activity* activity;
public:
    Gameplay(Activity* activity):
        activity(activity) {
        LOG_INFO("Scene Gameplay created.");
    };
    ~Gameplay() {
        LOG_INFO("Scene Gameplay Destructed.");
    };
    status start() {
        LOG_DEBUG("Start Gameplay scene.");
        spriteBatch = new SpriteBatch();
        float renderWidth = (float) GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float) GraphicsManager::getInstance()->getRenderHeight();
        float halfWidth = renderWidth / 2;
        float halfHeight = renderHeight / 2;
        background = addBackground("textures/Background_temp.png", 360, 640, Location(halfWidth, halfHeight));
        Background* gameBox = addBackground("textures/GameBox.png", 360, 380, Location(halfWidth, halfHeight));
        gameBox->sprite->opaque = 0.0f;
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::OPAQUE, 0.7f, Ease::Sinusoidal::InOut)
                    ->target(1.0f)->remove(true)->start();
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start();
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start(0.5f);
        // Apple.
        apple = addFruit("textures/apple.png", 64, 64, Location(halfWidth - 64.0, halfHeight + 64.0f));
        apple->setClickFunction(std::bind(&Gameplay::onAppleClick, this));
        TweenManager::getInstance()->addTween(apple->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(apple->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());        
        // Tomato.
        tomato = addFruit("textures/tomato.png", 64, 64, Location(halfWidth, halfHeight + 64.0f));
        tomato->setClickFunction(std::bind(&Gameplay::onTomatoClick, this));
        TweenManager::getInstance()->addTween(tomato->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(tomato->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        // Pear.
        pear = addFruit("textures/pear.png", 64, 64, Location(halfWidth + 64.0f, halfHeight + 64.0f));
        pear->setClickFunction(std::bind(&Gameplay::onPearClick, this));
        TweenManager::getInstance()->addTween(pear->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(pear->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
                    
        // Orange.
        orange = addFruit("textures/orange.png", 64, 64, Location(halfWidth - 64.0f, halfHeight));
        orange->setClickFunction(std::bind(&Gameplay::onOrangeClick, this));
        TweenManager::getInstance()->addTween(orange->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(orange->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());

        line = new Line(5.0f);
        line->color = Vector(0.2f, 1.0f, 0.4f);
        
        grub01 = SoundManager::getInstance()->registerSound("sounds/Grub01.wav");
        grub02 = SoundManager::getInstance()->registerSound("sounds/Grub02.wav");
        grub03 = SoundManager::getInstance()->registerSound("sounds/Grub03.wav");
        grub04 = SoundManager::getInstance()->registerSound("sounds/Grub04.wav");
        grub05 = SoundManager::getInstance()->registerSound("sounds/Grub05.wav");
        SoundManager::getInstance()->loadResources();
        
        created = true;
        return STATUS_OK;
    };
    /*
    void gestureTapEvent(int x, int y) {
        if (!created) return;
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        line->addPoint(Vector(point.x, point.y, 0.0f));
    };
    */
    void update() {
        Scene::update();
    };
    void onAppleClick() {
        apple->alive = false;
        SoundManager::getInstance()->playSound(grub01);
        LOG_DEBUG("Kill aplle.");
    };    
    void onTomatoClick() {
        tomato->alive = false;
        SoundManager::getInstance()->playSound(grub02);
        LOG_DEBUG("Kill tomato.");
    };
    void onPearClick() {
        pear->alive = false;
        SoundManager::getInstance()->playSound(grub03);
        LOG_DEBUG("Kill pear.");
    };
    void onOrangeClick() {
        orange->alive = false;
        SoundManager::getInstance()->playSound(grub04);
        LOG_DEBUG("Kill pear.");
    };    
    void backEvent() {
        activity->setStartScene();
    };
    void gestureSwipeEvent(int x, int y, int direction) {
        if (uiModeType != ACONFIGURATION_UI_MODE_TYPE_WATCH) return;
        if (direction == SWIPE_DIRECTION_RIGHT) activity->setStartScene();
    };
    Background* background;
    // Fruits.
    Fruit* apple;
    Fruit* tomato;
    Fruit* pear;
    Fruit* orange;
    Line* line;
    Sound* grub01;
    Sound* grub02;
    Sound* grub03;
    Sound* grub04;
    Sound* grub05;
};

#endif
