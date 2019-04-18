#ifndef __SOUNDMANAGER_H__
#define __SOUNDMANAGER_H__

#include <android_native_app_glue.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "Singleton.h"
#include "Resource.h"

class SoundManager;

class Sound {
public:
	Sound(const char* path) :
		resource(path),
		buffer(NULL),
		length(0) {
		//
	}
	const char* getPath() {
		return resource.getPath();
	}
	uint8_t* getBuffer() {
		return buffer;
	};
	off_t getLength() {
		return length;
	};
	status load() {
		LOG_INFO("Loading sound: %s", resource.getPath());
		// Opens sound file.
		if (resource.open() != STATUS_OK) goto ERROR;
		// Reads sound file.
		length = resource.getLength();
		buffer = new uint8_t[length];
		if (resource.read(buffer, length) != STATUS_OK) goto ERROR;
		resource.close();
		return STATUS_OK;
ERROR:
		LOG_ERROR("Error while reading sound");
		return STATUS_ERROR;
	}
	status unload() {
		delete[] buffer;
		buffer = NULL;
		length = 0;
		return STATUS_OK;
	}
private:
	friend class SoundManager;
	Resource resource;
	uint8_t* buffer;
	off_t length;
};

class SoundQueue {
public:
	SoundQueue() :
		playerObj(NULL),
		playerPlay(NULL),
		playerQueue() {
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
		LOG_ERROR("Error while starting SoundQueue");
		return STATUS_ERROR;
	}
	void finalize() {
		LOG_INFO("Stopping SoundQueue");
		// Destroys sound player.
		if (playerObj != NULL) {
			(*playerObj)->Destroy(playerObj);
			playerObj = NULL;
			playerPlay = NULL;
			playerQueue = NULL;
		}
	}
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
		LOG_ERROR("Error trying to play sound");
	}
private:
	// Sound player.
	SLObjectItf playerObj;
	SLPlayItf playerPlay;
	SLBufferQueueItf playerQueue;
};

class SoundManager: public Singleton<SoundManager> {
public:
	SoundManager() :
		engineObj(NULL),
		engine(NULL),
		outputMixObj(NULL),
		playerObj(NULL),
		playerPlay(NULL),
		playerSeek(NULL),
		soundQueues(),
		currentQueue(0),
		sounds() {
		LOG_INFO("Creating SoundManager");
	};
	~SoundManager() {
		LOG_INFO("Destructing SoundManager");
		for (std::vector<Sound*>::iterator it = sounds.begin(); it < sounds.end(); ++it) {
			delete (*it);
		};
		sounds.clear();
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
		LOG_INFO("Starting sound player with %d SoundQueue", QUEUE_COUNT);
		for (int32_t i= 0; i < QUEUE_COUNT; ++i) {
			if (soundQueues[i].initialize(engine, outputMixObj) != STATUS_OK) goto ERROR;
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
		for (std::vector<Sound*>::iterator it = sounds.begin(); it < sounds.end(); ++it) {
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
	}
	status playMusic(const char* path) {
		SLresult result;
		LOG_INFO("Opening music: %s", path);
		Resource resource(path);
		ResourceDescriptor rd = resource.descript();
		if (rd.descriptor < 0) {
			LOG_INFO("Could not open music file");
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
		LOG_ERROR("Error playing music");
		return STATUS_ERROR;
	}
	void stopMusic() {
		LOG_INFO("Stopping Music");
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
	}
	Sound* registerSound(const char* path) {
		// Finds out if sound already loaded.
		for (std::vector<Sound*>::iterator it = sounds.begin(); it < sounds.end(); ++it) {
			if (strcmp(path, (*it)->getPath()) == 0) return (*it);
		}
		// Appends a new sound to the sound array.
		Sound* sound = new Sound(path);
		sounds.push_back(sound);
		return sound;
	}
	void playSound(Sound* sound) {
		int32_t currentQueue = ++currentQueue;
		SoundQueue& soundQueue = soundQueues[currentQueue % QUEUE_COUNT];
		soundQueue.playSound(sound);
	}
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

#endif