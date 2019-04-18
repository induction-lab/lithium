#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "GraphicsManager.h"
#include "TimeManager.h"

class SpriteBatch; 

class Sprite {
public:
    struct Vertex {
        GLfloat x, y, u, v;
    };
    Sprite(GraphicsManager* pGraphicsManager, const char* pTexturePath, int32_t pHeight, int32_t pWidth) :
		mLocation(),
		mTexturePath(pTexturePath), mTexture(0),
		mSheetWidth(0), mSheetHeight(0),
		mSpriteHeight(pHeight), mSpriteWidth(pWidth),
		mFrameCount(0), mFrameXCount(0), mFrameYCount(0),
		mAnimStartFrame(0), mAnimFrameCount(1),
		mAnimSpeed(0), mAnimFrame(0), mAnimLoop(false) {}	
    void setAnimation(int32_t pStartFrame, int32_t pFrameCount, float pSpeed, bool pLoop) {
		mAnimStartFrame = pStartFrame;
		mAnimFrame = 0.0f, mAnimSpeed = pSpeed, mAnimLoop = pLoop;
		mAnimFrameCount = pFrameCount;
	}
    bool animationEnded() { return mAnimFrame > (mAnimFrameCount-1); }
    void setLocation(Location pLocation) { mLocation = pLocation; }
	Location getLocation() { return mLocation; }
	int32_t getSpriteHeight() { return mSpriteHeight; }
	int32_t getSpriteWidth() { return mSpriteWidth; }
protected:
    friend class SpriteBatch;
    status load(GraphicsManager* pGraphicsManager) {
		Texture* texture = pGraphicsManager->loadTexture(mTexturePath);
		if (texture->load() != STATUS_OK) return STATUS_ERROR;
		mTexture = texture->getId();
		mSheetWidth = texture->getWidth();
		mSheetHeight = texture->getHeight();
		mFrameXCount = mSheetWidth / mSpriteWidth;
		mFrameYCount = mSheetHeight / mSpriteHeight;
		mFrameCount = (mSheetHeight / mSpriteHeight) * (mSheetWidth / mSpriteWidth);
		LOG_INFO("Loaded sprite with %d feame count", mFrameCount);
		loadStatus = STATUS_OK;
		return STATUS_OK;
	}
    void draw(Vertex pVertices[4], float pTimeStep) {
		if (mSheetWidth == 0 || mSheetHeight == 0) return;
		int32_t currentFrame, currentFrameX, currentFrameY;
		// Updates animation in loop mode.
		mAnimFrame += pTimeStep * mAnimSpeed;
		if (mAnimLoop) {
			currentFrame = (mAnimStartFrame + int32_t(mAnimFrame) % mAnimFrameCount);
		} else {
			// Updates animation in one-shot mode.
			if (animationEnded()) {
				currentFrame = mAnimStartFrame + (mAnimFrameCount-1);
			} else {
				currentFrame = mAnimStartFrame + int32_t(mAnimFrame);
			}
		}
		// Computes frame X and Y indexes from its id.
		currentFrameX = currentFrame % mFrameXCount;
		// currentFrameY is converted from OpenGL coordinates to top-left coordinates.
		currentFrameY = mFrameYCount - 1 - (currentFrame / mFrameXCount);
		// Draws selected frame.
		GLfloat posX1 = mLocation.x - float(mSpriteWidth / 2);
		GLfloat posY1 = mLocation.y - float(mSpriteHeight / 2);
		GLfloat posX2 = posX1 + mSpriteWidth;
		GLfloat posY2 = posY1 + mSpriteHeight;
		GLfloat u1 = GLfloat(currentFrameX * mSpriteWidth) / GLfloat(mSheetWidth);
		GLfloat u2 = GLfloat((currentFrameX + 1) * mSpriteWidth) / GLfloat(mSheetWidth);
		GLfloat v1 = GLfloat(currentFrameY * mSpriteHeight) / GLfloat(mSheetHeight);
		GLfloat v2 = GLfloat((currentFrameY + 1) * mSpriteHeight) / GLfloat(mSheetHeight);
		pVertices[0].x = posX1; pVertices[0].y = posY1;
		pVertices[0].u = u1;    pVertices[0].v = v1;		
		pVertices[1].x = posX1; pVertices[1].y = posY2;
		pVertices[1].u = u1;    pVertices[1].v = v2;
		pVertices[2].x = posX2; pVertices[2].y = posY1;
		pVertices[2].u = u2;    pVertices[2].v = v1;
		pVertices[3].x = posX2; pVertices[3].y = posY2;
		pVertices[3].u = u2;    pVertices[3].v = v2;
	}
private:
	status loadStatus = STATUS_NOT_LOADED;
    const char* mTexturePath;
    GLuint mTexture;
	Location mLocation;
    // Frame.
    int32_t mSheetHeight, mSheetWidth;
    int32_t mSpriteHeight, mSpriteWidth;
    int32_t mFrameXCount, mFrameYCount, mFrameCount;
    // Animation.
    int32_t mAnimStartFrame, mAnimFrameCount;
    float mAnimSpeed, mAnimFrame;
    bool mAnimLoop;
};

