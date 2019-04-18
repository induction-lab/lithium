#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

class Gameplay : public Scene {
private:
    Activity* activity;
public:
    Gameplay(Activity* activity):
        activity(activity) {
        LOG_INFO("Scene Gameplay created.");
    };
    ~Gameplay() {
        LOG_INFO("Scene Gameplay Destructed.");
    };
    status start() {
        LOG_DEBUG("Start Gameplay scene.");
        float renderWidth = (float) GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float) GraphicsManager::getInstance()->getRenderHeight();
        SpriteBatch* spriteBatch = new SpriteBatch();
        Sprite* background = spriteBatch->registerSprite("textures/Background.png", 360, 640);
        background->setLocation(renderWidth / 2, renderHeight / 2);
        return STATUS_OK;
    };
    void update() {
        //
    };
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
        Location location = GraphicsManager::getInstance()->screenToRender((float)x, (float)y);
        switch (event) {
        case Touch::TOUCH_RELEASE:
            activity->setStartScene();
            break;
        default:
            break;
        }
    };
    void backEvent() {
        activity->setStartScene();
    }
};

#endif
