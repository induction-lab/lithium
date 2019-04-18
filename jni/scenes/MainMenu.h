#ifndef __MAINMENU_H__
#define __MAINMENU_H__

class MainMenu : public Scene {
private:
    Activity* activity;
public:
    MainMenu(Activity* activity):
        Scene(),
        activity(activity) {
        LOG_INFO("Scene MainMenu created.");
    };
    ~MainMenu() {
        LOG_INFO("Scene MainMenu Destructed.");
    };
    status start() {
        LOG_INFO("Start MainMenu scene.");
        spriteBatch = new SpriteBatch();
        float renderWidth = (float) GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float) GraphicsManager::getInstance()->getRenderHeight();
        float halfWidth = renderWidth / 2;
        float halfHeight = renderHeight / 2;
        background = addBackground("textures/Background.png", 360, 640, Location(halfWidth, halfHeight));
        gameBox = addBackground("textures/GameBox.png", 360, 380, Location(halfWidth, halfHeight));
        gameBox->sprite->opaque = 0.0f;
        Tween* t0 = TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::OPAQUE, 0.7f, Ease::Sinusoidal::InOut)
                    ->target(1.0f)->remove(true)->start();
        Tween* t1 = TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start();
        Tween* t2 = TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start(0.5f);
        exitButton = addButton("textures/Exit.png", 80, 78, Location(halfWidth - 85, halfHeight - 80));
		exitButton->setDownFunction(std::bind(&MainMenu::onAnyButtonDown, this));
        exitButton->setUpFunction(std::bind(&MainMenu::onAnyButtonUp, this));
        exitButton->setClickFunction(std::bind(&MainMenu::onExitButtonClick, this));
        TweenManager::getInstance()->addTween(exitButton->sprite, TweenType::POSITION_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(halfHeight - 70)->remove(false)->loop()->reverse()->start(0.7f);        
        playButton = addButton("textures/Play.png", 104, 100, Location(halfWidth, halfHeight - 100));
		playButton->setDownFunction(std::bind(&MainMenu::onAnyButtonDown, this));
        playButton->setUpFunction(std::bind(&MainMenu::onAnyButtonUp, this));
        playButton->setClickFunction(std::bind(&MainMenu::onPlayButtonClick, this));
        TweenManager::getInstance()->addTween(playButton->sprite, TweenType::POSITION_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(halfHeight - 90)->remove(false)->loop()->reverse()->start(0.9f);
        muteCheckBox = addCheckBox("textures/Mute.png", 80, 78, Location(halfWidth + 85, halfHeight - 80));
		muteCheckBox->setDownFunction(std::bind(&MainMenu::onAnyButtonDown, this));
        muteCheckBox->setUpFunction(std::bind(&MainMenu::onAnyButtonUp, this));
        muteCheckBox->setClickFunction(std::bind(&MainMenu::onMuteCheckBoxClick, this));
        TweenManager::getInstance()->addTween(muteCheckBox->sprite, TweenType::POSITION_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(halfHeight - 70)->remove(false)->loop()->reverse()->start(0.5);
        muteCheckBox->setChecked(configData->musicOn);
		soundButtonDown = SoundManager::getInstance()->registerSound("sounds/ButtonDown.wav");
        soundButtonUp = SoundManager::getInstance()->registerSound("sounds/ButtonUp.wav");
        SoundManager::getInstance()->loadResources();
        return STATUS_OK;
    };
    void update() {
        Scene::update();
    };
    void pause() {
        //
    };
    void resume() {
        //
    };
	void onAnyButtonDown() {
        SoundManager::getInstance()->playSound(soundButtonDown);
	};
	void onAnyButtonUp() {
        SoundManager::getInstance()->playSound(soundButtonUp);
	};
    void onExitButtonClick() {
        activity->quit = true;
    }
	void onPlayButtonClick() {
        activity->changeScene(new Gameplay(activity));
	};
	void onMuteCheckBoxClick() {
        if (muteCheckBox->getChecked()) {
            SoundManager::getInstance()->playMusic("sounds/Intro.mp3");
            configData->musicOn = true;
        } else {
            SoundManager::getInstance()->stopMusic();
            configData->musicOn = false;
        }
	};
    void backEvent() {
        activity->quit = true;
    };
    void gestureSwipeEvent(int x, int y, int direction) {
        if (uiModeType != ACONFIGURATION_UI_MODE_TYPE_WATCH) return;
        if (direction == SWIPE_DIRECTION_RIGHT) activity->quit = true;
    };    
    Background* background;
    Background* gameBox;
    Button* exitButton;
    Button* playButton;
    CheckBox* muteCheckBox;
    Sound* soundButtonDown;
    Sound* soundButtonUp;
};

#endif // __MAINMENU_H__