class SpriteBatch : public GraphicsComponent {
public:
    SpriteBatch(TimeManager* pTimeManager, GraphicsManager* pGraphicsManager) :
		mTimeManager(pTimeManager),
		mGraphicsManager(pGraphicsManager),
		mSprites(), mVertices(), mIndexes(),
		mShaderProgram(0),
		aPosition(-1), aTexture(-1), uProjection(-1), uTexture(-1) {
		mGraphicsManager->registerComponent(this);
	}
    ~SpriteBatch() {
		std::vector<Sprite*>::iterator spriteIt;
		for (spriteIt = mSprites.begin(); spriteIt < mSprites.end(); ++spriteIt) {
			delete (*spriteIt);
		}
	} 
    Sprite* registerSprite(const char* pTexturePath, int32_t pHeight, int32_t pWidth) {
		int32_t spriteCount = mSprites.size();
		int32_t index = spriteCount * 4; // Points to 1st vertex.
		// Precomputes the index buffer.
		mIndexes.push_back(index+0); mIndexes.push_back(index+1);
		mIndexes.push_back(index+2); mIndexes.push_back(index+2);
		mIndexes.push_back(index+1); mIndexes.push_back(index+3);
		for (int i = 0; i < 4; ++i) {
			mVertices.push_back(Sprite::Vertex());
		}
		// Appends a new sprite to the sprite array.
		Sprite* sprite = new Sprite(mGraphicsManager, pTexturePath, pHeight, pWidth);
		mSprites.push_back(sprite);
		return sprite;
	}
    status load() {
		// Creates and retrieves shader attributes and uniforms.
		Shader* shader = mGraphicsManager->loadShader("shaders/Sprite.shader");
		if (shader->load() != STATUS_OK) goto ERROR;
		mShaderProgram = shader->getProgram();
		aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
		aTexture = glGetAttribLocation(mShaderProgram, "aTexture");
		uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
		uTexture = glGetUniformLocation(mShaderProgram, "u_texture");
		// Loads sprites.
		for (std::vector<Sprite*>::iterator spriteIt = mSprites.begin(); spriteIt < mSprites.end(); ++spriteIt) {
			if ((*spriteIt)->load(mGraphicsManager) != STATUS_OK) goto ERROR;
		}
		loadStatus = STATUS_OK;
		return STATUS_OK;
	ERROR:
		LOG_ERROR("Error loading sprite batch");
		return STATUS_ERROR;
	}
    void draw() {
		// Selects sprite shader and passes its parameters.
		glUseProgram(mShaderProgram);
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, mGraphicsManager->getProjectionMatrix());
		glUniform1i(uTexture, 0);
		// Indicates to OpenGL how position and uv coordinates are stored.
		glEnableVertexAttribArray(aPosition);
		glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Sprite::Vertex), &(mVertices[0].x));
		glEnableVertexAttribArray(aTexture);
		glVertexAttribPointer(aTexture, 2, GL_FLOAT, GL_FALSE, sizeof(Sprite::Vertex), &(mVertices[0].u));
		// Activates transparency.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Renders all sprites in batch.
		const int32_t vertexPerSprite = 4;
		const int32_t indexPerSprite = 6;
		float timeStep = mTimeManager->elapsed();
		int32_t spriteCount = mSprites.size();
		int32_t currentSprite = 0, firstSprite = 0;
		while (bool canDraw = (currentSprite < spriteCount)) {
			// Switches texture.
			Sprite* sprite = mSprites[currentSprite];
			if (sprite->loadStatus == STATUS_NOT_LOADED) sprite->load(mGraphicsManager);
			GLuint currentTexture = sprite->mTexture;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, sprite->mTexture);
			// Generate sprite vertices for current textures.
			do {
				sprite = mSprites[currentSprite];
				if (sprite->mTexture == currentTexture) {
					Sprite::Vertex* vertices = (&mVertices[currentSprite * 4]);
					sprite->draw(vertices, timeStep);
				} else {
					break;
				}
			} while (canDraw = (++currentSprite < spriteCount));
			// Renders sprites each time texture changes.
			glDrawElements(GL_TRIANGLES, (currentSprite - firstSprite) * indexPerSprite, GL_UNSIGNED_SHORT, &mIndexes[firstSprite * indexPerSprite]);
			firstSprite = currentSprite;
		}
		// Cleans up OpenGL state.
		glUseProgram(0);
		glDisableVertexAttribArray(aPosition);
		glDisableVertexAttribArray(aTexture);
		glDisable(GL_BLEND);
	}
private:
    SpriteBatch(const SpriteBatch&);
    void operator=(const SpriteBatch&);
    TimeManager* mTimeManager;
    GraphicsManager* mGraphicsManager;
    std::vector<Sprite*> mSprites;
    std::vector<Sprite::Vertex> mVertices;
    std::vector<GLushort> mIndexes;
    GLuint mShaderProgram;
    GLuint aPosition; GLuint aTexture;
    GLuint uProjection; GLuint uTexture;
};

#endif