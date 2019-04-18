#ifndef __SOUNDMANAGER_H__
#define __SOUNDMANAGER_H__

#include <android_native_app_glue.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class SoundManager;

class Sound {
public:
    Sound(android_app* application, const char* path) :
        mResource(application, path),
        mBuffer(NULL),
        mLength(0) {
        //
    }
    const char* getPath() {
        return mResource.getPath();
    }
    uint8_t* getBuffer() {
        return mBuffer;
    };
    off_t getLength() {
        return mLength;
    };
    status load() {
        LOG_INFO("Loading sound: %s", mResource.getPath());
        // Opens sound file.
        if (mResource.open() != STATUS_OK) goto ERROR;
        // Reads sound file.
        mLength = mResource.getLength();
        mBuffer = new uint8_t[mLength];
        if (mResource.read(mBuffer, mLength) != STATUS_OK) goto ERROR;
        mResource.close();
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error while reading sound");
        return STATUS_ERROR;
    }
    status unload() {
        delete[] mBuffer;
        mBuffer = NULL;
        mLength = 0;
        return STATUS_OK;
    }
private:
    friend class SoundManager;
    Resource mResource;
    uint8_t* mBuffer;
    off_t mLength;
};

class SoundQueue {
public:
    SoundQueue() :
        mPlayerObj(NULL),
        mPlayer(NULL),
        mPlayerQueue() {
        //
    }
    status initialize(SLEngineItf engine, SLObjectItf outputMixObj) {
        LOG_INFO("Starting SoundQueue");
        SLresult result;
        // Set-up sound audio source.
        SLDataLocator_AndroidSimpleBufferQueue dataLocatorIn;
        dataLocatorIn.locatorType =	SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
        // At most one buffer in the queue.
        dataLocatorIn.numBuffers = 1;
        SLDataFormat_PCM dataFormat;
        dataFormat.formatType    = SL_DATAFORMAT_PCM;
        dataFormat.numChannels   = 1; // Mono sound.
        dataFormat.samplesPerSec = SL_SAMPLINGRATE_44_1;
        dataFormat.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
        dataFormat.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
        dataFormat.channelMask   = SL_SPEAKER_FRONT_CENTER;
        dataFormat.endianness    = SL_BYTEORDER_LITTLEENDIAN;
        SLDataSource dataSource;
        dataSource.pLocator = &dataLocatorIn;
        dataSource.pFormat = &dataFormat;
        SLDataLocator_OutputMix dataLocatorOut;
        dataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
        dataLocatorOut.outputMix = outputMixObj;
        SLDataSink dataSink;
        dataSink.pLocator = &dataLocatorOut;
        dataSink.pFormat = NULL;
        // Creates the sounds player and retrieves its interfaces.
        const SLuint32 soundPlayerIIDCount = 2;
        const SLInterfaceID soundPlayerIIDs[] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE };
        const SLboolean soundPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
        result = (*engine)->CreateAudioPlayer(engine, &mPlayerObj, &dataSource, &dataSink, soundPlayerIIDCount, soundPlayerIIDs, soundPlayerReqs);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*mPlayerObj)->Realize(mPlayerObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_PLAY, &mPlayer);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*mPlayerObj)->GetInterface(mPlayerObj, SL_IID_BUFFERQUEUE, &mPlayerQueue);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        // Starts the sound player. Nothing can be heard while the
        // sound queue remains empty.
        result = (*mPlayer)->SetPlayState(mPlayer, SL_PLAYSTATE_PLAYING);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error while starting SoundQueue");
        return STATUS_ERROR;
    }
    void finalize() {
        LOG_INFO("Stopping SoundQueue");
        // Destroys sound player.
        if (mPlayerObj != NULL) {
            (*mPlayerObj)->Destroy(mPlayerObj);
            mPlayerObj = NULL;
            mPlayer = NULL;
            mPlayerQueue = NULL;
        }
    }
    void playSound(Sound* sound) {
        SLresult result;
        SLuint32 playerState;
        (*mPlayerObj)->GetState(mPlayerObj, &playerState);
        if (playerState == SL_OBJECT_STATE_REALIZED) {
            int16_t* buffer = (int16_t*) sound->getBuffer();
            off_t length = sound->getLength();
            // Removes any sound from the queue.
            result = (*mPlayerQueue)->Clear(mPlayerQueue);
            if (result != SL_RESULT_SUCCESS) goto ERROR;
            // Plays the new sound.
            result = (*mPlayerQueue)->Enqueue(mPlayerQueue, buffer, length);
            if (result != SL_RESULT_SUCCESS) goto ERROR;
        }
        return;
ERROR:
        LOG_ERROR("Error trying to play sound");
    }
