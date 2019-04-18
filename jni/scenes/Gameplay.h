#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "Fruit.h"
#include "RasterFont.h"

class Gameplay: public Scene {
private:
    Activity* activity;
public:
    Gameplay(Activity* activity):
        activity(activity) {
        LOG_INFO("Create Gameplay scene.");
    };
    ~Gameplay() {
        LOG_INFO("Destroy Gameplay scene.");
        for (std::vector<Fruit*>::iterator it = fruits.begin(); it < fruits.end(); ++it) {
            SAFE_DELETE(*it);
        }
        fruits.clear();
        if (rasterFont != NULL) SAFE_DELETE(rasterFont);
    };
    status start() {
        if (created) return STATUS_OK;
        LOG_DEBUG("Start Gameplay scene.");
        spriteBatch = new SpriteBatch();
        float renderWidth = (float)GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float)GraphicsManager::getInstance()->getRenderHeight();
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
        Lift01Sound = SoundManager::getInstance()->registerSound("sounds/Lift01.wav");
        Lift02Sound = SoundManager::getInstance()->registerSound("sounds/Lift02.wav");
        Lift03Sound = SoundManager::getInstance()->registerSound("sounds/Lift03.wav");        
        zipUp01Sound = SoundManager::getInstance()->registerSound("sounds/ZipUp01.wav");
        zipUp02Sound = SoundManager::getInstance()->registerSound("sounds/ZipUp02.wav");
        zipUp03Sound = SoundManager::getInstance()->registerSound("sounds/ZipUp03.wav");
        zipDown01Sound = SoundManager::getInstance()->registerSound("sounds/ZipDown01.wav");
        zipDown02Sound = SoundManager::getInstance()->registerSound("sounds/ZipDown02.wav");
        zipDown03Sound = SoundManager::getInstance()->registerSound("sounds/ZipDown03.wav");
        SoundManager::getInstance()->loadResources();
        // Create score points bar.
        rasterFont = new RasterFont("textures/Font.png", 64, 64, Vector2(halfWidth, halfHeight - 150), Justification::MIDDLE);
        rasterFont->setText("0000000");
        scores = 0;
        // Create fruits.
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) addFruit(x, y);
        switch ((int)frand(3)) {
            case 0: SoundManager::getInstance()->playSound(zipDown01Sound); break;
            case 1: SoundManager::getInstance()->playSound(zipDown02Sound); break;
            case 2: SoundManager::getInstance()->playSound(zipDown03Sound); break;
        }
        testForMatch();
        created = true;
        return STATUS_OK;
    };
    // Some black magic.
    Vector2 getSkrewedLocation(int X, int Y) {
        float renderWidth = (float) GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float) GraphicsManager::getInstance()->getRenderHeight();
        float halfWidth = renderWidth / 2;
        float halfHeight = renderHeight / 2;
        float dy = (X * 2.0f) - (0.2f * GRID_SIZE * Y);
        float dx = (Y * 2.0f);
        return Vector2(halfWidth + X * 42 - 100 - dx, halfHeight - Y * 42 + 110 - dy);
    };
    void addFruit(int X, int Y, bool loadFromSave = false) {
        // LOG_INFO("Creating new fruit.");
        int fruitType = (int)frand(7);
        Fruit* fruit = new Fruit(fruitType);
        const char* fruitTextures[7] = {
            "textures/AppleFruit.png",
            "textures/BannanaFruit.png",
            "textures/CarrotFruit.png",
            "textures/GrapesFruit.png",
            "textures/OrangeFruit.png",
            "textures/PearFruit.png",
            "textures/TomatoFruit.png"
        };
        fruit->sprite = spriteBatch->registerSprite(fruitTextures[fruitType], 64, 64);
        fruit->sprite->location = getSkrewedLocation(X, -1);
        fruit->sprite->scale = Vector2(0.9f, 0.9f);
        fruit->sprite->opaque = 0.0f;
        TweenManager::getInstance()->addTween(fruit->sprite, TweenType::OPAQUE, 0.1f, Ease::Sinusoidal::InOut)->target(1.0f)->remove(true)->start();
        fruit->setClickFunction(std::bind(&Gameplay::onFruitClick, this, std::placeholders::_1, std::placeholders::_2));
        fruit->setDeadFunction(std::bind(&Gameplay::onFruitDead, this, std::placeholders::_1, std::placeholders::_2));
        fruit->moveTo(getSkrewedLocation(X, Y));
        fruit->index = Vector2(X, Y);
        fruits.push_back(fruit);
    };
    // Get fruit by index.
    Fruit* getFruit(int X, int Y) {
        for (std::vector<Fruit*>::iterator it = fruits.begin(); it < fruits.end(); ++it) {
            if ((*it)->index.x == X && (*it)->index.y == Y) return (*it);
        }
        return NULL;
    };
    void update() {
        Scene::update();
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (getFruit(x, y) != NULL) getFruit(x, y)->update();
        }
        if (rasterFont != NULL) rasterFont->update();
    };
    void dropFruits(int X, int Y) {
        // LOG_DEBUG("Drop fruits ...");
        // Backup dead fruit index.
        getFruit(X, Y)->index.y = -1;
        // Move down all upper fruits.
        for (int y = Y - 1; y >= 0; y--) getFruit(X, y)->index.y = y + 1;
        // Restore dead fruit index.
        getFruit(X, -1)->index.y = 0;
    };
    void updateBoard() {
        // LOG_DEBUG("Update board ...");
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (getFruit(x, y)->dead) {
                // Delete fruit.
                spriteBatch->unregisterSprite(getFruit(x, y)->sprite);
                std::vector<Fruit*>::iterator it = find(fruits.begin(), fruits.end(), getFruit(x, y));
                if (it != fruits.end()) {
                    SAFE_DELETE(*it);
                    fruits.erase(it);
                }
                addFruit(x, y);
            } else if (getFruit(x, y)->sprite->location != getSkrewedLocation(x, y)) {
                // Move fruits to their location.
                getFruit(x, y)->moveTo(getSkrewedLocation(x, y));
                getFruit(x, y)->selected = false;
            }
        }
        switch ((int)frand(3)) {
            case 0: SoundManager::getInstance()->playSound(zipDown01Sound); break;
            case 1: SoundManager::getInstance()->playSound(zipDown02Sound); break;
            case 2: SoundManager::getInstance()->playSound(zipDown03Sound); break;
        }
        testForMatch();
    };
    int testForMatch() {
        // LOG_DEBUG("Testing match fruits ...");
        int result = 0;
        int count, type;
        // Horizontal test.
        //   ______
        //  | > o
        //  | o o
        //
        for (int y = 0; y < GRID_SIZE; y++) {
            type = -1;
            count = 1;
            for (int x = 0; x < GRID_SIZE; x++) {
                // If type not mutch, reset match count.
                if (getFruit(x, y)->type != type) {
                    count = 1;
                    type = getFruit(x, y)->type;
                } else count++;
                // Mutch count 3 or great, kill it fruits.
                if (count >= MIN_MATCH_COUNT) {
                    for (int p = x - count + 1; p <= x; p++) getFruit(p, y)->kill();
                    result += count;
                }
            }
        }
        // Vertical test.
        //   ______
        //  | o o 
        //  | ^ o 
        //
        for (int x = 0; x < GRID_SIZE; x++) {
            type = -1;
            count = 1;
            for (int y = GRID_SIZE - 1; y >= 0; y--) {
                // If type not mutch, reset match count.
                if (getFruit(x, y)->type != type) {
                    count = 1;
                    type = getFruit(x, y)->type;
                } else count++;
                // Mutch count 3 or great, kill it fruits.
                if (count >= MIN_MATCH_COUNT) {
                    for (int p = y + count - 1; p >= y; p--) getFruit(x, p)->kill();
                    result += count;
                }
            }
        }
        if (result >= MIN_MATCH_COUNT) {
            switch ((int)frand(5)) {
                case 0: SoundManager::getInstance()->playSound(grub01Sound); break;
                case 1: SoundManager::getInstance()->playSound(grub02Sound); break;
                case 2: SoundManager::getInstance()->playSound(grub03Sound); break;
                case 3: SoundManager::getInstance()->playSound(grub04Sound); break;
                case 4: SoundManager::getInstance()->playSound(grub05Sound); break;
            }
            updateScore(result);
        }
        dyingFruits = result;
        return result;
    };
    void onFruitClick(int X, int Y) {
        if (!getFruit(X, Y)->alive) return;
        if (dyingFruits > 0) return;
        bool swaped = false;
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (getFruit(x, y)->selected && getFruit(x, y)->index != getFruit(X, Y)->index) {
                getFruit(x, y)->moveTo(getSkrewedLocation(X, Y));
                getFruit(X, Y)->moveTo(getSkrewedLocation(x, y));
                std::swap(getFruit(x, y)->index, getFruit(X, Y)->index);
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
            getFruit(x, y)->selected = false;
        }
        if (!swaped) {
            getFruit(X, Y)->selected = true;
            /*
            switch ((int)frand(5)) {
                case 0: SoundManager::getInstance()->playSound(hey01Sound); break;
                case 1: SoundManager::getInstance()->playSound(hey02Sound); break;
                case 2: SoundManager::getInstance()->playSound(hey03Sound); break;
                case 3: SoundManager::getInstance()->playSound(hey04Sound); break;
                case 4: SoundManager::getInstance()->playSound(hey05Sound); break;
            }
            */
            switch ((int)frand(3)) {
                case 0: SoundManager::getInstance()->playSound(Lift01Sound); break;
                case 1: SoundManager::getInstance()->playSound(Lift02Sound); break;
                case 2: SoundManager::getInstance()->playSound(Lift03Sound); break;
            }
        } else if (testForMatch() < 3) updateScore(-2);
    };
    void onFruitDead(int X, int Y) {
        dyingFruits--;
        dropFruits(X, Y);
        if (dyingFruits <= 0) updateBoard();
    };
    void updateScore(int value) {
        scores += value;
        if (scores < 0) scores = 0;
        std::string str = std::to_string(scores);
        while (str.size() < 7) str = "0" + str;
        rasterFont->setText(str.c_str());        
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
    Sound* Lift01Sound;
    Sound* Lift02Sound;
    Sound* Lift03Sound;
    Sound* zipUp01Sound;
    Sound* zipUp02Sound;
    Sound* zipUp03Sound;
    Sound* zipDown01Sound;
    Sound* zipDown02Sound;
    Sound* zipDown03Sound;    
    // Fruits.
    int dyingFruits;
    std::vector<Fruit*> fruits;
    // Score points.
    RasterFont* rasterFont;
    int scores;
};

#endif // __GAMEPLAY_H__
