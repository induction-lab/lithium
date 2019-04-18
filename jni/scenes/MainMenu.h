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
        gameBox->sprite->opaque = 0.0f;
        Tween* t0 = TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::OPAQUE, 0.7f, Ease::Sinusoidal::InOut)
                    ->target(1.0f)->remove(true)->start();
        Tween* t1 = TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start();
        Tween* t2 = TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start(0.5f);
        exitButton = addButton("textures/ExitButton.png", 80, 78, Vector2(halfWidth - 85, halfHeight - 80));
		exitButton->setDownFunction(std::bind(&MainMenu::onAnyButtonDown, this));
        exitButton->setUpFunction(std::bind(&MainMenu::onAnyButtonUp, this));
        exitButton->setClickFunction(std::bind(&MainMenu::onExitButtonClick, this));
        TweenManager::getInstance()->addTween(exitButton->sprite, TweenType::POSITION_Y, 0.35f, Ease::Sinusoidal::InOut)
            ->target(halfHeight - 70)->remove(false)->loop()->reverse()->start(0.7f);        
        playButton = addButton("textures/PlayButton.png", 104, 100, Vector2(halfWidth, halfHeight - 100));
		playButton->setDownFunction(std::bind(&MainMenu::onAnyButtonDown, this));
        playButton->setUpFunction(std::bind(&MainMenu::onAnyButtonUp, this));
        playButton->setClickFunction(std::bind(&MainMenu::onPlayButtonClick, this));
        TweenManager::getInstance()->addTween(playButton->sprite, TweenType::POSITION_Y, 0.35f, Ease::Sinusoidal::InOut)
            ->target(halfHeight - 90)->remove(false)->loop()->reverse()->start(0.9f);
        soundSettingsButton = addButton("textures/SoundSettingButton.png", 80, 78, Vector2(halfWidth + 85, halfHeight - 80));
		soundSettingsButton->setDownFunction(std::bind(&MainMenu::onAnyButtonDown, this));
        soundSettingsButton->setUpFunction(std::bind(&MainMenu::onAnyButtonUp, this));
        soundSettingsButton->setClickFunction(std::bind(&MainMenu::onSoundsButtonClick, this));
        TweenManager::getInstance()->addTween(soundSettingsButton->sprite, TweenType::POSITION_Y, 0.35f, Ease::Sinusoidal::InOut)
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
	void onAnyButtonDown() {
        SoundManager::getInstance()->playSound(buttonDownSound);
	};
	void onAnyButtonUp() {
        SoundManager::getInstance()->playSound(buttonUpSound);
	};
    void onExitButtonClick() {
        activity->quit = true;
    }
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
    Button* exitButton;
    Button* playButton;
    Button* soundSettingsButton;
    Sound* buttonDownSound;
    Sound* buttonUpSound;
};

#endif // __MAINMENU_H__
