#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <GLES/gl.h>
#include <png.h>

#include "Resource.h"

class Texture {
public:
	Texture(const char* path) :
		resource(path),
		textureId(0),
		width(0), height(0) {
		//
	}
	const char* getPath() {
		return resource.getPath();
	}
	int32_t getHeight() {
		return height;
	}
	int32_t getWidth() {
		return width;
	}
	status load() {
		uint8_t* imageBuffer = loadImage();
		if (imageBuffer == NULL) return STATUS_ERROR;
		// Creates a new OpenGL texture.
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		// Set-up texture properties.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Loads image data into OpenGL.
		glTexImage2D(GL_TEXTURE_2D, 0, mFormat, width, height, 0, mFormat, GL_UNSIGNED_BYTE, imageBuffer);
		delete[] imageBuffer;
		if (glGetError() != GL_NO_ERROR) {
			LOG_ERROR("Error loading texture into OpenGL.");
			unload();
			return STATUS_ERROR;
		}
		return STATUS_OK;
	}
	void unload() {
		if (textureId != 0) {
			glDeleteTextures(1, &textureId);
			textureId = 0;
		}
		width = 0;
		height = 0;
		mFormat = 0;
	}
	void apply() {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
	GLuint getId() {
		return textureId;
	}
protected:
	uint8_t* loadImage() {
		LOG_INFO("Loading texture: %s", resource.getPath());
		png_byte header[8];
		png_structp pngPtr = NULL;
		png_infop infoPtr = NULL;
		png_byte* imageBuffer = NULL;
		png_bytep* rowPtrs = NULL;
		png_int_32 rowSize;
		bool transparency;
		// Opens and checks image signature (first 8 bytes).
		if (resource.open() != STATUS_OK) goto ERROR;
		if (resource.read(header, sizeof(header)) != STATUS_OK) goto ERROR;
		if (png_sig_cmp(header, 0, 8) != 0) goto ERROR;
		// Creates required structures.
		pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!pngPtr) goto ERROR;
		infoPtr = png_create_info_struct(pngPtr);
		if (!infoPtr) goto ERROR;
		// Prepares reading operation by setting-up a read callback.
		png_set_read_fn(pngPtr, &resource, callback_read);
		// Set-up error management. If an error occurs while reading,
		// code will come back here and jump
		if (setjmp(png_jmpbuf(pngPtr))) goto ERROR;
		// Ignores first 8 bytes already read and processes header.
		png_set_sig_bytes(pngPtr, 8);
		png_read_info(pngPtr, infoPtr);
		// Retrieves PNG info and updates PNG struct accordingly.
		png_int_32 depth, colorType;
		png_uint_32 pngWidth, pngHeight;
		png_get_IHDR(pngPtr, infoPtr, &pngWidth, &pngHeight, &depth, &colorType, NULL, NULL, NULL);
		width = pngWidth;
		height = pngHeight;
		// Creates a full alpha channel if transparency is encoded as
		// an array of palette entries or a single transparent color.
		transparency = false;
		if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) {
			png_set_tRNS_to_alpha(pngPtr);
			transparency = true;
			goto ERROR;
		}
		// Expands PNG with less than 8bits per channel to 8bits.
		if (depth < 8) {
			png_set_packing (pngPtr);
			// Shrinks PNG with 16bits per color channel down to 8bits.
		} else if (depth == 16) {
			png_set_strip_16(pngPtr);
		}
		// Indicates that image needs conversion to RGBA if needed.
		switch (colorType) {
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(pngPtr);
			mFormat = transparency ? GL_RGBA : GL_RGB;
			break;
		case PNG_COLOR_TYPE_RGB:
			mFormat = transparency ? GL_RGBA : GL_RGB;
			break;
		case PNG_COLOR_TYPE_RGBA:
			mFormat = GL_RGBA;
			break;
		case PNG_COLOR_TYPE_GRAY:
			png_set_expand_gray_1_2_4_to_8(pngPtr);
			mFormat = transparency ? GL_LUMINANCE_ALPHA:GL_LUMINANCE;
			break;
		case PNG_COLOR_TYPE_GA:
			png_set_expand_gray_1_2_4_to_8(pngPtr);
			mFormat = GL_LUMINANCE_ALPHA;
			break;
		}
		// Validates all tranformations.
		png_read_update_info(pngPtr, infoPtr);
		// Get row size in bytes.
		rowSize = png_get_rowbytes(pngPtr, infoPtr);
		if (rowSize <= 0) goto ERROR;
		// Ceates the image buffer that will be sent to OpenGL.
		imageBuffer = new png_byte[rowSize * height];
		if (!imageBuffer) goto ERROR;
		// Pointers to each row of the image buffer. Row order is
		// inverted because different coordinate systems are used by
		// OpenGL (1st pixel is at bottom left) and PNGs (top-left).
		rowPtrs = new png_bytep[height];
		if (!rowPtrs) goto ERROR;
		for (int32_t i = 0; i < height; ++i) {
			rowPtrs[height - (i + 1)] = imageBuffer + i * rowSize;
		}
		// Reads image content.
		png_read_image(pngPtr, rowPtrs);
		// Frees memory and resources.
		resource.close();
		png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
		delete[] rowPtrs;
		LOG_INFO("Texture size: %d x %d", width, height);
		return imageBuffer;
ERROR:
		LOG_ERROR("Error while reading PNG file");
		resource.close();
		delete[] rowPtrs;
		delete[] imageBuffer;
		if (pngPtr != NULL) {
			png_infop* infoPtrP = infoPtr != NULL ? &infoPtr: NULL;
			png_destroy_read_struct(&pngPtr, infoPtrP, NULL);
		}
		return NULL;
	}
private:
	static void callback_read(png_structp pStruct, png_bytep pData, png_size_t pSize) {
		Resource* lResource = ((Resource*) png_get_io_ptr(pStruct));
		if (lResource->read(pData, pSize) != STATUS_OK) {
			lResource->close();
		}
	}
public:
	Resource resource;
	GLuint textureId;
	int32_t width, height;
	GLint mFormat;
};

#endif
