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
    Resource(android_app* pApplication, const char* pPath) :
            mAssetManager(pApplication->activity->assetManager),
            mPath(pPath),
            mAsset(NULL) {}
    status open() {
        mAsset = AAssetManager_open(mAssetManager, mPath, AASSET_MODE_UNKNOWN);
        return (mAsset != NULL) ? STATUS_OK : STATUS_ERROR;
    }
    status read(void* pBuffer, size_t pCount) {
        int32_t lReadCount = AAsset_read(mAsset, (char*)pBuffer, pCount);
        return (lReadCount == pCount) ? STATUS_OK : STATUS_ERROR;
    }
    void close() {
        if (mAsset != NULL) {
            AAsset_close(mAsset);
            mAsset = NULL;
        }
    }
	ResourceDescriptor descript() {
		ResourceDescriptor lDescriptor = { -1, 0, 0 };
		AAsset* lAsset = AAssetManager_open(mAssetManager, mPath,
		AASSET_MODE_UNKNOWN);
		if (lAsset != NULL) {
			lDescriptor.mDescriptor = AAsset_openFileDescriptor(lAsset, &lDescriptor.mStart, &lDescriptor.mLength);
			AAsset_close(lAsset);
		}
		return lDescriptor;
	}	
    const char* getPath() {
        return mPath;
    }
    off_t getLength() {
        return AAsset_getLength(mAsset);
    }    
private:
    const char* mPath;
    AAssetManager *mAssetManager;
    AAsset* mAsset;
};

#endif
