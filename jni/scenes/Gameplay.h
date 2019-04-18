#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "Line.h"

#define GRID_SIZE 8

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
        // Create fruits.
        float dx = 0.0f;
        float dy = 0.0f;
        for (int j = 0; j < GRID_SIZE; j++) {
            for (int i = 0; i < GRID_SIZE; i++) {
                const char* fruitTextures[7] = {
                    "textures/AppleFruit.png",
                    "textures/BannanaFruit.png",
                    "textures/CarrotFruit.png",
                    "textures/GrapesFruit.png",
                    "textures/OrangeFruit.png",
                    "textures/PearFruit.png",
                    "textures/TomatoFruit.png"
                };
                int n = (int)frand(6);
                fruits[i][j] = addFruit(fruitTextures[n], 64, 64, Location(i * 32 + 72 - dx, renderHeight - j * 32 - 64 - dy));
                fruits[i][j]->sprite->scale = Vector2(0.7f, 0.7f);
                fruits[i][j]->setClickFunction(std::bind(&Gameplay::onFruitClick, this, std::placeholders::_1));
                dy += 2.0f;
            }
            dy -= 2.0f * GRID_SIZE;
            dx += 2.0f;
        }
        // Line (just for test).
        line = new Line(5.0f);
        line->color = Vector(0.3f, 0.3f, 0.3f);
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
        line->addPoint(Vector(point.x, point.y, 0.0f));
        return 0;
    };
    void update() {
        Scene::update();
    };
    void onFruitClick(Fruit* fruit) {
        fruit->alive = false;
        LOG_DEBUG("Kill fruit.");
        switch ((int)frand(4)) {
            case 0: SoundManager::getInstance()->playSound(grub01Sound); break;
            case 1: SoundManager::getInstance()->playSound(grub02Sound); break;
            case 2: SoundManager::getInstance()->playSound(grub03Sound); break;
            case 3: SoundManager::getInstance()->playSound(grub04Sound); break;
            case 4: SoundManager::getInstance()->playSound(grub05Sound); break;
        }
    }
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
    Fruit* fruits[GRID_SIZE][GRID_SIZE];
};

#endif // __GAMEPLAY_H__
