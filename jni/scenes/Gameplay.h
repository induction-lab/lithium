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
        if (created) return STATUS_OK;
        LOG_DEBUG("Start Gameplay scene.");
        spriteBatch = new SpriteBatch();
        float renderWidth = (float) GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float) GraphicsManager::getInstance()->getRenderHeight();
        float halfWidth = renderWidth / 2;
        float halfHeight = renderHeight / 2;
        background = addBackground("textures/Background.png", 360, 640, Location(halfWidth, halfHeight));
        gameBox = addBackground("textures/GameBox.png", 360, 380, Location(halfWidth, halfHeight));
        gameBox->sprite->opaque = 0.0f;
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::OPAQUE, 0.7f, Ease::Sinusoidal::InOut)
                    ->target(1.0f)->remove(true)->start();
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start();
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start(0.5f);
        // Apple.
        apple = addFruit("textures/AppleFruit.png", 64, 64, Location(halfWidth - 64.0, halfHeight + 64.0f));
        apple->setClickFunction(std::bind(&Gameplay::onAppleClick, this));
        TweenManager::getInstance()->addTween(apple->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(apple->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());        
        // Tomato.
        tomato = addFruit("textures/TomatoFruit.png", 64, 64, Location(halfWidth, halfHeight + 64.0f));
        tomato->setClickFunction(std::bind(&Gameplay::onTomatoClick, this));
        TweenManager::getInstance()->addTween(tomato->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(tomato->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        // Pear.
        pear = addFruit("textures/PearFruit.png", 64, 64, Location(halfWidth + 64.0f, halfHeight + 64.0f));
        pear->setClickFunction(std::bind(&Gameplay::onPearClick, this));
        TweenManager::getInstance()->addTween(pear->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(pear->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        // Orange.
        orange = addFruit("textures/OrangeFruit.png", 64, 64, Location(halfWidth - 64.0f, halfHeight));
        orange->setClickFunction(std::bind(&Gameplay::onOrangeClick, this));
        TweenManager::getInstance()->addTween(orange->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(orange->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        // Grapes.
        grapes = addFruit("textures/GrapesFruit.png", 64, 64, Location(halfWidth, halfHeight));
        grapes->setClickFunction(std::bind(&Gameplay::onGrapesClick, this));
        TweenManager::getInstance()->addTween(grapes->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(grapes->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        // Bannana.
        bannana = addFruit("textures/BannanaFruit.png", 64, 64, Location(halfWidth + 64.0f, halfHeight));
        bannana->setClickFunction(std::bind(&Gameplay::onBannanaClick, this));
        TweenManager::getInstance()->addTween(bannana->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(bannana->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        // Carrot.
        carrot = addFruit("textures/CarrotFruit.png", 64, 64, Location(halfWidth, halfHeight - 64));
        carrot->setClickFunction(std::bind(&Gameplay::onCarrotClick, this));
        TweenManager::getInstance()->addTween(carrot->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        TweenManager::getInstance()->addTween(carrot->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.1f)->remove(false)->loop()->reverse()->start(frand());
        // Line (just for test).
        line = new Line(5.0f);
        line->color = Vector(0.2f, 1.0f, 0.4f);
        // Load sounds.
        grub01Sound = SoundManager::getInstance()->registerSound("sounds/Grub01.wav");
        grub02Sound = SoundManager::getInstance()->registerSound("sounds/Grub02.wav");
        grub03Sound = SoundManager::getInstance()->registerSound("sounds/Grub03.wav");
        grub04Sound = SoundManager::getInstance()->registerSound("sounds/Grub04.wav");
        grub05Sound = SoundManager::getInstance()->registerSound("sounds/Grub05.wav");
        SoundManager::getInstance()->loadResources();
        created = true;
        return STATUS_OK;
    };
    int gestureTapEvent(int x, int y) {
        if (!created) return false;
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        // line->addPoint(Vector(point.x, point.y, 0.0f));
        return 0;
    };
    void update() {
        Scene::update();
    };
    void onAppleClick() {
        apple->alive = false;
        SoundManager::getInstance()->playSound(grub01Sound);
        LOG_DEBUG("Kill aplle.");
    };    
    void onTomatoClick() {
        tomato->alive = false;
        SoundManager::getInstance()->playSound(grub02Sound);
        LOG_DEBUG("Kill tomato.");
    };
    void onPearClick() {
        pear->alive = false;
        SoundManager::getInstance()->playSound(grub03Sound);
        LOG_DEBUG("Kill pear.");
    };
    void onOrangeClick() {
        orange->alive = false;
        SoundManager::getInstance()->playSound(grub04Sound);
        LOG_DEBUG("Kill pear.");
    };
    void onGrapesClick() {
        grapes->alive = false;
        SoundManager::getInstance()->playSound(grub05Sound);
        LOG_DEBUG("Kill greapes.");
    };
    void onBannanaClick() {
        bannana->alive = false;
        SoundManager::getInstance()->playSound(grub05Sound);
        LOG_DEBUG("Kill bannana.");
    };
    void onCarrotClick() {
        carrot->alive = false;
        SoundManager::getInstance()->playSound(grub05Sound);
        LOG_DEBUG("Kill carrot.");
    };
    int backEvent() {
        activity->setStartScene();
        return 1;
    };
    int gestureSwipeEvent(int x, int y, int direction) {
        if (uiModeType != ACONFIGURATION_UI_MODE_TYPE_WATCH) return 0;
        if (direction == SWIPE_DIRECTION_RIGHT) {
            activity->setStartScene();
            return 1;
        }
        return 0;
    };
    Background* background;
    Background* gameBox;
    Line* line;
    // Sounds.
    Sound* grub01Sound;
    Sound* grub02Sound;
    Sound* grub03Sound;
    Sound* grub04Sound;
    Sound* grub05Sound;
    // Fruits.
    Fruit* apple;
    Fruit* tomato;
    Fruit* pear;
    Fruit* orange;
    Fruit* grapes;
    Fruit* bannana;
    Fruit* carrot;
};

#endif // __GAMEPLAY_H__
