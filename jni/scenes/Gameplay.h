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
        line = new Line(5.0f);
        line->color = Vector(0.2f, 1.0f, 0.4f);
        created = true;
        return STATUS_OK;
    };
    void gestureTapEvent(int x, int y) {
        if (!created) return;
        Location point = GraphicsManager::getInstance()->screenToRender(x, y);
        line->addPoint(Vector(point.x, point.y, 0.0f));
    };
    void update() {
        Scene::update();
    };
    void backEvent() {
        activity->setStartScene();
    };
    void gestureSwipeEvent(int x, int y, int direction) {
        if (uiModeType != ACONFIGURATION_UI_MODE_TYPE_WATCH) return;
        if (direction == SWIPE_DIRECTION_RIGHT) activity->setStartScene();
    };
    Background* background;
    Line* line;
};

#endif
