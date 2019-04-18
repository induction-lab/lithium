#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__

#include "Scene.h"

class Activity: public ActivityHandler {
private:    
    bool quit;              // exit requested
    bool firstStart;        // indicates if application is launched for the first time.
    bool paused;            // activity paused state    
    bool sceneChanged;      // scene must be changed
    Scene *scene;           // scene to be used
    Scene *nextScene;
    // change current scene
    status SetScene(Scene *scn) {
        if (scene != NULL){
            GraphicsManager::getInstance()->stop();
            GraphicsManager::getInstance()->reset();
            SAFE_DELETE(scene);
            scene = NULL;
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
    }
    void ChangeScene(Scene *scn){
        sceneChanged = true;
        nextScene = scn;
    }
protected:
    status onActivate() {
        LOG_DEBUG("Activate Engine.");
        if (firstStart) {
            LOG_DEBUG("First start detected.");
            ChangeScene(new MainMenu());
            firstStart = false;
        }
        return STATUS_OK;
    }
    status onStep() {
        if (quit) return STATUS_EXIT;
        // change scene
        if (sceneChanged) {
            sceneChanged = false;
            SetScene(nextScene);
            nextScene = NULL;
        }
        if (scene != NULL && !paused) scene->update();
        return STATUS_OK;
    }
    void onDeactivate() {
        LOG_DEBUG("Dectivating Engine.");
    }
    void onDestroy() {
        if (scene != NULL) SAFE_DELETE(scene);
    }
};

#endif
