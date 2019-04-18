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
        sounds = addBackground("textures/SoundPanel.png", 268, 239, Vector2(halfWidth, halfHeight));
        okButton = addButton("textures/OkButton.png", 80, 78, Vector2(halfWidth + 70, halfHeight - 100));
        okButton->setDownFunction(std::bind(&SoundSetting::onAnyButtonDown, this));
        okButton->setUpFunction(std::bind(&SoundSetting::onAnyButtonUp, this));
        okButton->setClickFunction(std::bind(&SoundSetting::onOkButtonClick, this));
        TweenManager::getInstance()->addTween(okButton->sprite, TweenType::POSITION_Y, 0.37f, Ease::Sinusoidal::InOut)
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
        aha01Sound = SoundManager::getInstance()->registerSound("sounds/Aha01.wav");
        aha02Sound = SoundManager::getInstance()->registerSound("sounds/Aha02.wav");
        aha03Sound = SoundManager::getInstance()->registerSound("sounds/Aha03.wav");        
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
            ->onComplete(std::bind(&SoundSetting::onLeafTweenComplete, this, std::placeholders::_1))
            ->start();
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
        if (musicSlider->getChanged()) return;
        switch ((int)frand(3)) {
            case 0: SoundManager::getInstance()->playSound(aha01Sound); break;
            case 1: SoundManager::getInstance()->playSound(aha02Sound); break;
            case 2: SoundManager::getInstance()->playSound(aha03Sound); break;
        }
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
    // Leafs.
    Background* leaf01;
    Background* leaf02;
    Background* leaf03;
    Background* leaf04;
    Background* leaf05;
    Background* leaf06;
    // Controls.
    Background* sounds;
    Button* okButton;
    Slider* soundSlider;
    Slider* musicSlider;
    Sound* buttonDownSound;
    Sound* buttonUpSound;
    Sound* aha01Sound;
    Sound* aha02Sound;
    Sound* aha03Sound;    
};

#endif // __SOUND_SETTING_H__
