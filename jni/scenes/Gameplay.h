#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "Line.h"

#define GRID_SIZE 6

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
        background = addBackground("textures/Background.png", 360, 640, Vector2(halfWidth, halfHeight));
        gameBox = addBackground("textures/GameBox.png", 360, 380, Vector2(halfWidth, halfHeight));
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
        const char* fruitTextures[7] = {
            "textures/AppleFruit.png",
            "textures/BannanaFruit.png",
            "textures/CarrotFruit.png",
            "textures/GrapesFruit.png",
            "textures/OrangeFruit.png",
            "textures/PearFruit.png",
            "textures/TomatoFruit.png"
        };
        for (int y = 0; y < GRID_SIZE; y++) {
            for (int x = 0; x < GRID_SIZE; x++) {
                int fruitType = (int)frand(7);
                fruits[x][y] = addFruit(fruitTextures[fruitType], 64, 64, Vector2(halfWidth + x * 42 - 100 - dx, halfHeight - y * 42 + 110 - dy));
                fruits[x][y]->sprite->scale = Vector2(0.9f, 0.9f);
                fruits[x][y]->setClickFunction(std::bind(&Gameplay::onFruitClick, this, std::placeholders::_1, std::placeholders::_2));
                fruits[x][y]->index = Vector2(x, y);
                fruits[x][y]->type = fruitType;
                dy += 2.0f;
            }
            dy -= 2.0f * GRID_SIZE;
            dx += 2.0f;
        }
        testBoard();
        // Line (just for test).
        line = new Line(5.0f);
        line->color = Vector(0.3f, 0.3f, 0.3f);
        // Load sounds.
        grub01Sound = SoundManager::getInstance()->registerSound("sounds/Grub01.wav");
        grub02Sound = SoundManager::getInstance()->registerSound("sounds/Grub02.wav");
        grub03Sound = SoundManager::getInstance()->registerSound("sounds/Grub03.wav");
        grub04Sound = SoundManager::getInstance()->registerSound("sounds/Grub04.wav");
        grub05Sound = SoundManager::getInstance()->registerSound("sounds/Grub05.wav");
        hey01Sound = SoundManager::getInstance()->registerSound("sounds/Hey01.wav");
        hey02Sound = SoundManager::getInstance()->registerSound("sounds/Hey02.wav");
        hey03Sound = SoundManager::getInstance()->registerSound("sounds/Hey03.wav");
        hey04Sound = SoundManager::getInstance()->registerSound("sounds/Hey04.wav");
        hey05Sound = SoundManager::getInstance()->registerSound("sounds/Hey05.wav");
        zipUp01Sound = SoundManager::getInstance()->registerSound("sounds/ZipUp01.wav");
        zipUp02Sound = SoundManager::getInstance()->registerSound("sounds/ZipUp02.wav");
        zipUp03Sound = SoundManager::getInstance()->registerSound("sounds/ZipUp03.wav");
        zipDown01Sound = SoundManager::getInstance()->registerSound("sounds/ZipDown01.wav");
        zipDown02Sound = SoundManager::getInstance()->registerSound("sounds/ZipDown02.wav");
        zipDown03Sound = SoundManager::getInstance()->registerSound("sounds/ZipDown03.wav");
        SoundManager::getInstance()->loadResources();
        created = true;
        return STATUS_OK;
    };
    int gestureTapEvent(int x, int y) {
        if (!created) return false;
        Vector2 point = GraphicsManager::getInstance()->screenToRender(x, y);
        // line->addPoint(Vector(point.x, point.y, 0.0f));
        return 0;
    };
    void update() {
        Scene::update();
    };
    int testBoard() {
        LOG_DEBUG("Testing board ...");
        int result = 0;
        int count, type;
        // Horizontal test.
        for (int y = 0; y < GRID_SIZE; y++) {
            type = -1;
            count = 1;
            for (int x = 0; x < GRID_SIZE; x++) {
                if (fruits[x][y]->type != type || !fruits[x][y]->alive) {
                    count = 1;
                    type = fruits[x][y]->type;
                } else count++;
                if (count >= 3) {
                    for (int p = x - count + 1; p <= x; p++) if (fruits[p][y] != NULL && fruits[p][y]->alive) fruits[p][y]->alive = false;
                    result += count;
                }
            }
        }
        // Vertical test.
        for (int x = 0; x < GRID_SIZE; x++) {
            type = -1;
            count = 1;
            for (int y = 0; y < GRID_SIZE; y++) {
                if (fruits[x][y]->type != type || !fruits[x][y]->alive) {
                    count = 1;
                    type = fruits[x][y]->type;
                } else count++;
                if (count >= 3) {
                    for (int p = y - count + 1; p <= y; p++) if (fruits[x][p] != NULL && fruits[x][p]->alive) fruits[x][p]->alive = false;
                    result += count;
                }
            }
        }
        return result;
    }
    void onFruitClick(int X, int Y) {
        bool swaped = false;
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (fruits[x][y]->selected && fruits[x][y]->index != fruits[X][Y]->index) {
                TweenManager::getInstance()->addTween(fruits[x][y]->sprite, TweenType::POSITION_XY, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(fruits[X][Y]->sprite->getLocation().x, fruits[X][Y]->sprite->getLocation().y)
                    ->remove(true)->start();
                TweenManager::getInstance()->addTween(fruits[X][Y]->sprite, TweenType::POSITION_XY, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(fruits[x][y]->sprite->getLocation().x, fruits[x][y]->sprite->getLocation().y)
                    ->remove(true)->start();
                std::swap(fruits[x][y], fruits[X][Y]);
                std::swap(fruits[x][y]->index, fruits[X][Y]->index);
                swaped = true;
                switch ((int)frand(3)) {
                    case 0: SoundManager::getInstance()->playSound(zipUp01Sound); break;
                    case 1: SoundManager::getInstance()->playSound(zipUp02Sound); break;
                    case 2: SoundManager::getInstance()->playSound(zipUp03Sound); break;
                }                
                break;
            }
            if (swaped) break;
        }
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            fruits[x][y]->selected = false;
        }
        if (!swaped) {
            fruits[X][Y]->selected = true;
            switch ((int)frand(5)) {
                case 0: SoundManager::getInstance()->playSound(hey01Sound); break;
                case 1: SoundManager::getInstance()->playSound(hey02Sound); break;
                case 2: SoundManager::getInstance()->playSound(hey03Sound); break;
                case 3: SoundManager::getInstance()->playSound(hey04Sound); break;
                case 4: SoundManager::getInstance()->playSound(hey05Sound); break;
            }
        } else if (testBoard() >= 3) {
            switch ((int)frand(5)) {
                case 0: SoundManager::getInstance()->playSound(grub01Sound); break;
                case 1: SoundManager::getInstance()->playSound(grub02Sound); break;
                case 2: SoundManager::getInstance()->playSound(grub03Sound); break;
                case 3: SoundManager::getInstance()->playSound(grub04Sound); break;
                case 4: SoundManager::getInstance()->playSound(grub05Sound); break;
            }
        }
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
    Sound* hey01Sound;
    Sound* hey02Sound;
    Sound* hey03Sound;
    Sound* hey04Sound;
    Sound* hey05Sound;
    Sound* zipUp01Sound;
    Sound* zipUp02Sound;
    Sound* zipUp03Sound;
    Sound* zipDown01Sound;
    Sound* zipDown02Sound;
    Sound* zipDown03Sound;    
    // Fruits.
    Fruit* fruits[GRID_SIZE][GRID_SIZE];
};

#endif // __GAMEPLAY_H__