private:
    // Sound player.
    SLObjectItf mPlayerObj;
    SLPlayItf mPlayer;
    SLBufferQueueItf mPlayerQueue;
};

class SoundManager {
public:
    SoundManager(android_app* application) :
        mApplication(application),
        mEngineObj(NULL),
        mEngine(NULL),
        mOutputMixObj(NULL),
        mMusicPlayerObj(NULL),
        mMusicPlayer(NULL),
        mMusicPlayerSeek(NULL),
        mSoundQueues(),
        mCurrentQueue(0),
        mSounds() {
        LOG_INFO("Creating SoundManager");
        mInstance = this;
    };
    ~SoundManager() {
        LOG_INFO("Destructing SoundManager");
        for (std::vector<Sound*>::iterator soundIt = mSounds.begin(); soundIt < mSounds.end(); ++soundIt) {
            delete (*soundIt);
        };
        mSounds.clear();
        mInstance = 0;
    }
    static SoundManager* getPtr() {
        return mInstance;
    }
    status start() {
        LOG_INFO("Starting SoundManager");
        SLresult result;
        const SLuint32      engineMixIIDCount = 1;
        const SLInterfaceID engineMixIIDs[]   = { SL_IID_ENGINE };
        const SLboolean     engineMixReqs[]   = { SL_BOOLEAN_TRUE };
        const SLuint32      outputMixIIDCount = 0;
        const SLInterfaceID outputMixIIDs[]   = {};
        const SLboolean     outputMixReqs[]   = {};
        // Creates OpenSL ES engine and dumps its capabilities.
        result = slCreateEngine(&mEngineObj, 0, NULL, engineMixIIDCount, engineMixIIDs, engineMixReqs);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*mEngineObj)->Realize(mEngineObj,SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE, &mEngine);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        // Creates audio output.
        result = (*mEngine)->CreateOutputMix(mEngine, &mOutputMixObj, outputMixIIDCount, outputMixIIDs, outputMixReqs);
        result = (*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE);
        // Set-up sound player.
        LOG_INFO("Starting sound player with %d SoundQueue", QUEUE_COUNT);
        for (int32_t i= 0; i < QUEUE_COUNT; ++i) {
            if (mSoundQueues[i].initialize(mEngine, mOutputMixObj) != STATUS_OK) goto ERROR;
        }
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error while starting SoundManager");
        stop();
        return STATUS_ERROR;
    }
    status loadResources() {
        LOG_INFO("SoundManager: Loads resources");
        // Loads resources
        for (std::vector<Sound*>::iterator it = mSounds.begin(); it < mSounds.end(); ++it) {
            if ((*it)->load() != STATUS_OK) return STATUS_ERROR;
        };
        return STATUS_OK;
    }
    void stop() {
        LOG_INFO("Stopping SoundManager");
        // Stops and destroys Music player.
        stopMusic();
        // Destroys sound player.
        for (int32_t i= 0; i < QUEUE_COUNT; ++i) {
            mSoundQueues[i].finalize();
        }
        // Destroys audio output and engine.
        if (mOutputMixObj != NULL) {
            (*mOutputMixObj)->Destroy(mOutputMixObj);
            mOutputMixObj = NULL;
        }
        if (mEngineObj != NULL) {
            (*mEngineObj)->Destroy(mEngineObj);
            mEngineObj = NULL;
            mEngine = NULL;
        }
        // Frees sound resources.
        for (std::vector<Sound*>::iterator it = mSounds.begin(); it < mSounds.end(); ++it) {
            (*it)->unload();
        }
    }
    status playMusic(const char* path) {
        SLresult result;
        LOG_INFO("Opening music: %s", path);
        Resource resource(mApplication, path);
        ResourceDescriptor descriptor = resource.descript();
        if (descriptor.mDescriptor < 0) {
            LOG_INFO("Could not open music file");
            return STATUS_ERROR;
        }
        // Creates Music player and retrieves its interfaces.
        SLDataLocator_AndroidFD dataLocatorIn = { SL_DATALOCATOR_ANDROIDFD, descriptor.mDescriptor, descriptor.mStart, descriptor.mLength };
        SLDataFormat_MIME dataFormat = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
        SLDataSource dataSource= { &dataLocatorIn, &dataFormat };
        SLDataLocator_OutputMix dataLocatorOut = { SL_DATALOCATOR_OUTPUTMIX, mOutputMixObj };
        SLDataSink dataSink = { &dataLocatorOut, NULL };
        // Creates Music player and retrieves its interfaces.
        const SLuint32 musicPlayerIIDCount = 2;
        const SLInterfaceID musicPlayerIIDs[] = { SL_IID_PLAY, SL_IID_SEEK };
        const SLboolean musicPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
        result = (*mEngine)->CreateAudioPlayer(mEngine, &mMusicPlayerObj, &dataSource, &dataSink, musicPlayerIIDCount, musicPlayerIIDs, musicPlayerReqs);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*mMusicPlayerObj)->Realize(mMusicPlayerObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*mMusicPlayerObj)->GetInterface(mMusicPlayerObj, SL_IID_PLAY, &mMusicPlayer);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*mMusicPlayerObj)->GetInterface(mMusicPlayerObj, SL_IID_SEEK, &mMusicPlayerSeek);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        // Enables looping and starts playing.
        result = (*mMusicPlayerSeek)->SetLoop(mMusicPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*mMusicPlayer)->SetPlayState(mMusicPlayer, SL_PLAYSTATE_PLAYING);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error playing music");
        return STATUS_ERROR;
    }
    void stopMusic() {
        LOG_INFO("Stopping Music");
        if (mMusicPlayer != NULL) {
            SLuint32 musicPlayerState;
            (*mMusicPlayerObj)->GetState(mMusicPlayerObj, &musicPlayerState);
            if (musicPlayerState == SL_OBJECT_STATE_REALIZED) {
                (*mMusicPlayer)->SetPlayState(mMusicPlayer, SL_PLAYSTATE_PAUSED);
                (*mMusicPlayerObj)->Destroy(mMusicPlayerObj);
                mMusicPlayerObj = NULL;
                mMusicPlayer = NULL;
                mMusicPlayerSeek = NULL;
            }
        }
    }
    Sound* registerSound(const char* path) {
        // Finds out if sound already loaded.
        for (std::vector<Sound*>::iterator it = mSounds.begin(); it < mSounds.end(); ++it) {
            if (strcmp(path, (*it)->getPath()) == 0) return (*it);
        }
        // Appends a new sound to the sound array.
        Sound* sound = new Sound(mApplication, path);
        mSounds.push_back(sound);
        return sound;
    }
    void playSound(Sound* sound) {
        int32_t currentQueue = ++mCurrentQueue;
        SoundQueue& soundQueue = mSoundQueues[currentQueue % QUEUE_COUNT];
        soundQueue.playSound(sound);
    }
private:
    static SoundManager* mInstance;
    android_app* mApplication;
    // OpenSL ES engine.
    SLObjectItf mEngineObj;
    SLEngineItf mEngine;
    // Audio output.
    SLObjectItf mOutputMixObj;
    // Background music player.
    SLObjectItf mMusicPlayerObj;
    SLPlayItf mMusicPlayer;
    SLSeekItf mMusicPlayerSeek;
    // Sound players.
    static const int32_t QUEUE_COUNT = 4;
    SoundQueue mSoundQueues[QUEUE_COUNT];
    int32_t mCurrentQueue;
    // Sounds.
    std::vector<Sound*> mSounds;
};

SoundManager* SoundManager::mInstance = 0;

#endif