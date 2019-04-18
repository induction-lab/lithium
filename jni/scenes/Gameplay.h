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
        // Animated leafs.
        if (uiModeType != ACONFIGURATION_UI_MODE_TYPE_WATCH) {
            //  ____
            // |   |
            // |__·|
            //           
            leaf01 = addBackground("textures/Leafs.png", 327, 287, Vector2(renderWidth - 105.0f, 90.0f));
            leaf01->sprite->setFrame(0);
            leaf01->sprite->order = 1;
            leaf01->sprite->pivot = Vector(162.0f, -142.0f, 0.0f);
            onLeafTweenComplete((Tweenable*)leaf01->sprite);
            //  ____
            // |   |
            // |·__|
            //      
            leaf02 = addBackground("textures/Leafs.png", 327, 287, Vector2(30.0f, 30.0f));
            leaf02->sprite->setFrame(1);
            leaf02->sprite->order = 1;
            leaf02->sprite->pivot = Vector(-162.0f, -142.0f, 0.0f);
            onLeafTweenComplete((Tweenable*)leaf02->sprite);
            //  ____
            // |   |
            // |_·_|
            //
            leaf03 = addBackground("textures/Leafs.png", 327, 287, Vector2(halfWidth + 35.0f, 40.0f));
            leaf03->sprite->setFrame(2);
            leaf03->sprite->order = 0;
            leaf03->sprite->pivot = Vector(0.0f, -142.0f, 0.0f);
            onLeafTweenComplete((Tweenable*)leaf03->sprite);
            //  ____
            // |·  |
            // |___|
            //            
            leaf04 = addBackground("textures/Leafs.png", 327, 287, Vector2(130.0f, renderHeight - 120.0f));
            leaf04->sprite->setFrame(3);
            leaf04->sprite->order = 1;
            leaf04->sprite->pivot = Vector(-162.0f, 142.0f, 0.0f);
            onLeafTweenComplete((Tweenable*)leaf04->sprite);
            //  ____
            // |  ·|
            // |___|
            //            
            leaf05 = addBackground("textures/Leafs.png", 327, 287, Vector2(renderWidth - 10.0f, renderHeight - 60.0f));
            leaf05->sprite->setFrame(4);
            leaf05->sprite->order = 1;
            leaf05->sprite->pivot = Vector(162.0f, 142.0f, 0.0f);
            onLeafTweenComplete((Tweenable*)leaf05->sprite);
            //  ____
            // | · |
            // |___|
            //            
            leaf06 = addBackground("textures/Leafs.png", 327, 287, Vector2(halfWidth + 50.0f, renderHeight - 120.0f));
            leaf06->sprite->setFrame(5);
            leaf06->sprite->order = 0;
            leaf06->sprite->pivot = Vector(0.0f, 142.0f, 0.0f);
            onLeafTweenComplete((Tweenable*)leaf06->sprite);
        }
        // Load sounds.
        clickSound = SoundManager::getInstance()->registerSound("sounds/Click.wav");
        doubleSound = SoundManager::getInstance()->registerSound("sounds/Double.wav");
        fall01Sound = SoundManager::getInstance()->registerSound("sounds/Fall01.wav");
        fall02Sound = SoundManager::getInstance()->registerSound("sounds/Fall02.wav");
        fall03Sound = SoundManager::getInstance()->registerSound("sounds/Fall03.wav");
        fruitsStartSound = SoundManager::getInstance()->registerSound("sounds/FruitsStart.wav");
        match01Sound = SoundManager::getInstance()->registerSound("sounds/Match01.wav");
        match02Sound = SoundManager::getInstance()->registerSound("sounds/Match02.wav");
        match03Sound = SoundManager::getInstance()->registerSound("sounds/Match03.wav");
        moveSound = SoundManager::getInstance()->registerSound("sounds/Move.wav");
        noMoveSound = SoundManager::getInstance()->registerSound("sounds/NoMove.wav");
        scoreSound = SoundManager::getInstance()->registerSound("sounds/Score.wav");
        excellent01Sound = SoundManager::getInstance()->registerSound("sounds/Excellent01.wav");
        excellent02Sound = SoundManager::getInstance()->registerSound("sounds/Excellent02.wav");
        excellent03Sound = SoundManager::getInstance()->registerSound("sounds/Excellent03.wav");
        fineSound = SoundManager::getInstance()->registerSound("sounds/Fine.wav");
        unbelievable01Sound = SoundManager::getInstance()->registerSound("sounds/Unbelievable01.wav");
        unbelievable02Sound = SoundManager::getInstance()->registerSound("sounds/Unbelievable02.wav");
        unbelievable03Sound = SoundManager::getInstance()->registerSound("sounds/Unbelievable03.wav");
        wonderful01Sound = SoundManager::getInstance()->registerSound("sounds/Wonderful01.wav");
        wonderful02Sound = SoundManager::getInstance()->registerSound("sounds/Wonderful02.wav");
        wonderful03Sound = SoundManager::getInstance()->registerSound("sounds/Wonderful03.wav");
        wow01Sound = SoundManager::getInstance()->registerSound("sounds/Wow01.wav");
        wow02Sound = SoundManager::getInstance()->registerSound("sounds/Wow02.wav");
        wow03Sound = SoundManager::getInstance()->registerSound("sounds/Wow03.wav");
        bonus01Sound = SoundManager::getInstance()->registerSound("sounds/Bonus01.wav");
        bonus02Sound = SoundManager::getInstance()->registerSound("sounds/Bonus02.wav");
        bonus03Sound = SoundManager::getInstance()->registerSound("sounds/Bonus03.wav");
        bonus04Sound = SoundManager::getInstance()->registerSound("sounds/Bonus04.wav");
        accent01Sound = SoundManager::getInstance()->registerSound("sounds/Accent01.wav");
        accent02Sound = SoundManager::getInstance()->registerSound("sounds/Accent02.wav");
        accent03Sound = SoundManager::getInstance()->registerSound("sounds/Accent03.wav");
        SoundManager::getInstance()->loadResources();
        // Create score points text.
        scoreText = addRasterFont("textures/Font.png", 64, 64, Vector2(halfWidth, halfHeight - 150), Justification::MIDDLE, TextAnimation::SCALE);
        scoresPerSwapText = addRasterFont("textures/Font.png", 64, 64, Vector2(halfWidth, halfHeight), Justification::MIDDLE, TextAnimation::ZOOM);
        scoresPerSwapText->scale = Vector2(1.2f, 1.2f);
        changedScoreText = addRasterFont("textures/WhiteFont.png", 64, 64, Vector2(halfWidth + 90, halfHeight - 158), Justification::RIGHT, TextAnimation::SLIDE);
        changedScoreText->scale = Vector2(0.7f, 0.7f);
        scores = configData->ScorePoints;
        scoresPerSwap = 0;
        updateScore(scores);
        // Create fruits.
        dyingFruits = 0;
        swapedFruits = 0;
        droppedFruits = 0;        
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            addFruit(x, y, configData->fruitsType[x][y]);
        }
        SoundManager::getInstance()->playSound(fruitsStartSound);
        printBoard();
        matchStep = 0;
        stepsComplited = false;
        missStep = configData->missStep;
        lostScores = configData->lostScores;
        particleSystem = new ParticleSystem();
        lastGoodTime = lastBadTime = lastBonusTime = TimeManager::getInstance()->getTime();
        // First test for match.
        testForMatch();        
        created = true;
        return STATUS_OK;
    };
    // Repeat leaf animation.
    void onLeafTweenComplete(Tweenable* leaf) {
        TweenManager::getInstance()->addTween((Sprite*)leaf, TweenType::ROTATION_CW, 1.5f, Ease::Sinusoidal::InOut)
            ->target(frand(4.0f) - 2.0f)->remove(true)
            ->onComplete(std::bind(&Gameplay::onLeafTweenComplete, this, std::placeholders::_1))
            ->start();
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
        if (fruitType == -1) fruitType = (int)frand(FRUITS_COUNT);
        Fruit* fruit = new Fruit(fruitType);
        const char* fruitTextures[12] = {
            "textures/AppleFruit.png",
            "textures/BannanaFruit.png",
            "textures/CarrotFruit.png",
            "textures/GrapesFruit.png",
            "textures/OrangeFruit.png",
            "textures/PearFruit.png",
            "textures/TomatoFruit.png",
            "textures/RedishFruit.png",
            "textures/LemonFruit.png",
            "textures/ChilliFruit.png",
            "textures/CherryFruit.png"            
        };
        fruit->sprite = spriteBatch->registerSprite(fruitTextures[fruitType], 64, 64);
        fruit->sprite->location = getSkrewedLocation(x, -1);
        fruit->sprite->scale = Vector2(0.9f, 0.9f);
        fruit->sprite->opaque = 0.0f;
        TweenManager::getInstance()->addTween(fruit->sprite, TweenType::OPAQUE, 0.1f, Ease::Sinusoidal::InOut)->target(1.0f)->remove(true)->start();
        fruit->setClickFunction(std::bind(&Gameplay::onFruitClick, this, std::placeholders::_1, std::placeholders::_2));
        fruit->setMoveFunction(std::bind(&Gameplay::onFruitMove, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        fruit->setMovedFunction(std::bind(&Gameplay::onFruitMoved, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        fruit->setKillFunction(std::bind(&Gameplay::onFruitKill, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        fruit->setDyingFunction(std::bind(&Gameplay::onFruitDying, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        fruit->setDeadFunction(std::bind(&Gameplay::onFruitDead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        fruit->lastAccentTime = TimeManager::getInstance()->getTime();
        if (fruitType < FRUITS_COUNT) fruit->moveTo(x, y, 0.0f, FruitMoveType::DROP);
        else fruit->moveTo(x, y, 0.0f, FruitMoveType::DROP_EXTRA);
        fruits[x][y] = fruit;
    };
    // Update scene.
    void update() {
        Scene::update();
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            if (fruits[x][y] != NULL) fruits[x][y]->update();
            if (created && fruits[x][y]->type >= FRUITS_COUNT) {
                if (TimeManager::getInstance()->getTime() > fruits[x][y]->lastAccentTime + 5.0f) {
                    if ((int)frand(2) == 0  && swapedFruits == 0 && droppedFruits == 0) {
                        Vector2 location = getSkrewedLocation(x, y);
                        Tween* t1= TweenManager::getInstance()->addTween(fruits[x][y]->sprite, TweenType::SCALE_XY, 0.37f, Ease::Sinusoidal::InOut)
                            ->remove(true)->target(1.2f, 1.2f);
                        Tween* t2= TweenManager::getInstance()->addTween(fruits[x][y]->sprite, TweenType::SCALE_XY, 0.37f, Ease::Sinusoidal::InOut)
                            ->remove(true)->target(1.0f, 1.0f);
                        t1->addChain(t2)->start(0.1f);
                        if ((int)frand(0) == 0) {
                            switch ((int)frand(3)) {
                                case 0: SoundManager::getInstance()->playSound(accent01Sound); break;
                                case 1: SoundManager::getInstance()->playSound(accent02Sound); break;
                                case 2: SoundManager::getInstance()->playSound(accent03Sound); break;
                            }
                        }
                        addAnimation("textures/AccentForBonus.png", 78, 78, Vector2(location.x, location.y + 2.5f), 18, 1.0f, 0.5f);
                    }
                    fruits[x][y]->lastAccentTime = TimeManager::getInstance()->getTime();
                }
            }
        }
        // If player stupid...
        if (TimeManager::getInstance()->getTime() > lastGoodTime + 15.0f) {
            if (TimeManager::getInstance()->getTime() > lastBadTime + 1.0f && scores > 0) {
                lastBadTime = TimeManager::getInstance()->getTime();
                SoundManager::getInstance()->playSound(scoreSound);
                updateScore(lostScores);
                lostScores--;
            }
        }
        particleSystem->update();
        // Add bonus fruit to empty place.
        if (matchStep == 0 && swapedFruits == 0 && droppedFruits == 0) {
            float renderWidth = (float)GraphicsManager::getInstance()->getRenderWidth();
            float renderHeight = (float)GraphicsManager::getInstance()->getRenderHeight();
            float halfWidth = renderWidth / 2;
            float halfHeight = renderHeight / 2;
            Vector2 location = Vector2(halfWidth, halfHeight);
            if (stepsComplited) {
                if (scoresPerSwap >= MIN_MATCH_UNBELIEVABLE_COUNT) {
                    addSuperText("textures/Unbelievable.png", 360, 80, location, 15);
                    switch ((int)frand(3)) {
                        case 0: SoundManager::getInstance()->playSound(unbelievable01Sound); break;
                        case 1: SoundManager::getInstance()->playSound(unbelievable02Sound); break;
                        case 2: SoundManager::getInstance()->playSound(unbelievable03Sound); break;                    
                    }
                    std::string str = std::to_string(scoresPerSwap);
                    scoresPerSwapText->location = Vector2(halfWidth + 80, halfHeight - 37);
                    scoresPerSwapText->setText(str.c_str());                    
                } else if (scoresPerSwap >= MIN_MATCH_EXELENT_COUNT) {
                    addSuperText("textures/Excellent.png", 360, 80, location, 10);
                    switch ((int)frand(3)) {
                        case 0: SoundManager::getInstance()->playSound(excellent01Sound); break;
                        case 1: SoundManager::getInstance()->playSound(excellent02Sound); break;
                        case 2: SoundManager::getInstance()->playSound(excellent03Sound); break;                    
                    }
                    std::string str = std::to_string(scoresPerSwap);
                    scoresPerSwapText->location = Vector2(halfWidth + 55, halfHeight - 34);
                    scoresPerSwapText->setText(str.c_str());                    
                } else if (scoresPerSwap >= MIN_MATCH_WONDERFUL_COUNT) {
                    addSuperText("textures/Wonderful.png", 360, 80, location, 12);
                    switch ((int)frand(3)) {
                        case 0: SoundManager::getInstance()->playSound(wonderful01Sound); break;
                        case 1: SoundManager::getInstance()->playSound(wonderful02Sound); break;
                        case 2: SoundManager::getInstance()->playSound(wonderful03Sound); break;                    
                    }
                    std::string str = std::to_string(scoresPerSwap);
                    scoresPerSwapText->location = Vector2(halfWidth + 46, halfHeight - 32);
                    scoresPerSwapText->setText(str.c_str());                    
                } else if (scoresPerSwap >= MIN_MATCH_FINE_COUNT) {
                    addSuperText("textures/Fine.png", 360, 80, location, 5);
                    SoundManager::getInstance()->playSound(fineSound);
                    std::string str = std::to_string(scoresPerSwap);
                    scoresPerSwapText->location = Vector2(halfWidth + 39, halfHeight - 30);
                    scoresPerSwapText->setText(str.c_str());
                }
                scoresPerSwap = 0;
                // Add bonus fruit (shance 5).
                if ((int)frand(0) == 0) {
                    int n = 0;
                    bool goodPlace = false;
                    while (!goodPlace || n == GRID_SIZE * GRID_SIZE) {
                        n++;
                        int X = (int)frand(GRID_SIZE);
                        int Y = (int)frand(GRID_SIZE);
                        if (fruits[X][Y]->type < FRUITS_COUNT && !fruits[X][Y]->dropped && fruits[X][Y]->alive) {
                            LOG_DEBUG("Bonus!!!");
                            Vector2 location = getSkrewedLocation(X, Y);
                            location.y = location.y + 40;                            
                            addAnimation("textures/BonusFruitCreate.png", 64, 96, location, 26, 1.2f, 0.5f);
                            LOG_DEBUG("Kill %d %d for bonus.", X, Y);
                            fruits[X][Y]->kill(0.0f, FruitKillType::REPLACE);
                            goodPlace = true;
                        }
                    }
                }
                stepsComplited = false;
            }
        }
    };
    // Move fruits to index position. Delete dead fruits. Create new.
    void updateBoard() {
        LOG_DEBUG("Update board ...");
        printBoard(false);
        int deaded = 0;
        for (int y = 0; y < GRID_SIZE; y++)
        for (int x = 0; x < GRID_SIZE; x++) {
            fruits[x][y]->selected = false;
            if (fruits[x][y]->alive) {
                if (fruits[x][y]->dropped) {
                    fruits[x][y]->moveTo(x, y, 0.0f, FruitMoveType::DROP);
                }
            } else {
                deaded++;
                spriteBatch->unregisterSprite(fruits[x][y]->sprite);
                SAFE_DELETE(fruits[x][y]);
                addFruit(x, y, -1);
            }
        }
        switch ((int)frand(3)) {
            case 0: SoundManager::getInstance()->playSound(fall01Sound); break;
            case 1: SoundManager::getInstance()->playSound(fall02Sound); break;
            case 2: SoundManager::getInstance()->playSound(fall03Sound); break;                    
        }
        updateScore(deaded);
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
                        if (fruits[p][y]->type < FRUITS_COUNT) fruits[p][y]->kill(0.15f, FruitKillType::DEAD);
                        else fruits[p][y]->kill(0.15f, FruitKillType::DEAD_EXTRA);
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
                        if (fruits[x][p]->type < FRUITS_COUNT) fruits[x][p]->kill(0.15f, FruitKillType::DEAD);
                        else fruits[x][p]->kill(0.15f, FruitKillType::DEAD_EXTRA);
                        result++;
                    }
                }
            }
        }
        LOG_DEBUG("Test result = %d", result);
        return result;
    };
    // Result check.
    void resultCheck() {
        LOG_DEBUG("Result check.");
        testForMatch();
        if (dyingFruits >= MIN_MATCH_COUNT) {
            matchStep++;
            if (matchStep > 1) showBonus(matchStep);
        } else matchStep = 0;
    }
    // Select fruit.
    void onFruitClick(int X, int Y) {
        LOG_DEBUG("Select %d %d, type=%d", X, Y, fruits[X][Y]->type);
        if (fruits[X][Y]->selected) {
            fruits[X][Y]->selected = false;
            SoundManager::getInstance()->playSound(clickSound);            
            return;
        }
        if (!fruits[X][Y]->alive) return;
        if (dyingFruits > 0) return;
        if (swapedFruits > 0) return;
        if (droppedFruits > 0) return;
        bool swaped = false;
        if (fruits[X][Y]->type >= FRUITS_COUNT) {
            matchStep = 1;
            missStep = 0;
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
            case FruitMoveType::SWAP_BACK:
            case FruitMoveType::SWAP:
                swapedFruits++;
                break;
            case FruitMoveType::DROP:
            case FruitMoveType::DROP_EXTRA: {
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
                    if (dyingFruits < MIN_MATCH_COUNT) {
                        missStep++;
                        // Come back miss fruits if scores low.
                        if (scores - 2 * missStep <= 0) {
                            Vector2 prevIndex = fruits[x][y]->prevIndex;
                            fruits[x][y]->moveTo((int)prevIndex.x, (int)prevIndex.y, 0.0f, FruitMoveType::SWAP_BACK);
                            fruits[(int)prevIndex.x][(int)prevIndex.y]->moveTo(x, y, 0.0f, FruitMoveType::SWAP_BACK);
                            std::swap(fruits[x][y], fruits[(int)prevIndex.x][(int)prevIndex.y]);
                            scoreText->reset();
                            SoundManager::getInstance()->playSound(noMoveSound);
                        }
                        updateScore(-2 * missStep);
                    } else missStep = 0;
                }
                break;
            }
            case FruitMoveType::SWAP_BACK: {
                swapedFruits--;
                if (swapedFruits == 0) {
                    //
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
                }
                break;
            }
            case FruitMoveType::DROP_EXTRA: {
                droppedFruits--;
                resultCheck();
            }
        }
    };
    // Fruit callbacks.
    void onFruitKill(int x, int y, FruitKillType killType, float delay = 0.0f) {
        // reset time limit
        lastGoodTime = lastBadTime = TimeManager::getInstance()->getTime();        
        switch (killType) {
            case FruitKillType::DEAD: {
                dyingFruits++;                
                break;                
            }
            case FruitKillType::DEAD_EXTRA: {
                dyingFruits++;
                Vector2 location = getSkrewedLocation(x, y);
                Background* a = addAnimation("textures/BonusFruitKill.png", 128, 128, location, 17, 0.7f, delay);
                a->sprite->angle = Deg(frand(360.0f));                
                break;
            }
        }
    };
    void onFruitDying(int x, int y, FruitKillType killType) {
        switch(killType) {
            case FruitKillType::DEAD_EXTRA: {
                for (int Y = 0; Y < GRID_SIZE; Y++) for (int X = 0; X < GRID_SIZE; X++) fruits[X][Y]->selected = false;
                // Bonus fruit dead type.
                switch (fruits[x][y]->type) {
                    case 7: { // reddish: '+' kill
                        LOG_DEBUG("Kill reddish!!!");
                        int p;
                        float delay;
                        for (p = y + 1, delay = 0.0f; p < GRID_SIZE; p++, delay += 0.25f)
                            if (fruits[x][p]->alive) fruits[x][p]->kill(delay, FruitKillType::DEAD_EXTRA);
                        for (p = y - 1, delay = 0.0f; p >= 0; p--, delay += 0.25f)
                            if (fruits[x][p]->alive) fruits[x][p]->kill(delay, FruitKillType::DEAD_EXTRA);
                        for (p = x + 1, delay = 0.0f; p < GRID_SIZE; p++, delay += 0.25f)
                            if (fruits[p][y]->alive) fruits[p][y]->kill(delay, FruitKillType::DEAD_EXTRA);
                        for (p = x - 1, delay = 0.0f; p >= 0; p--, delay += 0.25f)
                            if (fruits[p][y]->alive) fruits[p][y]->kill(delay, FruitKillType::DEAD_EXTRA);
                        break;
                    }
                    case 8: { // lemon: 'x' kill
                        LOG_DEBUG("Kill lemon!!!");
                        int p, q;
                        float delay;
                        for (p = x + 1, q = y + 1, delay = 0.0f; p < GRID_SIZE && q < GRID_SIZE; p++, q++, delay += 0.25f) 
                            if (fruits[p][q]->alive) fruits[p][q]->kill(delay, FruitKillType::DEAD_EXTRA);
                        for (p = x - 1, q = y - 1, delay = 0.0f; p >= 0 && q >= 0; p--, q--, delay += 0.25f) 
                            if (fruits[p][q]->alive) fruits[p][q]->kill(delay, FruitKillType::DEAD_EXTRA);
                        for (p = x + 1, q = y - 1, delay = 0.0f; p < GRID_SIZE && q >= 0; p++, q--, delay += 0.25f) 
                            if (fruits[p][q]->alive) fruits[p][q]->kill(delay, FruitKillType::DEAD_EXTRA);
                        for (p = x - 1, q = y + 1, delay = 0.0f; p >= 0 && q < GRID_SIZE; p--, q++, delay += 0.25f) 
                            if (fruits[p][q]->alive) fruits[p][q]->kill(delay, FruitKillType::DEAD_EXTRA);
                        break;
                    }
                    case 9: { // chili: big bomb (3 x 3) kill by spiral
                        LOG_DEBUG("Kill chili!!!");
                        int d[12][2] = {
                            {-1,  0}, {-1, -1}, { 0, -1}, { 1, -1}, { 1,  0}, { 1,  1}, { 0,  1}, {-1,  1},
                            {-2,  0}, { 0, -2}, { 2,  0}, { 0,  2}
                        };
                        float delay = 0.0f;
                        for (int n = 0; n < 12; n++) {
                            int p = x + d[n][0];
                            int q = y + d[n][1];
                            if (p >= GRID_SIZE || q >= GRID_SIZE || p < 0 || q < 0) continue;
                            if (fruits[p][q]->alive) fruits[p][q]->kill(delay, FruitKillType::DEAD_EXTRA);
                            delay += 0.25f;
                        }
                        break;
                    }
                    case 10: { // cherry: kill all fruits type with whom swapped
                        LOG_DEBUG("Kill cherry!!!");                        
                        /// not empleementted yet.
                        break;
                    }
                }
            }
            case FruitKillType::DEAD: {
                switch ((int)frand(3)) {
                    case 0: SoundManager::getInstance()->playSound(match01Sound); break;
                    case 1: SoundManager::getInstance()->playSound(match02Sound); break;
                    case 2: SoundManager::getInstance()->playSound(match03Sound); break;
                }
                // Emit star particles.
                if (SLOW_DOWN == 1) particleSystem->emit(3, getSkrewedLocation(x, y));
                break;
            }
        }
    };
    void onFruitDead(int x, int y, FruitKillType killType) {
        lostScores = -1;
        switch (killType) {
            case FruitKillType::DEAD:
            case FruitKillType::DEAD_EXTRA: {
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
                    stepsComplited = true;
                }
                break;
            }
            case FruitKillType::REPLACE: {
                spriteBatch->unregisterSprite(fruits[x][y]->sprite);
                SAFE_DELETE(fruits[x][y]);
                /// -----------------------------------
                addFruit(x, y, (int)frand(3) + 7);
                /// -----------------------------------
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
                    case 0: SoundManager::getInstance()->playSound(wow01Sound); break;
                    case 1: SoundManager::getInstance()->playSound(wow02Sound); break;
                    case 2: SoundManager::getInstance()->playSound(wow03Sound); break;
                }
                break;
            }
        }
    }    
    // Scores.
    void updateScore(int value) {
        LOG_DEBUG("updateScore value=%d", value);
        if (scores < 0) return;
        if (matchStep > 1) value *= matchStep;
        if (scores + value < 0) value = -scores;
        scores += value;        
        if (created) {
            if (value > 0) scoresPerSwap += value;
            std::string changedStr = (value > 0) ? "+" + std::to_string(value) : std::to_string(value);
            if (value != 0) {
                changedScoreText->setText(changedStr.c_str());
            }
        } else scores = value;
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
        configData->lostScores = lostScores;
        configData->firstSrtart = false;
    };
private:
    Activity* activity;
    // Decorations.
    Background* background;
    Background* gameBox;
    // Leafs.
    Background* leaf01;
    Background* leaf02;
    Background* leaf03;
    Background* leaf04;
    Background* leaf05;
    Background* leaf06;
    // Bonus text.
    Background* bonus;
    int matchStep;
    int missStep;
    int lostScores;
    // Fruits.
    Fruit* fruits[GRID_SIZE][GRID_SIZE];
    bool stepsComplited;
    // Fruit Animations.
    int dyingFruits;
    int swapedFruits;
    int droppedFruits;
    // Score points.
    RasterFont* scoreText;
    RasterFont* changedScoreText;
    RasterFont* scoresPerSwapText;
    int scores;
    int scoresPerSwap;
    // Particle system.
    ParticleSystem* particleSystem;
    // Time stamps.
    float lastGoodTime;
    float lastBadTime;
    float lastBonusTime;
    // Sounds.
    Sound* clickSound;
    Sound* doubleSound;
    Sound* fall01Sound;
    Sound* fall02Sound;
    Sound* fall03Sound;
    Sound* fruitsStartSound;
    Sound* match01Sound;
    Sound* match02Sound;
    Sound* match03Sound;
    Sound* moveSound;
    Sound* noMoveSound;
    Sound* scoreSound;
    Sound* excellent01Sound;
    Sound* excellent02Sound;
    Sound* excellent03Sound;
    Sound* fineSound;
    Sound* unbelievable01Sound;
    Sound* unbelievable02Sound;
    Sound* unbelievable03Sound;
    Sound* wonderful01Sound;
    Sound* wonderful02Sound;
    Sound* wonderful03Sound;
    Sound* wow01Sound;
    Sound* wow02Sound;
    Sound* wow03Sound;
    Sound* bonus01Sound;
    Sound* bonus02Sound;
    Sound* bonus03Sound;
    Sound* bonus04Sound;
    Sound* accent01Sound;
    Sound* accent02Sound;
    Sound* accent03Sound;
};

#endif // __GAMEPLAY_H__
