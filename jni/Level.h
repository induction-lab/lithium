#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "Resource.h"

class Level {
public:
    Level() {
        //
    };
    ~Level() {};
    status loadFromFile(const char* path) {
        Resource resource(path);
        LOG_INFO("Loading level: %s", resource.getPath());
        GLint result;
        if (resource.open() != STATUS_OK) {
            LOG_ERROR("Error open level resource.");
            resource.close();
            return STATUS_ERROR;
        }
        GLint levelLength = resource.getLength();
        char *levelBuffer = new char[levelLength];
        if (resource.read(levelBuffer, levelLength) != STATUS_OK) {
            LOG_ERROR("Error reading level resource");
            resource.close();
            SAFE_DELETE_ARRAY(levelBuffer);
            return STATUS_ERROR;
        }
        float x;
        float y;
        sscanf(levelBuffer, "%f %f", &x, &y);
        LOG_INFO("This: %f %f", x, y);
        LOG_INFO("%s", levelBuffer);        
        return STATUS_OK;
ERROR:
        resource.close();
        return STATUS_ERROR;
    };
};

#endif // __LEVEL_H__
