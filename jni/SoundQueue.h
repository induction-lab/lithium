#ifndef __SOUNDQUEUE_H__
#define __SOUNDQUEUE_H__

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class SoundQueue {
public:
    SoundQueue():
        playerObj(NULL),
        playerPlay(NULL),
        playerQueue() {
        //
    };
    status initialize(SLEngineItf engine, SLObjectItf outputMixObj) {
        LOG_INFO("Starting SoundQueue.");
        SLresult result;
        // Set-up sound audio source.
        SLDataLocator_AndroidSimpleBufferQueue dataLocatorIn;
        dataLocatorIn.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
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
        result = (*engine)->CreateAudioPlayer(engine, &playerObj, &dataSource, &dataSink, soundPlayerIIDCount, soundPlayerIIDs, soundPlayerReqs);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*playerObj)->Realize(playerObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*playerObj)->GetInterface(playerObj, SL_IID_PLAY, &playerPlay);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        result = (*playerObj)->GetInterface(playerObj, SL_IID_BUFFERQUEUE, &playerQueue);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        // Starts the sound player. Nothing can be heard while the
        // sound queue remains empty.
        result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
        if (result != SL_RESULT_SUCCESS) goto ERROR;
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error while starting SoundQueue.");
        return STATUS_ERROR;
    };
    void finalize() {
        LOG_INFO("Stopping SoundQueue.");
        // Destroys sound player.
        if (playerObj != NULL) {
            (*playerObj)->Destroy(playerObj);
            playerObj = NULL;
            playerPlay = NULL;
            playerQueue = NULL;
        }
    };
    void playSound(Sound* sound) {
        SLresult result;
        SLuint32 playerState;
        (*playerObj)->GetState(playerObj, &playerState);
        if (playerState == SL_OBJECT_STATE_REALIZED) {
            int16_t* buffer = (int16_t*) sound->getBuffer();
            off_t length = sound->getLength();
            // Removes any sound from the queue.
            result = (*playerQueue)->Clear(playerQueue);
            if (result != SL_RESULT_SUCCESS) goto ERROR;
            // Plays the new sound.
            result = (*playerQueue)->Enqueue(playerQueue, buffer, length);
            if (result != SL_RESULT_SUCCESS) goto ERROR;
        }
        return;
ERROR:
        LOG_ERROR("Error trying to play sound.");
    };
private:
    // Sound player.
    SLObjectItf playerObj;
    SLPlayItf playerPlay;
    SLBufferQueueItf playerQueue;
};

#endif // __SOUNDQUEUE_H__