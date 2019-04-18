#ifndef __SOUND_H__
#define __SOUND_H__

#include "Resource.h"
#include "SoundManager.h"

class SoundManager;

class Sound {
public:
    Sound(const char* path):
        resource(path),
        buffer(NULL),
        length(0) {
        //
    };
    const char* getPath() {
        return resource.getPath();
    };
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
        LOG_ERROR("Error while reading sound.");
        return STATUS_ERROR;
    };
    status unload() {
        SAFE_DELETE_ARRAY(buffer);
        length = 0;
        return STATUS_OK;
    };
private:
    friend class SoundManager;
    Resource resource;
    uint8_t* buffer;
    off_t length;
};

#endif // __SOUND_H__