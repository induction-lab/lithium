#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__

class Scene {
public:
    virtual ~Scene(void) {};
    virtual status start(void) = 0;
    virtual void update(void) = 0;
	virtual void touchPress(Location location) {}; 	    // when screen is touched
	virtual void touchMove(Location location) {};	    // touch move
	virtual void touchRelease(Location location) {};    // touch end
};

class Activity: public ActivityHandler, public InputListener {
private:
    bool quit;              // exit requested
    bool firstStart;        // indicates if application is launched for the first time.
    bool paused;            // activity paused state
    bool sceneChanged;      // scene must be changed
    Scene *scene;           // scene to be used
    Scene *nextScene;
    // change current scene
    status setScene(Scene *scn) {
        if (scene != NULL) {
            TweenManager::getInstance()->clear();
            GraphicsManager::getInstance()->clear();
            GraphicsManager::getInstance()->reset();
            SAFE_DELETE(scene);            
        }
        scene = scn;
        if (scene->start() != STATUS_OK) return STATUS_ERROR;
        return STATUS_OK;
    }
public:
    Activity():
        quit(false),
        firstStart(true),
        scene(NULL),
        sceneChanged(false) {
        LOG_DEBUG("Creating Activity.");
    }
    ~Activity() {
        LOG_DEBUG("Destructing Activity.");
        SAFE_DELETE(scene);
    }
    void ChangeScene(Scene *scn) {
        sceneChanged = true;
        nextScene = scn;
    }
    void touchEvent(Touch::TouchEvent event, int x, int y, size_t pointerId) {
		Location location = GraphicsManager::getInstance()->screenToRender((float)x, (float)y);
        switch (event) {
		case Touch::TOUCH_PRESS:
			break;
        case Touch::TOUCH_MOVE:
            break;
        case Touch::TOUCH_RELEASE:
			if (scene != NULL) scene->touchRelease(location);
            break;
        }
    }
    status setStartScene();
protected:
    status onActivate() {
        LOG_INFO("Activate Engine.");
        if (firstStart) {
            LOG_DEBUG("First start detected.");
            if (setStartScene() != STATUS_OK) return STATUS_ERROR;
            firstStart = false;
        }
        return STATUS_OK;
    }
    status onStep() {
        if (quit) return STATUS_EXIT;
        // change scene
        if (sceneChanged) {
            sceneChanged = false;
            if (setScene(nextScene) != STATUS_OK) return STATUS_ERROR;
            nextScene = NULL;
        }
        if (scene != NULL && !paused) scene->update();
        return STATUS_OK;
    }
    void onDeactivate() {
        LOG_INFO("Dectivating Engine.");
    }
    void onDestroy() {
        if (scene != NULL) SAFE_DELETE(scene);
    }
};

#include "scenes\Gameplay.h"
#include "scenes\MainMenu.h"

status Activity::setStartScene() {
    return setScene(new MainMenu(this));
}

#endif
