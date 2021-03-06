#ifndef __SOUNDQUEUE_H__
#define __SOUNDQUEUE_H__

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class SoundQueue {
public:
    SoundQueue():
        playerObj(NULL),
        playerVolume(NULL),
        playerPlay(NULL),
        playerQueue() {
        //
    };
    status initialize(SLEngineItf engine, SLObjectItf outputMixObj) {
        LOG_DEBUG("Starting SoundQueue.");
        // Set-up sound audio source.
        SLDataLocator_AndroidSimpleBufferQueue dataLocatorIn;
        dataLocatorIn.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
        // At most one buffer in the queue.
        dataLocatorIn.numBuffers = 1;
        SLDataFormat_PCM dataFormat;
        dataFormat.formatType    = SL_DATAFORMAT_PCM;
        dataFormat.numChannels   = 1; // mono sound
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
        const SLuint32 soundPlayerIIDCount = 3;
        const SLInterfaceID soundPlayerIIDs[] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
        const SLboolean soundPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
        if ((*engine)->CreateAudioPlayer(engine, &playerObj, &dataSource, &dataSink, soundPlayerIIDCount, soundPlayerIIDs, soundPlayerReqs) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*playerObj)->Realize(playerObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*playerObj)->GetInterface(playerObj, SL_IID_VOLUME, &playerVolume) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &playerPlay) != SL_RESULT_SUCCESS) goto ERROR;
        if ((*playerObj)->GetInterface(playerObj, SL_IID_BUFFERQUEUE, &playerQueue) != SL_RESULT_SUCCESS) goto ERROR;
        // Starts the sound player. Nothing can be heard while the sound queue remains empty.
        if ((*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING) != SL_RESULT_SUCCESS) goto ERROR;
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error while starting SoundQueue.");
        return STATUS_ERROR;
    };
    void finalize() {
        LOG_DEBUG("Stopping SoundQueue.");
        // Destroys sound player.
        if (playerObj != NULL) {
            (*playerObj)->Destroy(playerObj);
            playerObj = NULL;
            playerVolume = NULL;
            playerPlay = NULL;
            playerQueue = NULL;
        }
    };
    void reset() {
        SLuint32 playerState;
        (*playerObj)->GetState(playerObj, &playerState);
        if (playerState == SL_OBJECT_STATE_REALIZED) (*playerQueue)->Clear(playerQueue);
    };
    void playSound(Sound* sound) {
        SLuint32 playerState;
        (*playerObj)->GetState(playerObj, &playerState);
        if (playerState == SL_OBJECT_STATE_REALIZED) {
            int16_t* buffer = (int16_t*) sound->getBuffer();
            off_t length = sound->getLength();
            // Removes any sound from the queue.
            if ((*playerQueue)->Clear(playerQueue) != SL_RESULT_SUCCESS) goto ERROR;
            // Plays the new sound.
            if ((*playerQueue)->Enqueue(playerQueue, buffer, length) != SL_RESULT_SUCCESS) goto ERROR;
        }
        return;
ERROR:
        LOG_ERROR("Error trying to play sound.");
    };
    void setVolume(float volume) {
        if (playerVolume == NULL) return;
        // Millibels from linear amplification.
        int millibels = lroundf(2000.f * log10f(volume));
        if (millibels < SL_MILLIBEL_MIN) millibels = SL_MILLIBEL_MIN;
        // Maximum supported level could be higher: GetMaxVolumeLevel.
        else if (millibels > 0) millibels = 0;
        if ((*playerVolume)->SetVolumeLevel(playerVolume, millibels) != SL_RESULT_SUCCESS) LOG_ERROR("Error setting queue volume level.");
    };
private:
    // Sound player.
    SLObjectItf playerObj;
    SLPlayItf playerPlay;
    SLVolumeItf playerVolume;
    SLBufferQueueItf playerQueue;
};

#endif // __SOUNDQUEUE_H__