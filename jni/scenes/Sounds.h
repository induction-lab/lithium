#ifndef __SOUNDS_H__
#define __SOUNDS_H__

class Sounds: public Scene {
private:
    Activity* activity;
public:
    Sounds(Activity* activity):
        activity(activity) {
        LOG_INFO("Scene Sounds created.");
    };
    ~Sounds() {
        LOG_INFO("Scene Sounds Destructed.");
    };
    status start() {
        LOG_DEBUG("Start Sounds scene.");
        spriteBatch = new SpriteBatch();
        float renderWidth = (float) GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float) GraphicsManager::getInstance()->getRenderHeight();
        float halfWidth = renderWidth / 2;
        float halfHeight = renderHeight / 2;
        background = addBackground("textures/Background.png", 360, 640, Location(halfWidth, halfHeight));
        gameBox = addBackground("textures/GameBox.png", 360, 380, Location(halfWidth, halfHeight));
        gameBox->sprite->opaque = 0.0f;
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::OPAQUE, 0.7f, Ease::Sinusoidal::InOut)
                    ->target(1.0f)->remove(true)->start();
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_X, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start();
        TweenManager::getInstance()->addTween(gameBox->sprite, TweenType::SCALE_Y, 0.35f, Ease::Sinusoidal::InOut)
                    ->target(1.03f)->remove(false)->loop()->reverse()->start(0.5f);
        test = addBackground("textures/test.png", 268, 247, Location(halfWidth, halfHeight));
        created = true;
        return STATUS_OK;
    };
    void update() {
        Scene::update();
    };
    void onBackEvent() {
        activity->setStartScene();
    };
    void gestureSwipeEvent(int x, int y, int direction) {
        // if (uiModeType != ACONFIGURATION_UI_MODE_TYPE_WATCH) return;
        if (direction == SWIPE_DIRECTION_RIGHT) activity->setStartScene();
    };
    Background* background;
    Background* gameBox;
    Background* test;
};

#endif // __SOUNDS_H__
