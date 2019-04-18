#ifndef __SOUNDMANAGER_H__
#define __SOUNDMANAGER_H__

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <vector>

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
        playerVolume(NULL),
        playerPlay(NULL),
        playerSeek(NULL),
        soundQueues(),
        currentQueue(0),
        sounds(),
        musicVolume(0),
        soundVolume(0) {
        LOG_INFO("Creating SoundManager.");
    };
    ~SoundManager() {
        LOG_INFO("Destructing SoundManager.");
        reset();
    };
    status start() {
        LOG_INFO("Starting SoundManager.");
        const SLuint32      engineMixIIDCount = 1;
        const SLInterfaceID engineMixIIDs[]   = { SL_IID_ENGINE };
        const SLboolean     engineMixReqs[]   = { SL_BOOLEAN_TRUE };
        const SLuint32      outputMixIIDCount = 1;
        const SLInterfaceID outputMixIIDs[]   = { SL_IID_VOLUME };
        const SLboolean     outputMixReqs[]   = { SL_BOOLEAN_FALSE };
        // Creates OpenSL ES engine and dumps its capabilities.
        if (slCreateEngine(&engineObj, 0, NULL, engineMixIIDCount, engineMixIIDs, engineMixReqs) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine) != SL_RESULT_SUCCESS) goto ERROR;
        // Creates audio output.
        if ((*engine)->CreateOutputMix(engine, &outputMixObj, outputMixIIDCount, outputMixIIDs, outputMixReqs) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*outputMixObj)->Realize(outputMixObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) goto ERROR;

        // Set-up sound player.
        LOG_DEBUG("Starting sound player with %d SoundQueue.", QUEUE_COUNT);
        for (int i= 0; i < QUEUE_COUNT; ++i) {
            if (soundQueues[i].initialize(engine, outputMixObj) != STATUS_OK) goto ERROR;
        }
        if (sounds.size() > 0) {
            if (loadResources() != STATUS_OK) goto ERROR;
        }
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
        for (int i = 0; i < QUEUE_COUNT; ++i) {
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
        if (engine == NULL) return;
        for (int i= 0; i < QUEUE_COUNT; ++i) {
            soundQueues[i].reset();
        }
        for (std::vector<Sound*>::iterator it = sounds.begin(); it < sounds.end(); ++it) {
            (*it)->unload();
            SAFE_DELETE(*it);
        };
        sounds.clear();        
    }
    status playMusic(const char* path) {
        stopMusic();
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
        const SLuint32 musicPlayerIIDCount = 3;
        const SLInterfaceID musicPlayerIIDs[] = { SL_IID_PLAY, SL_IID_SEEK, SL_IID_VOLUME };
        const SLboolean musicPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
        if ((*engine)->CreateAudioPlayer(engine, &playerObj, &dataSource, &dataSink, musicPlayerIIDCount, musicPlayerIIDs, musicPlayerReqs) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*playerObj)->Realize(playerObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &playerPlay) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*playerObj)->GetInterface(playerObj, SL_IID_SEEK, &playerSeek) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*playerObj)->GetInterface(playerObj, SL_IID_VOLUME, &playerVolume) != SL_RESULT_SUCCESS) goto ERROR;
        // Set music volume.
        setMusicVolume(musicVolume);
        // Enables looping and starts playing.
        if ((*playerSeek)->SetLoop(playerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING) != SL_RESULT_SUCCESS) goto ERROR;
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error playing music.");
        return STATUS_ERROR;
    };
    void stopMusic() {
        LOG_DEBUG("Stopping Music.");
        if (playerPlay != NULL) {
            SLuint32 playerState;
            (*playerObj)->GetState(playerObj, &playerState);
            if (playerState == SL_OBJECT_STATE_REALIZED) {
                (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PAUSED);
                (*playerObj)->Destroy(playerObj);
                playerObj = NULL;
                playerVolume = NULL;
                playerPlay = NULL;
                playerSeek = NULL;
            }
        }
    };
    // Set volume level. [0.0f to 1.0f]
    void setMusicVolume(float volume) {
        musicVolume = volume;
        if (playerVolume == NULL) return;
        // Millibels from linear amplification.
        int millibels = lroundf(2000.f * log10f(volume));
        if (millibels < SL_MILLIBEL_MIN) millibels = SL_MILLIBEL_MIN;
        // Maximum supported level could be higher: GetMaxVolumeLevel.
        else if (millibels > 0) millibels = 0;
        if ((*playerVolume)->SetVolumeLevel(playerVolume, millibels) != SL_RESULT_SUCCESS) LOG_ERROR("Error setting music volume level.");
    };
    void setSoundVolume(float volume) {
        soundVolume = volume;
        for (int i = 0; i < QUEUE_COUNT; ++i) soundQueues[i].setVolume(volume);
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
        currentQueue = ++currentQueue;
        SoundQueue& soundQueue = soundQueues[currentQueue % QUEUE_COUNT];
        soundQueue.setVolume(soundVolume);
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
    SLVolumeItf playerVolume;
    SLSeekItf playerSeek;
    // Sound players.
    static const int QUEUE_COUNT = 12;
    SoundQueue soundQueues[QUEUE_COUNT];
    int currentQueue;
    // Sounds.
    std::vector<Sound*> sounds;
    // Volumes.
    float musicVolume;
    float soundVolume;
};

#endif // __SOUNDMANAGER_H__