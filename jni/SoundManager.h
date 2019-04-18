#ifndef __SOUNDMANAGER_H__
#define __SOUNDMANAGER_H__

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "Singleton.h"
#include "Resource.h"
#include "Sound.h"
#include "SoundQueue.h"

class SoundManager: public Singleton<SoundManager> {
public:
    SoundManager():
        engineObj(NULL),
        engine(NULL),
        outputMixObj(NULL),
        playerObj(NULL),
        playerPlay(NULL),
        playerSeek(NULL),
        soundQueues(),
        currentQueue(0),
        sounds() {
        LOG_INFO("Creating SoundManager.");
    };
    ~SoundManager() {
        LOG_INFO("Destructing SoundManager.");
        reset();
    };
    status start() {
        LOG_INFO("Starting SoundManager.");
        SLresult result;
        const SLuint32      engineMixIIDCount = 1;
        const SLInterfaceID engineMixIIDs[]   = { SL_IID_ENGINE };
        const SLboolean     engineMixReqs[]   = { SL_BOOLEAN_TRUE };
        const SLuint32      outputMixIIDCount = 1;
        const SLInterfaceID outputMixIIDs[]   = { SL_IID_VOLUME };
        const SLboolean     outputMixReqs[]   = { SL_BOOLEAN_FALSE };
        // Creates OpenSL ES engine and dumps its capabilities.
        result = slCreateEngine(&engineObj, 0, NULL, engineMixIIDCount, engineMixIIDs, engineMixReqs);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        // Creates audio output.
        result = (*engine)->CreateOutputMix(engine, &outputMixObj, outputMixIIDCount, outputMixIIDs, outputMixReqs);
        result = (*outputMixObj)->Realize(outputMixObj, SL_BOOLEAN_FALSE);
        // Set-up sound player.
        LOG_INFO("Starting sound player with %d SoundQueue.", QUEUE_COUNT);
        for (int32_t i= 0; i < QUEUE_COUNT; ++i) {
            if (soundQueues[i].initialize(engine, outputMixObj) != STATUS_OK) goto ERROR;
        }
        if (loadResources() != STATUS_OK) goto ERROR;
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error while starting SoundManager.");
        stop();
        return STATUS_ERROR;
    };
    status loadResources() {
        LOG_DEBUG("Loads sound resources.");
        // Loads resources.
        for (std::vector<Sound*>::iterator it = sounds.begin(); it < sounds.end(); ++it) {
            if ((*it)->load() != STATUS_OK) return STATUS_ERROR;
        };
        return STATUS_OK;
    };
    void stop() {
        LOG_INFO("Stopping SoundManager.");
        // Stops and destroys Music player.
        stopMusic();
        // Destroys sound player.
        for (int32_t i= 0; i < QUEUE_COUNT; ++i) {
            soundQueues[i].finalize();
        }
        // Destroys audio output and engine.
        if (outputMixObj != NULL) {
            (*outputMixObj)->Destroy(outputMixObj);
            outputMixObj = NULL;
        }
        if (engineObj != NULL) {
            (*engineObj)->Destroy(engineObj);
            engineObj = NULL;
            engine = NULL;
        }
        // Frees sound resources.
        for (std::vector<Sound*>::iterator it = sounds.begin(); it < sounds.end(); ++it) {
            (*it)->unload();
        }
    };
    void reset() {
        stopMusic();
        for (std::vector<Sound*>::iterator it = sounds.begin(); it < sounds.end(); ++it) {
            SAFE_DELETE(*it);
        };
        sounds.clear();        
    }
    status playMusic(const char* path) {
        SLresult result;
        LOG_INFO("Opening music: %s", path);
        Resource resource(path);
        ResourceDescriptor rd = resource.descript();
        if (rd.descriptor < 0) {
            LOG_ERROR("Could not open music file.");
            return STATUS_ERROR;
        }
        // Creates Music player and retrieves its interfaces.
        SLDataLocator_AndroidFD dataLocatorIn = { SL_DATALOCATOR_ANDROIDFD, rd.descriptor, rd.start, rd.length };
        SLDataFormat_MIME dataFormat = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
        SLDataSource dataSource = { &dataLocatorIn, &dataFormat };
        SLDataLocator_OutputMix dataLocatorOut = { SL_DATALOCATOR_OUTPUTMIX, outputMixObj };
        SLDataSink dataSink = { &dataLocatorOut, NULL };
        // Creates Music player and retrieves its interfaces.
        const SLuint32 musicPlayerIIDCount = 2;
        const SLInterfaceID musicPlayerIIDs[] = { SL_IID_PLAY, SL_IID_SEEK };
        const SLboolean musicPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
        result = (*engine)->CreateAudioPlayer(engine, &playerObj, &dataSource, &dataSink, musicPlayerIIDCount, musicPlayerIIDs, musicPlayerReqs);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*playerObj)->Realize(playerObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &playerPlay);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*playerObj)->GetInterface(playerObj, SL_IID_SEEK, &playerSeek);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        // Enables looping and starts playing.
        result = (*playerSeek)->SetLoop(playerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error playing music.");
        return STATUS_ERROR;
    };
    void stopMusic() {
        LOG_INFO("Stopping Music.");
        if (playerPlay != NULL) {
            SLuint32 playerState;
            (*playerObj)->GetState(playerObj, &playerState);
            if (playerState == SL_OBJECT_STATE_REALIZED) {
                (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PAUSED);
                (*playerObj)->Destroy(playerObj);
                playerObj = NULL;
                playerPlay = NULL;
                playerSeek = NULL;
            }
        }
    };
    Sound* registerSound(const char* path) {
        // Finds out if sound already loaded.
        for (std::vector<Sound*>::iterator it = sounds.begin(); it < sounds.end(); ++it) {
            if (strcmp(path, (*it)->getPath()) == 0) return (*it);
        }
        // Appends a new sound to the sound array.
        Sound* sound = new Sound(path);
        sounds.push_back(sound);
        return sound;
    };
    void playSound(Sound* sound) {
        int32_t currentQueue = ++currentQueue;
        SoundQueue& soundQueue = soundQueues[currentQueue % QUEUE_COUNT];
        soundQueue.playSound(sound);
    };
private:
    // OpenSL ES engine.
    SLObjectItf engineObj;
    SLEngineItf engine;
    // Audio output.
    SLObjectItf outputMixObj;
    // Background music player.
    SLObjectItf playerObj;
    SLPlayItf playerPlay;
    SLSeekItf playerSeek;
    // Sound players.
    static const int32_t QUEUE_COUNT = 4;
    SoundQueue soundQueues[QUEUE_COUNT];
    int32_t currentQueue;
    // Sounds.
    std::vector<Sound*> sounds;
};

#endif // __SOUNDMANAGER_H__