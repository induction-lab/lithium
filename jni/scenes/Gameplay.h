#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "Line.h"

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
        Line* line = new Line(5);
        line->color = Vector(1.0f, 0.5f, 0.2f);
        line->addPoint(Vector(100, 100, 0));
        line->addPoint(Vector(100, 300, 0));
        line->addPoint(Vector(200, 350, 0));
        line->addPoint(Vector(300, 400, 0));
        line->addPoint(Vector( 50, 500, 0));
        line->addPoint(Vector(150, 380, 0));
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
