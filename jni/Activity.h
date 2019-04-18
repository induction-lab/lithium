#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__

#include "Scene.h"

class Activity: public ActivityHandler {
private:
    bool sceneChanged;      // scene must be changed
    Scene *scene;           // scene to be used
    Scene *nextScene;
    // Change current scene.
    status setScene(Scene *scn) {
        if (scene != NULL) {
            TimeManager::getInstance()->reset();
            GraphicsManager::getInstance()->reset();
            //SoundManager::getInstance()->reset();
            TweenManager::getInstance()->reset();
            SAFE_DELETE(scene);            
        }
        scene = scn;
        if (scene->start() != STATUS_OK) return STATUS_ERROR;
        return STATUS_OK;
    };
public:
    // Status flages.
    bool quit;              // exit requested
    bool firstStart;        // indicates if application is launched for the first time.
    bool paused;            // activity paused state
    Activity():
        quit(false),
        firstStart(true),
        scene(NULL),
        sceneChanged(false) {
        LOG_DEBUG("Creating Activity.");
    };
    ~Activity() {
        LOG_DEBUG("Destructing Activity.");
        SAFE_DELETE(scene);
    };
    void changeScene(Scene *scn) {
        sceneChanged = true;
        nextScene = scn;
    };
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
    };
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
    };
    void onDeactivate() {
        LOG_INFO("Dectivating Engine.");
    };
    void onDestroy() {
        if (scene != NULL) SAFE_DELETE(scene);
    };
};

#include "scenes/Gameplay.h"
#include "scenes/MainMenu.h"

status Activity::setStartScene() {
    return setScene(new MainMenu(this));
}

#endif // __ACTIVITY_H__
