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
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            SAFE_DELETE(fruits[x][y]);
        }
        if (scoreText != NULL) SAFE_DELETE(scoreText);
        if (changedScoreText != NULL) SAFE_DELETE(changedScoreText);
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
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_X, 0.37f, Ease::Sinusoidal::InOut)
            ->target(1.03f)->remove(false)->loop()->reverse()->start();
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_Y, 0.37f, Ease::Sinusoidal::InOut)
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
        hurt01Sound = SoundManager::getInstance()->registerSound("sounds/Hurt01.wav");
        hurt02Sound = SoundManager::getInstance()->registerSound("sounds/Hurt02.wav");
        hurt03Sound = SoundManager::getInstance()->registerSound("sounds/Hurt03.wav");
        hurt04Sound = SoundManager::getInstance()->registerSound("sounds/Hurt04.wav");
        hurt05Sound = SoundManager::getInstance()->registerSound("sounds/Hurt05.wav");
        wowSound = SoundManager::getInstance()->registerSound("sounds/Wow.wav");
        hahaSound = SoundManager::getInstance()->registerSound("sounds/Haha.wav");
        SoundManager::getInstance()->loadResources();
        // Create score points text.
        scoreText = new RasterFont("textures/Font.png", 64, 64, Vector2(halfWidth, halfHeight - 150), Justification::MIDDLE, TextAnimation::SCALE);
        changedScoreText = new RasterFont("textures/WhiteFont.png", 64, 64, Vector2(halfWidth + 90, halfHeight - 158), Justification::RIGHT, TextAnimation::SLIDE);
        changedScoreText->scale = Vector2(0.7f, 0.7f);
        scores = configData->ScorePoints;
        updateScore(scores, true);
        // Create fruits.
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            addFruit(x, y, configData->fruitsType[x][y]);
        }
        switch ((int)frand(3)) {
            case 0: SoundManager::getInstance()->playSound(zipDown01Sound); break;
            case 1: SoundManager::getInstance()->playSound(zipDown02Sound); break;
            case 2: SoundManager::getInstance()->playSound(zipDown03Sound); break;
        }
        printBoard();
        // Create bonus text.
        bonusText = addBackground("textures/BonusText.png", 214, 75, Vector2(halfWidth, halfHeight));
        bonusText->sprite->scale = Vector2(0.5f, 0.5f);
        bonusText->sprite->opaque = 0.0f;
        bonusText->sprite->order = 1;
        bonusTextAnimated = false;
        matchStep = 1;
        missStep = configData->missStep;
        // First test for match.
        testForMatch();
        created = true;
        return STATUS_OK;
    };
    // New fruit.
    void addFruit(int x, int y, int fruitType = -1) {
        // LOG_DEBUG("Creating new fruit.");
        if (fruitType == -1) fruitType = (int)frand(7);
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
        fruit->sprite->location = getSkrewedLocation(x, -1);
        fruit->sprite->scale = Vector2(0.9f, 0.9f);
        fruit->sprite->opaque = 0.0f;
        TweenManager::getInstance()->addTween(fruit->sprite, TweenType::OPAQUE, 0.1f, Ease::Sinusoidal::InOut)->target(1.0f)->remove(true)->start();
        fruit->setClickFunction(std::bind(&Gameplay::onFruitClick, this, std::placeholders::_1, std::placeholders::_2));
        fruit->setDeadFunction(std::bind(&Gameplay::onFruitDead, this, std::placeholders::_1, std::placeholders::_2));
        fruit->setMovedFunction(std::bind(&Gameplay::onFruitMoved, this, std::placeholders::_1, std::placeholders::_2));
        fruit->moveTo(x, y);
        fruits[x][y] = fruit ;
    };
    // Update scene.
    void update() {
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (fruits[x][y] != NULL) fruits[x][y]->update();
        }
        if (scoreText != NULL) scoreText->update();
        if (changedScoreText != NULL) changedScoreText->update();
    };
    // Just for debug.
    void printBoard(bool full = true) {
        LOG_DEBUG("-------------------------------------");
        for (int y = 0; y < GRID_SIZE; y++) {
            std::string str = "|";
            for (int x = 0; x < GRID_SIZE; x++) {
                if (full) {
                    str += std::to_string((int)fruits[x][y]->index.x) + " ";
                    str += std::to_string((int)fruits[x][y]->index.y) + " ";
                }
                str += std::to_string((int)fruits[x][y]->type) + "|";
            }
            str += "\n";
            LOG_DEBUG("%s", str.c_str());
        }
    };    
    void dropFruits(int x, int y) {
        LOG_DEBUG("Drop fruits ... %d %d", x, y);
        // Move down all upper fruits.
        for (int p = y; p > 0; p--) {
            std::swap(fruits[x][p], fruits[x][p - 1]);
            std::swap(fruits[x][p]->index, fruits[x][p - 1]->index);
        };
    };
    void updateBoard() {
        LOG_DEBUG("Update board ...");
        printBoard(false);
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (fruits[x][y]->alive) {
                fruits[x][y]->moveTo(x, y);
                fruits[x][y]->selected = false;
            } else {
                SAFE_DELETE(fruits[x][y]);
                addFruit(x, y, -1);
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
        LOG_DEBUG("Testing match fruits ...");
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
                if (fruits[x][y]->type != type) {
                    count = 1;
                    type = fruits[x][y]->type;
                } else count++;
                // Mutch count 3 or great, kill it fruits.
                if (count >= MIN_MATCH_COUNT) {
                    for (int p = x - count + 1; p <= x; p++) if (fruits[p][y]->alive) {
                        fruits[p][y]->kill();
                        result++;
                    }
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
                if (fruits[x][y]->type != type) {
                    count = 1;
                    type = fruits[x][y]->type;
                } else count++;
                // Mutch count 3 or great, kill it fruits.
                if (count >= MIN_MATCH_COUNT) {
                    for (int p = y + count - 1; p >= y; p--) if (fruits[x][p]->alive) {
                        fruits[x][p]->kill();
                        result++;
                    }
                }
            }
        }
        // Result check.
        if (result >= MIN_MATCH_COUNT) {
            switch ((int)frand(5)) {
                case 0: SoundManager::getInstance()->playSound(grub01Sound); break;
                case 1: SoundManager::getInstance()->playSound(grub02Sound); break;
                case 2: SoundManager::getInstance()->playSound(grub03Sound); break;
                case 3: SoundManager::getInstance()->playSound(grub04Sound); break;
                case 4: SoundManager::getInstance()->playSound(grub05Sound); break;
            }
            // Show bones text.
            if (result >= MIN_MATCH_WOW_COUNT) {
                showBonusText(-1);
                updateScore(result * matchStep * 2);
            } else if (matchStep > 1) {
                showBonusText(matchStep);
                updateScore(result * matchStep);
            } else updateScore(result);
            matchStep++;
        } else matchStep = 1;
        dyingFruits = result;
        return result;
    };
    // Select fruit.
    void onFruitClick(int X, int Y) {
        LOG_DEBUG("click %d %d", X, Y);
        if (!fruits[X][Y]->alive) return;
        if (dyingFruits > 0) return;
        bool swaped = false;
        Vector2 s;
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (fruits[x][y]->selected && (fruits[x][y]->index != fruits[X][Y]->index)) {
                s = fruits[x][y]->index;
                fruits[x][y]->prevIndex = Vector2(x, y);
                fruits[x][y]->moveTo(X, Y);
                fruits[X][Y]->prevIndex = Vector2(X, Y);
                fruits[X][Y]->moveTo(x, y);
                std::swap(fruits[x][y], fruits[X][Y]);
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
            switch ((int)frand(3)) {
                case 0: SoundManager::getInstance()->playSound(Lift01Sound); break;
                case 1: SoundManager::getInstance()->playSound(Lift02Sound); break;
                case 2: SoundManager::getInstance()->playSound(Lift03Sound); break;
            }
            fruits[X][Y]->selected = true;
            matchStep = 1;
        } else if (testForMatch() < MIN_MATCH_COUNT) {
            std::swap(fruits[(int)s.x][(int)s.y], fruits[X][Y]);
            std::swap(fruits[(int)s.x][(int)s.y]->index, fruits[X][Y]->index);
            missStep++;
            updateScore(-2 * missStep);
        } else missStep = 0;
    };
    void onFruitDead(int x, int y) {
        dyingFruits--;
        dropFruits(x, y);
        if (dyingFruits <= 0) updateBoard();
    };
    void onFruitMoved(int x, int y) {
        if (missStep > 0) {
            fruits[x][y]->moveBack(0.15f);
            switch ((int)frand(5)) {
                case 0: SoundManager::getInstance()->playSound(hurt01Sound); break;
                case 1: SoundManager::getInstance()->playSound(hurt02Sound); break;
                case 2: SoundManager::getInstance()->playSound(hurt03Sound); break;
                case 3: SoundManager::getInstance()->playSound(hurt04Sound); break;
                case 4: SoundManager::getInstance()->playSound(hurt05Sound); break;
            }
        }
    };
    // Bonus text.
    void showBonusText(int step) {
        if (!bonusTextAnimated) {
            switch (step) {
                case 2: bonusText->sprite->setFrame(1); break;
                case 3: bonusText->sprite->setFrame(2); break;
                case 4: bonusText->sprite->setFrame(3); break;
                case 5: bonusText->sprite->setFrame(4); break;
                default: {
                    bonusText->sprite->setFrame(0);
                    SoundManager::getInstance()->playSound(wowSound);
                }
            }
            if (step > 1) SoundManager::getInstance()->playSound(hahaSound);
            Tween* t1 = TweenManager::getInstance()->addTween(bonusText->sprite, TweenType::OPAQUE, 0.25f, Ease::Sinusoidal::InOut)->target(1.0f)->remove(true)->start();
            Tween* t2 = TweenManager::getInstance()->addTween(bonusText->sprite, TweenType::SCALE_XY, 0.25f, Ease::Back::Out)->target(1.0f, 1.0f)->remove(true);
            Tween* t3 = TweenManager::getInstance()->addTween(bonusText->sprite, TweenType::SCALE_XY, 0.25f, Ease::Back::Out)->target(0.5f, 0.5f)->remove(true)->delay(0.7f);
            Tween* t4 = TweenManager::getInstance()->addTween(bonusText->sprite, TweenType::OPAQUE, 0.25f, Ease::Sinusoidal::InOut)->target(0.0f)->remove(true)->delay(0.7f)
                ->onComplete(std::bind(&Gameplay::onBonusTextComplete, this));
            t2->addChain(t3);
            t2->addChain(t4);
            t2->start();
            bonusTextAnimated = true;
        }        
    }
    void onBonusTextComplete() {
        bonusTextAnimated = false;
    };
    // Scores.
    void updateScore(int value, bool firstStart = false) {
        if (scores < 0) return;
        if (!firstStart) {
            if (scores + value < 0) scores = 0;
            else scores += value;
            std::string changedStr = (value > 0) ? "+" + std::to_string(value) : std::to_string(value);
            changedScoreText->setText(changedStr.c_str());
        }
        std::string str = std::to_string(scores);
        while (str.size() < 7) str = "0" + str;
        scoreText->setText(str.c_str());
    };
    // Back to main scene.
    int backEvent() {
        if (dyingFruits > 0) return 0;
        saveGameState();
        activity->setStartScene();
        return 1;
    };
    int gestureSwipeEvent(int x, int y, int direction) {
        if (dyingFruits > 0) return 0;
        if (uiModeType != ACONFIGURATION_UI_MODE_TYPE_WATCH) return 0;
        if (direction == SWIPE_DIRECTION_RIGHT) {
            saveGameState();
            activity->setStartScene();
            return 1;
        }
        return 0;
    };
    void saveGameState() {
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            configData->fruitsType[x][y] = fruits[x][y]->type;
        }
        configData->ScorePoints = scores;
    };
    Background* background;
    Background* gameBox;
    // Bonus text.
    Background* bonusText;
    bool bonusTextAnimated;
    int matchStep;
    int missStep;
    // Fruits.
    int dyingFruits;
    Fruit* fruits[GRID_SIZE][GRID_SIZE];
    // Score points.
    RasterFont* scoreText;
    RasterFont* changedScoreText;
    int scores;
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
    Sound* wowSound;
    Sound* hahaSound;
    Sound* hurt01Sound;
    Sound* hurt02Sound;
    Sound* hurt03Sound;
    Sound* hurt04Sound;
    Sound* hurt05Sound;
};

#endif // __GAMEPLAY_H__
