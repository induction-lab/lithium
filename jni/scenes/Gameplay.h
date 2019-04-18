#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

/* Main gameplay stuff */

#include "Fruit.h"
#include "RasterFont.h"
#include "ParticleSystem.h"

class Gameplay: public Scene {
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
        if (particleSystem != NULL) SAFE_DELETE(particleSystem);
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
        clickSound = SoundManager::getInstance()->registerSound("sounds/click.wav");
        doubleSound = SoundManager::getInstance()->registerSound("sounds/double.wav");
        fall_1Sound = SoundManager::getInstance()->registerSound("sounds/fall_1.wav");
        fall_2Sound = SoundManager::getInstance()->registerSound("sounds/fall_2.wav");
        fall_3Sound = SoundManager::getInstance()->registerSound("sounds/fall_3.wav");
        fruits_startSound = SoundManager::getInstance()->registerSound("sounds/fruits_start.wav");
        match_1Sound = SoundManager::getInstance()->registerSound("sounds/match_1.wav");
        match_2Sound = SoundManager::getInstance()->registerSound("sounds/match_2.wav");
        match_3Sound = SoundManager::getInstance()->registerSound("sounds/match_3.wav");
        moveSound = SoundManager::getInstance()->registerSound("sounds/move.wav");
        no_moveSound = SoundManager::getInstance()->registerSound("sounds/no_move.wav");
        scoreSound = SoundManager::getInstance()->registerSound("sounds/score.wav");
        vo_aha_1Sound = SoundManager::getInstance()->registerSound("sounds/vo_aha_1.wav");
        vo_aha_2Sound = SoundManager::getInstance()->registerSound("sounds/vo_aha_2.wav");
        vo_aha_3Sound = SoundManager::getInstance()->registerSound("sounds/vo_aha_3.wav");
        vo_excellent_1Sound = SoundManager::getInstance()->registerSound("sounds/vo_excellent_1.wav");
        vo_excellent_2Sound = SoundManager::getInstance()->registerSound("sounds/vo_excellent_2.wav");
        vo_excellent_3Sound = SoundManager::getInstance()->registerSound("sounds/vo_excellent_3.wav");
        vo_fine_1Sound = SoundManager::getInstance()->registerSound("sounds/vo_fine_1.wav");
        vo_unbelievable_1Sound = SoundManager::getInstance()->registerSound("sounds/vo_unbelievable_1.wav");
        vo_unbelievable_2Sound = SoundManager::getInstance()->registerSound("sounds/vo_unbelievable_2.wav");
        vo_unbelievable_3Sound = SoundManager::getInstance()->registerSound("sounds/vo_unbelievable_3.wav");
        vo_wonderful_1Sound = SoundManager::getInstance()->registerSound("sounds/vo_wonderful_1.wav");
        vo_wonderful_2Sound = SoundManager::getInstance()->registerSound("sounds/vo_wonderful_2.wav");
        vo_wonderful_3Sound = SoundManager::getInstance()->registerSound("sounds/vo_wonderful_3.wav");
        vo_wow_1Sound = SoundManager::getInstance()->registerSound("sounds/vo_wow_1.wav");
        vo_wow_2Sound = SoundManager::getInstance()->registerSound("sounds/vo_wow_2.wav");
        vo_wow_3Sound = SoundManager::getInstance()->registerSound("sounds/vo_wow_3.wav");
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
        SoundManager::getInstance()->playSound(fruits_startSound);
        printBoard();
        // Create bonus text.
        bonus = addBackground("textures/BonusText.png", 214, 75, Vector2(halfWidth, halfHeight));
        bonus->sprite->scale = Vector2(0.5f, 0.5f);
        bonus->sprite->opaque = 0.0f;
        bonus->sprite->order = 1;
        bonusAnimated = false;
        matchStep = 1;
        missStep = configData->missStep;
        particleSystem = new ParticleSystem();
        lastGoodTime = lastBadTime = TimeManager::getInstance()->getTime();
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
        fruit->setKillFunction(std::bind(&Gameplay::onFruitKill, this, std::placeholders::_1, std::placeholders::_2));
        fruit->setDeadFunction(std::bind(&Gameplay::onFruitDead, this, std::placeholders::_1, std::placeholders::_2));
        fruit->setMovedFunction(std::bind(&Gameplay::onFruitMoved, this, std::placeholders::_1, std::placeholders::_2));
        fruit->moveTo(x, y);
        fruits[x][y] = fruit;
    };
    // Update scene.
    void update() {
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (fruits[x][y] != NULL) fruits[x][y]->update();
        }
        // It's bad time...
        if (TimeManager::getInstance()->getTime() > lastGoodTime + 5.0f) {
            if (TimeManager::getInstance()->getTime() > lastBadTime + 1.0f && scores > 0) {
                lastBadTime = TimeManager::getInstance()->getTime();
                updateScore(-1);
                SoundManager::getInstance()->playSound(scoreSound);
            }
        }
        if (scoreText != NULL) scoreText->update();
        if (changedScoreText != NULL) changedScoreText->update();
        particleSystem->update();        
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
    // Move down all upper fruits index.
    void dropFruits(int x, int y) {
        LOG_DEBUG("Drop fruits ... %d %d", x, y);
        for (int p = y; p > 0; p--) {
            std::swap(fruits[x][p], fruits[x][p - 1]);
            std::swap(fruits[x][p]->index, fruits[x][p - 1]->index);
        };
    };
    // Move fruits to index position. Delete dead fruits. Create new.
    void updateBoard() {
        LOG_DEBUG("Update board ...");
        printBoard(false);
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (fruits[x][y]->alive) {
                fruits[x][y]->moveTo(x, y);
                fruits[x][y]->selected = false;
            } else {
                spriteBatch->unregisterSprite(fruits[x][y]->sprite);
                SAFE_DELETE(fruits[x][y]);
                addFruit(x, y, -1);
            }
        }
        SoundManager::getInstance()->playSound(moveSound);
        testForMatch();
    };
    // Test for match equals fruits in line.
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
                // If type not match, reset match count.
                if (fruits[x][y]->type != type) {
                    count = 1;
                    type = fruits[x][y]->type;
                } else count++;
                // Match count 3 or great, kill it fruits.
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
                // If type not match, reset match count.
                if (fruits[x][y]->type != type) {
                    count = 1;
                    type = fruits[x][y]->type;
                } else count++;
                // Match count 3 or great, kill it fruits.
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
            switch ((int)frand(3)) {
                case 0: SoundManager::getInstance()->playSound(match_1Sound); break;
                case 1: SoundManager::getInstance()->playSound(match_2Sound); break;
                case 2: SoundManager::getInstance()->playSound(match_3Sound); break;
            }
            // Show bonus text.
            if (result >= MIN_MATCH_WOW_COUNT) {
                showBonus(-1);
                updateScore(result * matchStep * 2);
            } else if (matchStep > 1) {
                showBonus(matchStep);
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
        Vector2 prevIndex;
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (fruits[x][y]->selected && (fruits[x][y]->index != fruits[X][Y]->index)) {
                prevIndex = fruits[x][y]->index;
                fruits[x][y]->prevIndex = Vector2(x, y);
                fruits[x][y]->moveTo(X, Y);
                fruits[X][Y]->prevIndex = Vector2(X, Y);
                fruits[X][Y]->moveTo(x, y);
                std::swap(fruits[x][y], fruits[X][Y]);
                swaped = true;
                SoundManager::getInstance()->playSound(moveSound);
            }
            if (swaped) break;
        }
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            fruits[x][y]->selected = false;
        }
        if (!swaped) {
            SoundManager::getInstance()->playSound(clickSound);
            fruits[X][Y]->selected = true;
            matchStep = 1;
        } else if (testForMatch() < MIN_MATCH_COUNT) {
            std::swap(fruits[(int)prevIndex.x][(int)prevIndex.y], fruits[X][Y]);
            std::swap(fruits[(int)prevIndex.x][(int)prevIndex.y]->index, fruits[X][Y]->index);
            missStep++;
            updateScore(-2 * missStep);
        } else missStep = 0;
    };
    // Fruit callbacks.
    void onFruitKill(int x, int y) {
        // Reset time limit.
        lastGoodTime = lastBadTime = TimeManager::getInstance()->getTime();
        // Emit star particles.
        particleSystem->emit(3, getSkrewedLocation(x, y));
    };
    void onFruitDead(int x, int y) {
        dyingFruits--;
        dropFruits(x, y);
        if (dyingFruits <= 0) updateBoard();
    };
    void onFruitMoved(int x, int y) {
        if (missStep > 0) {
            lastGoodTime = lastBadTime = TimeManager::getInstance()->getTime();
            // Come back miss fruits.
            SoundManager::getInstance()->playSound(no_moveSound);
            fruits[x][y]->moveBack(0.05f);
        }
    };
    // Bonus text.
    void showBonus(int step) {
        switch (step) {
            case 2: bonus->sprite->setFrame(1); break;
            case 3: bonus->sprite->setFrame(2); break;
            case 4: bonus->sprite->setFrame(3); break;
            case 5: bonus->sprite->setFrame(4); break;
            default: {
                bonus->sprite->setFrame(0);
                switch ((int)frand(3)) {
                    case 0: SoundManager::getInstance()->playSound(vo_wow_1Sound); break;
                    case 1: SoundManager::getInstance()->playSound(vo_wow_2Sound); break;
                    case 2: SoundManager::getInstance()->playSound(vo_wow_3Sound); break;
                }                
            }
        }
        if (step > 1 && step < 6) SoundManager::getInstance()->playSound(doubleSound);
        Tween* t1 = TweenManager::getInstance()->addTween(bonus->sprite, TweenType::OPAQUE, 0.25f, Ease::Sinusoidal::InOut)->target(1.0f)->remove(true)->start();
        Tween* t2 = TweenManager::getInstance()->addTween(bonus->sprite, TweenType::SCALE_XY, 0.25f, Ease::Back::Out)->target(1.0f, 1.0f)->remove(true);
        Tween* t3 = TweenManager::getInstance()->addTween(bonus->sprite, TweenType::SCALE_XY, 0.15f, Ease::Back::Out)->target(0.5f, 0.5f)->remove(true)->delay(0.7f);
        Tween* t4 = TweenManager::getInstance()->addTween(bonus->sprite, TweenType::OPAQUE, 0.15f, Ease::Sinusoidal::InOut)->target(0.0f)->remove(true)->delay(0.7f);
        t2->addChain(t3);
        t2->addChain(t4);
        t2->start();
    }
    // Scores.
    void updateScore(int value, bool firstStart = false) {
        if (scores < 0) return;
        if (!firstStart) {
            if (scores + value < 0) value = -scores;
            scores += value;
            std::string changedStr = (value > 0) ? "+" + std::to_string(value) : std::to_string(value);
            if (value != 0) changedScoreText->setText(changedStr.c_str());
            if (value > 0) lastGoodTime = lastBadTime = TimeManager::getInstance()->getTime();
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
    // Save state.
    void saveGameState() {
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            configData->fruitsType[x][y] = fruits[x][y]->type;
        }
        configData->ScorePoints = scores;
    };
private:
    Activity* activity;
    // Decorations.
    Background* background;
    Background* gameBox;
    // Bonus text.
    Background* bonus;
    bool bonusAnimated;
    int matchStep;
    int missStep;
    // Fruits.
    Fruit* fruits[GRID_SIZE][GRID_SIZE];
    int dyingFruits;
    // Score points.
    RasterFont* scoreText;
    RasterFont* changedScoreText;
    int scores;
    // Particle system.
    ParticleSystem* particleSystem;
    // For decrase points if player long time stupid.
    float lastGoodTime;
    float lastBadTime;    
    // Sounds.
    Sound* clickSound;
    Sound* doubleSound;
    Sound* fall_1Sound;
    Sound* fall_2Sound;
    Sound* fall_3Sound;
    Sound* fruits_startSound;
    Sound* match_1Sound;
    Sound* match_2Sound;
    Sound* match_3Sound;
    Sound* moveSound;
    Sound* no_moveSound;
    Sound* scoreSound;
    Sound* vo_aha_1Sound;
    Sound* vo_aha_2Sound;
    Sound* vo_aha_3Sound;
    Sound* vo_excellent_1Sound;
    Sound* vo_excellent_2Sound;
    Sound* vo_excellent_3Sound;
    Sound* vo_fine_1Sound;
    Sound* vo_unbelievable_1Sound;
    Sound* vo_unbelievable_2Sound;
    Sound* vo_unbelievable_3Sound;
    Sound* vo_wonderful_1Sound;
    Sound* vo_wonderful_2Sound;
    Sound* vo_wonderful_3Sound;
    Sound* vo_wow_1Sound;
    Sound* vo_wow_2Sound;
    Sound* vo_wow_3Sound;
};

#endif // __GAMEPLAY_H__
