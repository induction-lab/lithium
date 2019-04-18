#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <fstream>
#include <sys/stat.h>

struct ResourceDescriptor {
    int32_t descriptor;
    off_t start;
    off_t length;
};

class Resource {
private:
    const char* filePath;
    AAssetManager* assetManager;
    AAsset* asset;
public:
    Resource() {};
    Resource(const char* path):
        assetManager(application->activity->assetManager),
        filePath(path),
        asset(NULL) {
        //
    };
    status open() {
        asset = AAssetManager_open(assetManager, filePath, AASSET_MODE_UNKNOWN);
        return (asset != NULL) ? STATUS_OK : STATUS_ERROR;
    };
    const char* getPath() {
        return filePath;
    };
    status read(void* buffer, size_t count) {
        int32_t readCount = AAsset_read(asset, (char*)buffer, count);
        return (readCount == count) ? STATUS_OK : STATUS_ERROR;
    };
    void close() {
        if (asset != NULL) {
            AAsset_close(asset);
            asset = NULL;
        }
    };
    ResourceDescriptor descript() {
        ResourceDescriptor rd = { -1, 0, 0 };
        AAsset* asset = AAssetManager_open(assetManager, filePath, AASSET_MODE_UNKNOWN);
        if (asset != NULL) {
            rd.descriptor = AAsset_openFileDescriptor(asset, &rd.start, &rd.length);
            AAsset_close(asset);
        }
        return rd;
    };
    off_t getLength() {
        return AAsset_getLength(asset);
    };
};

void readConfig() {
    configData = new ConfigData();
    const char* internalPath = application->activity->internalDataPath;
    // internalDataPath points directly to the files/ directory.
    char configFile[255];
    sprintf(configFile, "%s/app_config.xml", internalPath);
    // Sometimes if this is the first time we run the app
    // then we need to create the internal storage "files" directory.
    struct stat sb;
    int32_t result = stat(internalPath, &sb);
    if (result == 0 && sb.st_mode & S_IFDIR) {
        LOG_DEBUG("'files/' dir already in app's internal data storage.");
    } else if (ENOENT == errno) {
        result = mkdir(internalPath, 0770);
    }
    // Test to see if the config file is already present.
    if (result == 0) {
        result = stat(configFile, &sb);
        if (result == 0 && sb.st_mode & S_IFREG) {
            LOG_INFO("Application config file present. Load it ...");
            FILE* appConfigFile = std::fopen(configFile, "rb");
            fseek(appConfigFile, 0, SEEK_END);
            int size = ftell(appConfigFile);
            rewind(appConfigFile);
            fread((char*)configData, 1, size, appConfigFile);
            std::fclose(appConfigFile);
        } else {
            LOG_INFO("Application config file does not exist. Using default values.");
        }
    }
};

void writeConfig() {
    const char* internalPath = application->activity->internalDataPath;
    char configFile[255];
    sprintf(configFile, "%s/app_config.xml", internalPath);
    LOG_INFO("Write application config file it ...");
    // Save the config file contents in the application's internal storage.
    FILE* appConfigFile = std::fopen(configFile, "w+");
    if (appConfigFile == NULL) {
        LOG_ERROR("Could not create app configuration file.");
    } else {
        LOG_INFO("App config file created successfully. Writing config data ...");
        int32_t result = std::fwrite(configData, sizeof(char), sizeof(configData), appConfigFile);
        if (result != sizeof(configData)) {
            LOG_ERROR("Error generating app configuration file.");
        }
    }
    std::fclose(appConfigFile);
    SAFE_DELETE(configData);
};

#endif // __RESOURCE_H__
