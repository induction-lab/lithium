#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

class Gameplay: public Scene {
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
        spriteBatch = new SpriteBatch();
        float renderWidth = (float) GraphicsManager::getInstance()->getRenderWidth();
        float renderHeight = (float) GraphicsManager::getInstance()->getRenderHeight();
        float halfWidth = renderWidth / 2;
        float halfHeight = renderHeight / 2;
        background = addBackground("textures/Background_temp.png", 360, 640, Location(halfWidth, halfHeight));
        return STATUS_OK;
    };
    void update() {
        Scene::update();
    };
    void backEvent() {
        activity->setStartScene();
    };
    Background* background;
};

#endif
