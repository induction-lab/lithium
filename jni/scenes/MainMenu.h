#ifndef __MAINMENU_H__
#define __MAINMENU_H__

class MainMenu : public Scene {
private:
    Activity* activity;
public:
    MainMenu(Activity* activity):
        Scene(),
        activity(activity) {
        LOG_INFO("Create MainMenu scene.");
    };
    ~MainMenu() {
        LOG_INFO("Destroy MainMenu scene.");
    };
    status start() {
        if (created) return STATUS_OK;
        LOG_INFO("Start MainMenu scene.");
        spriteBatch = new SpriteBatch();
        float renderWidth = (float)GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float)GraphicsManager::getInstance()->getRenderHeight();
        float halfWidth = renderWidth / 2;
        float halfHeight = renderHeight / 2;
        background = addBackground("textures/Background.png", 360, 640, Vector2(halfWidth, halfHeight));
        gameBox = addBackground("textures/GameBox.png", 360, 380, Vector2(halfWidth, halfHeight));
        gameBox->setClickFunction(std::bind(&MainMenu::onGameBoxClick, this));
        gameBox->sprite->opaque = 0.0f;
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::OPAQUE, 0.7f, Ease::Sinusoidal::InOut)
            ->target(1.0f)->remove(true)->start();
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_X, 0.37f, Ease::Sinusoidal::InOut)
            ->target(1.03f)->remove(false)->loop()->reverse()->start();
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_Y, 0.37f, Ease::Sinusoidal::InOut)
            ->target(1.03f)->remove(false)->loop()->reverse()->start(0.5f);
        // First start.
        if (configData->firstSrtart) {
            // induction logo.
            Background* boom = addBackground("textures/StartScreen.png", 360, 640, Vector2(halfWidth, halfHeight));
            boom->sprite->order = 10;
            TweenManager::getInstance()->addTween(boom->sprite, TweenType::FRAME, 2.3f, Ease::Linear)
                ->target(43.0f)->remove(true)->start(6.0f);
            Background* induction = addBackground("textures/induction.png", 239, 142, Vector2(halfWidth, halfHeight));
            induction->sprite->order = 10;
            Tween* t1 = TweenManager::getInstance()->addTween(induction->sprite, TweenType::FRAME, 1.25f, Ease::Linear)
                ->target(30.0f)->remove(true);
            Tween* t2 = TweenManager::getInstance()->addTween(induction->sprite, TweenType::OPAQUE, 0.5f, Ease::Linear)
                ->target(0.0f)->remove(true)->delay(3.7f);
            t1->addChain(t2);
            t1->start(0.7f);
            configData->firstSrtart = false;
            // Game logo.
            Background* gameLogo = addBackground("textures/GameLogo.png", 280, 150, Vector2(halfWidth, halfHeight + 55.0f));
            Tween* t3 = TweenManager::getInstance()->addTween(gameLogo->sprite, TweenType::FRAME, 2.3f, Ease::Linear)
                ->target(26.0f)->remove(true);
            Tween* t4 = TweenManager::getInstance()->addTween(gameLogo->sprite, TweenType::FRAME, 0.78f, Ease::Linear)
                ->target(33.0f)->remove(false)->loop();
            t3->addChain(t4);
            t3->start(7.0f);
        } else {
            Background* gameLogo = addBackground("textures/GameLogo.png", 280, 150, Vector2(halfWidth, halfHeight + 55.0f));
            gameLogo->sprite->setFrame(26);
            TweenManager::getInstance()->addTween(gameLogo->sprite, TweenType::FRAME, 0.78f, Ease::Linear)
                ->target(33.0f)->remove(false)->loop()->start();
        }
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
        exitButton = addButton("textures/ExitButton.png", 80, 78, Vector2(halfWidth - 85, halfHeight - 80));
        exitButton->setDownFunction(std::bind(&MainMenu::onAnyButtonDown, this));
        exitButton->setUpFunction(std::bind(&MainMenu::onAnyButtonUp, this));
        exitButton->setClickFunction(std::bind(&MainMenu::onExitButtonClick, this));
        TweenManager::getInstance()->addTween(exitButton->sprite, TweenType::POSITION_Y, 0.37f, Ease::Sinusoidal::InOut)
            ->target(halfHeight - 70)->remove(false)->loop()->reverse()->start(0.7f);
        playButton = addButton("textures/PlayButton.png", 104, 100, Vector2(halfWidth, halfHeight - 100));
        playButton->setDownFunction(std::bind(&MainMenu::onAnyButtonDown, this));
        playButton->setUpFunction(std::bind(&MainMenu::onAnyButtonUp, this));
        playButton->setClickFunction(std::bind(&MainMenu::onPlayButtonClick, this));
        TweenManager::getInstance()->addTween(playButton->sprite, TweenType::POSITION_Y, 0.37f, Ease::Sinusoidal::InOut)
            ->target(halfHeight - 90)->remove(false)->loop()->reverse()->start(0.9f);
        soundSettingsButton = addButton("textures/SoundSettingButton.png", 80, 78, Vector2(halfWidth + 85, halfHeight - 80));
        soundSettingsButton->setDownFunction(std::bind(&MainMenu::onAnyButtonDown, this));
        soundSettingsButton->setUpFunction(std::bind(&MainMenu::onAnyButtonUp, this));
        soundSettingsButton->setClickFunction(std::bind(&MainMenu::onSoundsButtonClick, this));
        TweenManager::getInstance()->addTween(soundSettingsButton->sprite, TweenType::POSITION_Y, 0.37f, Ease::Sinusoidal::InOut)
            ->target(halfHeight - 70)->remove(false)->loop()->reverse()->start(0.5f);
        buttonDownSound = SoundManager::getInstance()->registerSound("sounds/ButtonDown.wav");
        buttonUpSound = SoundManager::getInstance()->registerSound("sounds/ButtonUp.wav");
        SoundManager::getInstance()->loadResources();
        created = true;
        return STATUS_OK;
    };
    void update() {
        Scene::update();
    };
    // Repeat leaf animation.
    void onLeafTweenComplete(Tweenable* leaf) {
        TweenManager::getInstance()->addTween((Sprite*)leaf, TweenType::ROTATION_CW, 1.5f, Ease::Sinusoidal::InOut)
            ->target(frand(4.0f) - 2.0f)->remove(true)
            ->onComplete(std::bind(&MainMenu::onLeafTweenComplete, this, std::placeholders::_1))
            ->start();
    };
    void onAnyButtonDown() {
        SoundManager::getInstance()->playSound(buttonDownSound);
    };
    void onAnyButtonUp() {
        SoundManager::getInstance()->playSound(buttonUpSound);
    };
    void onGameBoxClick() {
        LOG_DEBUG("GameBox Click");
    };
    void onExitButtonClick() {
        activity->quit = true;
    };
    void onPlayButtonClick() {
        activity->changeScene(new Gameplay(activity));
    };
    void onSoundsButtonClick() {
        activity->changeScene(new SoundSetting(activity));
    };
    int backEvent() {
        activity->quit = true;
        return 1;
    };
    int gestureSwipeEvent(int x, int y, int direction) {
        if (uiModeType != ACONFIGURATION_UI_MODE_TYPE_WATCH) return 0;
        if (direction == SWIPE_DIRECTION_RIGHT) {
            activity->quit = true;
            return 1;
        }
        return 0;
    };
    Background* background;
    Background* gameBox;
    // Leafs.
    Background* leaf01;
    Background* leaf02;
    Background* leaf03;
    Background* leaf04;
    Background* leaf05;
    Background* leaf06;
    // Controls.
    Button* exitButton;
    Button* playButton;
    Button* soundSettingsButton;
    Sound* buttonDownSound;
    Sound* buttonUpSound;
};

#endif // __MAINMENU_H__
