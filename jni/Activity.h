#ifndef __ACTIVITY_H__
#define __ACTIVITY_H__

#include "Scene.h"

class Activity: public ActivityHandler {
private:
    bool sceneChanged;      // scene must be changed
    Scene *scene;           // scene to be used
    Scene *nextScene;
    bool configSaved;
    // Change current scene.
    status setScene(Scene *scn) {
        if (scene != NULL) {
            GraphicsManager::getInstance()->reset();
            SoundManager::getInstance()->reset();
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
    bool firstStart;        // indicates if application is launched for the first time
    bool paused;            // activity paused state
    Activity():
        quit(false),
        firstStart(true),
        paused(false),
        scene(NULL),
        sceneChanged(false),
        configSaved(false) {
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
        configSaved = false;
        if (firstStart) {
            LOG_DEBUG("First start detected ...");
            if (setStartScene() != STATUS_OK) return STATUS_ERROR;
            firstStart = false;
        }
        SoundManager::getInstance()->setSoundVolume((float)configData->soundVolume / 100.0f);
        SoundManager::getInstance()->setMusicVolume((float)configData->musicVolume / 100.0f);
        if (SoundManager::getInstance()->playMusic("sounds/Intro.mp3") != STATUS_OK) return STATUS_ERROR;
        return STATUS_OK;
    };
    status onStep() {
        if (quit) return STATUS_EXIT;
        // Change scene.
        if (sceneChanged) {
            sceneChanged = false;
            if (setScene(nextScene) != STATUS_OK) return STATUS_ERROR;
            nextScene = NULL;
        }
        if (scene != NULL && !paused) scene->update();
        return STATUS_OK;
    };
    void onPause() {
        paused = true;
        if (scene != NULL) scene->pause();
    };
    void onResume() {
        paused = false;
        if (scene != NULL) scene->resume();
    };
    void onDeactivate() {
        LOG_INFO("Deactivating Engine.");
    };
    void onCreateWindow() {
        readConfig();
    };
    void onDestroyWindow() {
        if (!configSaved) {
            writeConfig();
            configSaved = true;
        }
    };
    void onDestroy() {
        clearGameState();
        if (!configSaved) {
            writeConfig();
            configSaved = true;
        }
        if (scene != NULL) SAFE_DELETE(scene);
    };
};

// Scenes.
#include "scenes/SoundSetting.h"
#include "scenes/Gameplay.h"
#include "scenes/MainMenu.h"

status Activity::setStartScene() {
    return setScene(new MainMenu(this));
}

#endif // __ACTIVITY_H__
