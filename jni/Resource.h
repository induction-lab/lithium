#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <android_native_app_glue.h>

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
	Resource(const char* path) :
		assetManager(application->activity->assetManager),
		filePath(path),
		asset(NULL) {
		//
	}
	status open() {
		asset = AAssetManager_open(assetManager, filePath, AASSET_MODE_UNKNOWN);
		return (asset != NULL) ? STATUS_OK : STATUS_ERROR;
	}
	const char* getPath() {
		return filePath;
	}
	status read(void* buffer, size_t count) {
		int32_t readCount = AAsset_read(asset, (char*)buffer, count);
		return (readCount == count) ? STATUS_OK : STATUS_ERROR;
	}
	void close() {
		if (asset != NULL) {
			AAsset_close(asset);
			asset = NULL;
		}
	}
	ResourceDescriptor descript() {
		ResourceDescriptor rd = { -1, 0, 0 };
		AAsset* asset = AAssetManager_open(assetManager, filePath, AASSET_MODE_UNKNOWN);
		if (asset != NULL) {
			rd.descriptor = AAsset_openFileDescriptor(asset, &rd.start, &rd.length);
			AAsset_close(asset);
		}
		return rd;
	}
	off_t getLength() {
		return AAsset_getLength(asset);
	}
};

#endif
