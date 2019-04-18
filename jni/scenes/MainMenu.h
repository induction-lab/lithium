#ifndef __MAINMENU_H__
#define __MAINMENU_H__

class MainMenu : public Scene {
private:
    Activity* activity;
public:
    MainMenu(Activity* activity):
        activity(activity) {
        LOG_INFO("Scene MainMenu created.");
    };
    ~MainMenu() {
        LOG_INFO("Scene MainMenu Destructed.");
    };
    status start() {
        LOG_INFO("Start MainMenu scene.");
        InputManager::getInstance()->registerListener(this);
        float renderWidth = (float) GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float) GraphicsManager::getInstance()->getRenderHeight();
        SpriteBatch *spriteBatch = new SpriteBatch();
        Sprite* background = spriteBatch->registerSprite("textures/Background.png", 360, 640);
        background->setLocation(renderWidth / 2, renderHeight / 2);
        Sprite* gameBox = spriteBatch->registerSprite("textures/GameBox.png", 360, 380);
        gameBox->setLocation(renderWidth / 2, renderHeight / 2);
        sprite = spriteBatch->registerSprite("textures/Play.png", 104, 100);
        sprite->setLocation(renderWidth / 2, renderHeight / 2 - 60.0f);
        Tween* t0 = TweenManager::getInstance()->addTween(gameBox, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start();
        Tween* t1 = TweenManager::getInstance()->addTween(gameBox, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start(0.5f);
        Tween* t = TweenManager::getInstance()->addTween(sprite, TweenType::OPAQUE, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(0.3f)->remove(false)->loop()->reverse()->start();
		soundPress = SoundManager::getInstance()->registerSound("sounds/SoundPress.wav");
        soundRelease = SoundManager::getInstance()->registerSound("sounds/SoundRelease.wav");
        SoundManager::getInstance()->loadResources();
        SoundManager::getInstance()->playMusic("sounds/Intro.mp3");
        return STATUS_OK;
    };
    void update() {
        //
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        Location location = GraphicsManager::getInstance()->screenToRender((float)x, (float)y);
        switch (event) {
        case Touch::TOUCH_PRESS:
            if (sprite->pointInSprite(location)) {
                SoundManager::getInstance()->playSound(soundPress);
            }        
            break;
        case Touch::TOUCH_RELEASE:
            if (sprite->pointInSprite(location)) {
                SoundManager::getInstance()->playSound(soundRelease);
                InputManager::getInstance()->unregisterListener(this);
                activity->changeScene(new Gameplay(activity));
            }
            break;
        default:
            break;
        }
    };
    void backEvent() {
        activity->quit = true;
    };
    Sprite* sprite;
    Sound* soundPress;
    Sound* soundRelease;
};

#endif // __MAINMENU_H__
