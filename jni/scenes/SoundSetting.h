#ifndef __SOUND_SETTING_H__
#define __SOUND_SETTING_H__

class SoundSetting: public Scene {
private:
    Activity* activity;
public:
    SoundSetting(Activity* activity):
        activity(activity) {
        LOG_INFO("Create SoundSetting scene.");
    };
    ~SoundSetting() {
        LOG_INFO("Destroy SoundSetting scene.");
    };
    status start() {
        if (created) return STATUS_OK;
        LOG_DEBUG("Start SoundSetting scene.");
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
        sounds = addBackground("textures/SoundPanel.png", 268, 239, Vector2(halfWidth, halfHeight));
        okButton = addButton("textures/OkButton.png", 80, 78, Vector2(halfWidth + 70, halfHeight - 100));
		okButton->setDownFunction(std::bind(&SoundSetting::onAnyButtonDown, this));
        okButton->setUpFunction(std::bind(&SoundSetting::onAnyButtonUp, this));
        okButton->setClickFunction(std::bind(&SoundSetting::onOkButtonClick, this));
        TweenManager::getInstance()->addTween(okButton->sprite, TweenType::POSITION_Y, 0.35f, Ease::Sinusoidal::InOut)
            ->target(halfHeight - 90)->remove(false)->loop()->reverse()->start(0.2f);
		soundSlider = addSlider("textures/SliderBackground.png", 160, 20, Vector2(halfWidth + 25, halfHeight + 20));
        soundSlider->setSliderHandle("textures/SliderHandle.png", 48, 48, 0);
        soundSlider->setSlideFunction(std::bind(&SoundSetting::onSoundSliderSlide, this, std::placeholders::_1));
        soundSlider->setPosition(configData->soundVolume);
        soundSlider->setUpFunction(std::bind(&SoundSetting::onSoundSliderUp, this));
        musicSlider = addSlider("textures/SliderBackground.png", 160, 20, Vector2(halfWidth + 25, halfHeight - 35));
        musicSlider->setSliderHandle("textures/SliderHandle.png", 48, 48, 0);
        musicSlider->setSlideFunction(std::bind(&SoundSetting::onMusicSliderSlide, this, std::placeholders::_1));
        musicSlider->setPosition(configData->musicVolume);
        buttonDownSound = SoundManager::getInstance()->registerSound("sounds/ButtonDown.wav");
        buttonUpSound = SoundManager::getInstance()->registerSound("sounds/ButtonUp.wav");
        volumeSound = SoundManager::getInstance()->registerSound("sounds/SoundVolume.wav");
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
    void onOkButtonClick() {
        activity->setStartScene();
    };
    void onSoundSliderSlide(int volume) {
        LOG_DEBUG("Set sound volume to %d%%", soundSlider->precent);
        SoundManager::getInstance()->setSoundVolume((float)volume / 100.0f);
        configData->soundVolume = volume;
    };
    void onSoundSliderUp() {
        SoundManager::getInstance()->playSound(volumeSound);
    };
    void onMusicSliderSlide(int volume) {
        LOG_DEBUG("Set music volume to %d%%", musicSlider->precent);
        SoundManager::getInstance()->setMusicVolume((float)volume / 100.0f);
        configData->musicVolume = volume;
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
    Background* sounds;
    Button* okButton;
    Slider* soundSlider;
    Slider* musicSlider;
    Sound* buttonDownSound;
    Sound* buttonUpSound;
    Sound* volumeSound;
};

#endif // __SOUND_SETTING_H__
