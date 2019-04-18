#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <android_native_app_glue.h>

struct ResourceDescriptor {
	int32_t mDescriptor;
	off_t mStart;
	off_t mLength;
};

class Resource {
public:
    Resource() {};
	Resource(android_app* application, const char* path) :
        mAssetManager(application->activity->assetManager),
        mPath(path),
        mAsset(NULL) {
		//
	}
	status open() {
        mAsset = AAssetManager_open(mAssetManager, mPath, AASSET_MODE_UNKNOWN);
        return (mAsset != NULL) ? STATUS_OK : STATUS_ERROR;
    }
    const char* getPath() {
        return mPath;
    }
    status read(void* buffer, size_t count) {
        int32_t readCount = AAsset_read(mAsset, (char*)buffer, count);
        return (readCount == count) ? STATUS_OK : STATUS_ERROR;
    }
    void close() {
        if (mAsset != NULL) {
            AAsset_close(mAsset);
            mAsset = NULL;
        }
    }
	ResourceDescriptor descript() {
		ResourceDescriptor descriptor = { -1, 0, 0 };
		AAsset* asset = AAssetManager_open(mAssetManager, mPath, AASSET_MODE_UNKNOWN);
		if (asset != NULL) {
			descriptor.mDescriptor = AAsset_openFileDescriptor(asset, &descriptor.mStart, &descriptor.mLength);
			AAsset_close(asset);
		}
		return descriptor;
	}	
    off_t getLength() {
        return AAsset_getLength(mAsset);
    }
private:
    const char* mPath;
    AAssetManager* mAssetManager;
    AAsset* mAsset;
};

#endif
