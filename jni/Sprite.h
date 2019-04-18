#ifndef __SPRITE_H__
#define __SPRITE_H__

class SpriteBatch; 

class Sprite {
public:
    struct Vertex {
        GLfloat x, y, u, v;
    };
    Sprite(const char* texturePath, int32_t width, int32_t height) :
		location(),
		texturePath(texturePath), textureId(0),
		sheetWidth(0), sheetHeight(0),
		spriteWidth(width), spriteHeight(height),
		frameCount(0), frameXCount(0), frameYCount(0),
		animStartFrame(0), animFrameCount(1),
		animSpeed(0.0f), animFrame(0.0f), animLoop(false) {
		//
	}
    void setFrames(int32_t startFrame, int32_t frameCount, float speed, bool loop) {
		animStartFrame = startFrame;
		animFrame = 0.0f, animSpeed = speed, animLoop = loop;
		animFrameCount = frameCount;
	}
    bool animationEnded() { return animFrame > (animFrameCount-1); }
    void setLocation(float x, float y) { location = Location(x, y); }
	Location getLocation() { return location; }
	int32_t getSpriteHeight() { return spriteHeight; }
	int32_t getSpriteWidth() { return spriteWidth; }
protected:
    friend class SpriteBatch;
    status initialize() {
		Texture* texture = GraphicsManager::getInstance()->loadTexture(texturePath, GL_NEAREST, GL_CLAMP_TO_EDGE);
		textureId = texture->getTextureId();
		sheetWidth = texture->getWidth();
		sheetHeight = texture->getHeight();
		frameXCount = sheetWidth / spriteWidth;
		frameYCount = sheetHeight / spriteHeight;
		frameCount = (sheetHeight / spriteHeight) * (sheetWidth / spriteWidth);
		return STATUS_OK;
	}
    void draw(Vertex vertices[4], float pTimeStep) {
		if (sheetWidth == 0 || sheetHeight == 0) return;
		int32_t currentFrame, currentFrameX, currentFrameY;
		// Updates animation in loop mode.
		animFrame += pTimeStep * animSpeed;
		if (animLoop) {
			currentFrame = (animStartFrame + int32_t(animFrame) % animFrameCount);
		} else {
			// Updates animation in one-shot mode.
			if (animationEnded()) {
				currentFrame = animStartFrame + (animFrameCount-1);
			} else {
				currentFrame = animStartFrame + int32_t(animFrame);
			}
		}
		// Computes frame X and Y indexes from its id.
		currentFrameX = currentFrame % frameXCount;
		// currentFrameY is converted from OpenGL coordinates to top-left coordinates.
		currentFrameY = frameYCount - 1 - (currentFrame / frameXCount);
		// Draws selected frame.
		GLfloat posX1 = location.x - float(spriteWidth / 2);
		GLfloat posY1 = location.y - float(spriteHeight / 2);
		GLfloat posX2 = posX1 + spriteWidth;
		GLfloat posY2 = posY1 + spriteHeight;
		GLfloat u1 = GLfloat(currentFrameX * spriteWidth) / GLfloat(sheetWidth);
		GLfloat u2 = GLfloat((currentFrameX + 1) * spriteWidth) / GLfloat(sheetWidth);
		GLfloat v1 = GLfloat(currentFrameY * spriteHeight) / GLfloat(sheetHeight);
		GLfloat v2 = GLfloat((currentFrameY + 1) * spriteHeight) / GLfloat(sheetHeight);
		vertices[0].x = posX1; vertices[0].y = posY1;
		vertices[0].u = u1;    vertices[0].v = v1;		
		vertices[1].x = posX1; vertices[1].y = posY2;
		vertices[1].u = u1;    vertices[1].v = v2;
		vertices[2].x = posX2; vertices[2].y = posY1;
		vertices[2].u = u2;    vertices[2].v = v1;
		vertices[3].x = posX2; vertices[3].y = posY2;
		vertices[3].u = u2;    vertices[3].v = v2;
	}
private:
    const char* texturePath;
    GLuint textureId;
	Location location;
    // Frame.
    int32_t sheetHeight, sheetWidth;
    int32_t spriteHeight, spriteWidth;
    int32_t frameXCount, frameYCount, frameCount;
    // Animation.
    int32_t animStartFrame, animFrameCount;
    float animSpeed, animFrame;
    bool animLoop;
};

#endif