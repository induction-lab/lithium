#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

/* Main gameplay stuff */

#include "Fruit.h"
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
        // My sounds.
        hahaSound = SoundManager::getInstance()->registerSound("sounds/Haha.wav");
        bonus01Sound = SoundManager::getInstance()->registerSound("sounds/Bonus01.wav");
        bonus02Sound = SoundManager::getInstance()->registerSound("sounds/Bonus02.wav");
        bonus03Sound = SoundManager::getInstance()->registerSound("sounds/Bonus03.wav");
        bonus04Sound = SoundManager::getInstance()->registerSound("sounds/Bonus04.wav");
        SoundManager::getInstance()->loadResources();
        // Create score points text.
        scoreText = addRasterFont("textures/Font.png", 64, 64, Vector2(halfWidth, halfHeight - 150), Justification::MIDDLE, TextAnimation::SCALE);
        changedScoreText = addRasterFont("textures/WhiteFont.png", 64, 64, Vector2(halfWidth + 90, halfHeight - 158), Justification::RIGHT, TextAnimation::SLIDE);
        changedScoreText->scale = Vector2(0.7f, 0.7f);
        scores = configData->ScorePoints;
        updateScore(scores, true);
        // Create fruits.
        dyingFruits = 0;
        swapedFruits = 0;
        droppedFruits = 0;        
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            addFruit(x, y, configData->fruitsType[x][y]);
        }
        SoundManager::getInstance()->playSound(fruits_startSound);
        printBoard();
        matchStep = 0;
        missStep = configData->missStep;
        particleSystem = new ParticleSystem();
        lastGoodTime = lastBadTime = lastBonusTime = TimeManager::getInstance()->getTime();
        // First test for match.
        testForMatch();        
        created = true;
        return STATUS_OK;
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
    // New fruit.
    void addFruit(int x, int y, int fruitType = -1) {
        // LOG_DEBUG("Creating new fruit.");
        if (fruitType == -1) fruitType = (int)frand(7);
        Fruit* fruit = new Fruit(fruitType);
        const char* fruitTextures[8] = {
            "textures/AppleFruit.png",
            "textures/BannanaFruit.png",
            "textures/CarrotFruit.png",
            "textures/GrapesFruit.png",
            "textures/OrangeFruit.png",
            "textures/PearFruit.png",
            "textures/TomatoFruit.png",
            "textures/RedishFruit.png"
        };
        fruit->sprite = spriteBatch->registerSprite(fruitTextures[fruitType], 64, 64);
        fruit->sprite->location = getSkrewedLocation(x, -1);
        fruit->sprite->scale = Vector2(0.9f, 0.9f);
        fruit->sprite->opaque = 0.0f;
        TweenManager::getInstance()->addTween(fruit->sprite, TweenType::OPAQUE, 0.1f, Ease::Sinusoidal::InOut)->target(1.0f)->remove(true)->start();
        fruit->setClickFunction(std::bind(&Gameplay::onFruitClick, this, std::placeholders::_1, std::placeholders::_2));
        fruit->setMoveFunction(std::bind(&Gameplay::onFruitMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        fruit->setMovedFunction(std::bind(&Gameplay::onFruitMoved, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        fruit->setKillFunction(std::bind(&Gameplay::onFruitKill, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        fruit->setDyingFunction(std::bind(&Gameplay::onFruitDying, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        fruit->setDeadFunction(std::bind(&Gameplay::onFruitDead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        if (fruitType != 7) {
            fruit->moveTo(x, y, 0.0f, FruitMoveType::DROP);
        } else {
            fruit->moveTo(x, y, 0.0f, FruitMoveType::DROP_EXTRA);
        }
        fruits[x][y] = fruit;
    };
    // Update scene.
    void update() {
        Scene::update();
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (fruits[x][y] != NULL) fruits[x][y]->update();
        }
        // It's bad time...
        if (TimeManager::getInstance()->getTime() > lastGoodTime + 5.0f) {
            if (TimeManager::getInstance()->getTime() > lastBadTime + 1.0f && scores > 0) {
                lastBadTime = TimeManager::getInstance()->getTime();
                SoundManager::getInstance()->playSound(scoreSound);
                updateScore(-1);
            }
        }
        if (changedScoreText != NULL) changedScoreText->update();
        particleSystem->update();        
    };
    // Move fruits to index position. Delete dead fruits. Create new.
    void updateBoard() {
        LOG_DEBUG("Update board ...");
        printBoard(false);
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            fruits[x][y]->selected = false;
            if (fruits[x][y]->alive) {
                if (fruits[x][y]->dropped) {
                    fruits[x][y]->moveTo(x, y, 0.0f, FruitMoveType::DROP);
                }
            } else {
                spriteBatch->unregisterSprite(fruits[x][y]->sprite);
                SAFE_DELETE(fruits[x][y]);
                addFruit(x, y, -1);
            }
        }
        switch ((int)frand(3)) {
            case 0: SoundManager::getInstance()->playSound(fall_1Sound); break;
            case 1: SoundManager::getInstance()->playSound(fall_2Sound); break;
            case 2: SoundManager::getInstance()->playSound(fall_3Sound); break;                    
        }
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
                if (fruits[x][y]->type != type || fruits[x][y]->type == 7) {
                    count = 1;
                    type = fruits[x][y]->type;
                } else count++;
                // Match count 3 or great, kill it fruits.
                if (count >= MIN_MATCH_COUNT) {
                    for (int p = x - count + 1; p <= x; p++) if (fruits[p][y]->alive) {
                        fruits[p][y]->kill(0.15f, FruitKillType::DEAD);
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
                if (fruits[x][y]->type != type || fruits[x][y]->type == 7) {
                    count = 1;
                    type = fruits[x][y]->type;
                } else count++;
                // Match count 3 or great, kill it fruits.
                if (count >= MIN_MATCH_COUNT) {
                    for (int p = y + count - 1; p >= y; p--) if (fruits[x][p]->alive) {
                        fruits[x][p]->kill(0.15f, FruitKillType::DEAD);
                        result++;
                    }
                }
            }
        }
        LOG_WARN("Test result = %d", result);
        return result;
    };
    // Result check.
    void resultCheck() {
        LOG_DEBUG("Result check.");
        testForMatch();
        if (dyingFruits >= MIN_MATCH_COUNT) {
            /*
            // Show bonus text.
            if (dyingFruits >= MIN_MATCH_UNBELIEVABLE_COUNT) {
                showBonus(-4);
                updateScore(dyingFruits * matchStep);
            } else if (dyingFruits >= MIN_MATCH_WOW_COUNT) {
                showBonus(-3);
                updateScore(dyingFruits * matchStep);                
            } else if (dyingFruits >= MIN_MATCH_EXELENT_COUNT) {
                showBonus(-2);
                updateScore(dyingFruits * matchStep);
            } else if (dyingFruits >= MIN_MATCH_WONDERFUL_COUNT) {
                showBonus(-1);
                updateScore(dyingFruits * matchStep);
            } 
            */
            matchStep++;
            if (matchStep > 1) showBonus(matchStep);
            updateScore(dyingFruits);
        } else matchStep = 0;
    }
    // Select fruit.
    void onFruitClick(int X, int Y) {
        LOG_DEBUG("Select %d %d", X, Y);
        if (!fruits[X][Y]->alive) return;
        if (dyingFruits > 0) return;
        if (swapedFruits > 0) return;
        if (droppedFruits > 0) return;
        bool swaped = false;
        if (fruits[X][Y]->type == 7) {
            matchStep = 1;
            fruits[X][Y]->kill(0.0f, FruitKillType::DEAD_EXTRA);
        } else {
            // Standart click.
            // -----------------
            for (int y = 0; y < GRID_SIZE; y++)
            for (int x = 0; x < GRID_SIZE; x++) {
                if (fruits[x][y]->selected && fruits[x][y] != fruits[X][Y]) {
                    fruits[x][y]->moveTo(X, Y, 0.0f, FruitMoveType::SWAP);
                    fruits[x][y]->selected = false;
                    fruits[X][Y]->moveTo(x, y, 0.0f, FruitMoveType::SWAP);
                    fruits[X][Y]->selected = false;
                    std::swap(fruits[x][y], fruits[X][Y]);
                    swaped = true;
                    SoundManager::getInstance()->playSound(moveSound);
                }
                if (swaped) break;
            }
            if (!swaped) {
                SoundManager::getInstance()->playSound(clickSound);
                fruits[X][Y]->selected = true;
                matchStep = 0;
            } else {
                resultCheck();
            }
        }
    };
    // Fruit callbacks.
    void onFruitMove(int x, int y, FruitMoveType moveType) {
        switch (moveType) {
            case FruitMoveType::SWAP:
                swapedFruits++;
                break;
            case FruitMoveType::DROP: {
                droppedFruits++;
                break;
            }
        }
    };
    void onFruitMoved(int x, int y, FruitMoveType moveType) {
        fruits[x][y]->selected = false;
        switch (moveType) {
            case FruitMoveType::SWAP: {
                swapedFruits--;
                if (swapedFruits == 0) {
                    // Come back miss fruits.
                    if (dyingFruits < MIN_MATCH_COUNT) {
                        Vector2 prevIndex = fruits[x][y]->prevIndex;
                        fruits[x][y]->moveTo((int)prevIndex.x, (int)prevIndex.y, 0.0f, FruitMoveType::SWAP_BACK);
                        fruits[(int)prevIndex.x][(int)prevIndex.y]->moveTo(x, y, 0.0f, FruitMoveType::SWAP_BACK);
                        std::swap(fruits[x][y], fruits[(int)prevIndex.x][(int)prevIndex.y]);
                        missStep++;
                        updateScore(-2);
                        SoundManager::getInstance()->playSound(no_moveSound);
                    } else missStep = 0;
                }
                break;
            }
            case FruitMoveType::DROP: {
                LOG_DEBUG("droppedFruits = %d", droppedFruits);
                droppedFruits--;
                fruits[x][y]->dropped = false;
                if (droppedFruits == 0) {
                    resultCheck();
                    LOG_DEBUG("matchStep %d", matchStep);
                    // Add bonus redich fruit to empty place.
                    if (matchStep == 0) {
                        int n = 0;
                        bool goodPlace = false;
                        while (!goodPlace || n == GRID_SIZE * GRID_SIZE) {
                            n++;
                            int X = (int)frand(GRID_SIZE);
                            int Y = (int)frand(GRID_SIZE);
                            if (fruits[X][Y]->type != 7 && !fruits[X][Y]->dropped) {
                                LOG_DEBUG("Redish!!!");
                                Vector2 location = getSkrewedLocation(X, Y);
                                location.y = location.y + 40;                            
                                addAnimation("textures/BonusFruitCreate.png", 64, 96, location, 26, 1.2f, 0.5f);
                                LOG_DEBUG("Kill %d %d for redish.", X, Y);
                                fruits[X][Y]->kill(0.0f, FruitKillType::REPLACE);
                                goodPlace = true;
                            }
                        }
                    }
                }
                break;
            }
        }
    };
    // Fruit callbacks.
    void onFruitKill(int x, int y, FruitKillType killType) {
        if (killType == FruitKillType::DEAD) dyingFruits++;
        if (fruits[x][y]->type == 7) {
            LOG_DEBUG("Kill Redish!!!");
            for (int Y = 0; Y < GRID_SIZE; Y++)
            for (int X = 0; X < GRID_SIZE; X++) {
                fruits[X][Y]->selected = false;
            }
            int p;
            float delay;
            for (p = y + 1, delay = 0.0f; p < GRID_SIZE; p++, delay += 0.25f) if (fruits[x][p]->alive) {
                if (fruits[x][p]->type < FRUITS_COUNT) fruits[x][p]->kill(delay, FruitKillType::DEAD);
                else fruits[x][p]->kill(delay, FruitKillType::DEAD_EXTRA);
            }
            for (p = y - 1, delay = 0.0f; p >= 0; p--, delay += 0.25f) if (fruits[x][p]->alive) {
                if (fruits[x][p]->type < FRUITS_COUNT) fruits[x][p]->kill(delay, FruitKillType::DEAD);
                else fruits[x][p]->kill(delay, FruitKillType::DEAD_EXTRA);
            }
            for (p = x + 1, delay = 0.0f; p < GRID_SIZE; p++, delay += 0.25f) if (fruits[p][y]->alive) {
                if (fruits[p][y]->type < FRUITS_COUNT) fruits[p][y]->kill(delay);
                else fruits[p][y]->kill(delay, FruitKillType::DEAD_EXTRA);
            }
            for (p = x - 1, delay = 0.0f; p >= 0; p--, delay += 0.25f) if (fruits[p][y]->alive) {
                if (fruits[p][y]->type < FRUITS_COUNT) fruits[p][y]->kill(delay);
                else fruits[p][y]->kill(delay, FruitKillType::DEAD_EXTRA);
            }
            updateScore(dyingFruits);
        }
    };
    void onFruitDying(int x, int y, FruitKillType killType) {
        switch(killType) {
            case FruitKillType::DEAD_EXTRA: {
                Vector2 location = getSkrewedLocation(x, y);
                addAnimation("textures/BonusFruitKill.png", 128, 128, location, 17, 0.7f);
                lastGoodTime = lastBadTime = TimeManager::getInstance()->getTime(); // reset time limit
                break;
            }
            case FruitKillType::DEAD: {
                switch ((int)frand(3)) {
                    case 0: SoundManager::getInstance()->playSound(match_1Sound); break;
                    case 1: SoundManager::getInstance()->playSound(match_2Sound); break;
                    case 2: SoundManager::getInstance()->playSound(match_3Sound); break;
                }
                // Emit star particles.
                if (SLOW_DOWN == 1) particleSystem->emit(3, getSkrewedLocation(x, y));
                lastGoodTime = lastBadTime = TimeManager::getInstance()->getTime(); // reset time limit
                break;
            }
        }
    };
    void onFruitDead(int x, int y, FruitKillType killType) {
        switch (killType) {
            case FruitKillType::DEAD: {
                LOG_DEBUG("dyingFruits = %d", dyingFruits);
                dyingFruits--;
                // Move down all upper fruits index.
                for (int p = y; p > 0; p--) {
                    std::swap(fruits[x][p], fruits[x][p - 1]);
                    std::swap(fruits[x][p]->index, fruits[x][p - 1]->index);
                    fruits[x][p]->dropped = true;
                }
                if (dyingFruits == 0) {
                    updateBoard();
                }
                break;
            }
            case FruitKillType::REPLACE: {
                spriteBatch->unregisterSprite(fruits[x][y]->sprite);
                SAFE_DELETE(fruits[x][y]);
                addFruit(x, y, 7);
                switch ((int)frand(4)) {
                    case 0: SoundManager::getInstance()->playSound(bonus01Sound); break;
                    case 1: SoundManager::getInstance()->playSound(bonus02Sound); break;
                    case 2: SoundManager::getInstance()->playSound(bonus03Sound); break;
                    case 3: SoundManager::getInstance()->playSound(bonus04Sound); break;
                }
                break;
            }
        }
    };
    // Bonus text.
    void showBonus(int step) {
        LOG_WARN("showBonus = %d", step);
        float renderWidth = (float)GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float)GraphicsManager::getInstance()->getRenderHeight();
        float halfWidth = renderWidth / 2;
        float halfHeight = renderHeight / 2;
        Vector2 location = Vector2(halfWidth, halfHeight);
        if (step > 1) SoundManager::getInstance()->playSound(doubleSound);
        switch (step) {
            // x2, x3, x4, x5 text.
            case 2:
            case 3:
            case 4:
            case 5: {
                addBonusText("textures/MatchStepBonus.png", 214, 75, location, step - 1);
                break;
            }
            // Wow text.
            default: {
                addBonusText("textures/MatchStepBonus.png", 214, 75, location, 0);
                switch ((int)frand(3)) {
                    case 0: SoundManager::getInstance()->playSound(vo_wow_1Sound); break;
                    case 1: SoundManager::getInstance()->playSound(vo_wow_2Sound); break;
                    case 2: SoundManager::getInstance()->playSound(vo_wow_3Sound); break;
                }
                break;
            }
        }
    }    
    // Scores.
    void updateScore(int value, bool firstStart = false) {
        if (scores < 0) return;
        if (!firstStart) {
            if (missStep > 0) value *= missStep;
            if (matchStep > 1) value *= matchStep;
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
        // saveGameState();
        activity->setStartScene();
        return 1;
    };
    int gestureSwipeEvent(int x, int y, int direction) {
        if (dyingFruits > 0) return 0;
        if (uiModeType != ACONFIGURATION_UI_MODE_TYPE_WATCH) return 0;
        if (direction == SWIPE_DIRECTION_RIGHT) {
            // saveGameState();
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
    int matchStep;
    int missStep;
    // Fruits.
    Fruit* fruits[GRID_SIZE][GRID_SIZE];
    int dyingFruits;
    int swapedFruits;
    int droppedFruits;
    // Score points.
    RasterFont* scoreText;
    RasterFont* changedScoreText;
    int scores;
    // Particle system.
    ParticleSystem* particleSystem;
    // For decrase points if player long time stupid.
    float lastGoodTime;
    float lastBadTime;
    float lastBonusTime;
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
    Sound* hahaSound;
    Sound* bonus01Sound;
    Sound* bonus02Sound;
    Sound* bonus03Sound;
    Sound* bonus04Sound;
};

#endif // __GAMEPLAY_H__
