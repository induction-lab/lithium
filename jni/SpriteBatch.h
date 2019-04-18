#ifndef __SPRITEBATCH_H__
#define __SPRITEBATCH_H__

#include "GraphicsManager.h"
#include "TimeManager.h"

/// @see https://github.com/SFML/SFML/wiki/Source:-AnimatedSprite

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
    status load() {
		Texture* texture = GraphicsManager::getInstance().loadTexture(texturePath);
		textureId = texture->getId();
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

class SpriteBatch : public GraphicsComponent {
public:
    SpriteBatch():
		sprites(), vertices(), indexes(),
		shaderProgram(0),
		aPosition(-1), aTexture(-1), uProjection(-1), uTexture(-1) {
		GraphicsManager::getInstance().registerComponent(this);
	}
    ~SpriteBatch() {
		for (std::vector<Sprite*>::iterator it = sprites.begin(); it < sprites.end(); ++it) {
			delete (*it);
		}
		sprites.clear();		
	}
    Sprite* registerSprite(const char* texturePath, int32_t width, int32_t height) {
		int32_t spriteCount = sprites.size();
		int32_t index = spriteCount * 4; // Points to 1st vertex.
		// Precomputes the index buffer.
		indexes.push_back(index+0); indexes.push_back(index+1);
		indexes.push_back(index+2); indexes.push_back(index+2);
		indexes.push_back(index+1); indexes.push_back(index+3);
		for (int i = 0; i < 4; ++i) {
			vertices.push_back(Sprite::Vertex());
		}
		// Appends a new sprite to the sprite array.
		Sprite* sprite = new Sprite(texturePath, width, height);
		sprites.push_back(sprite);
		return sprite;
	}
    status load() {
		// Creates and retrieves shader attributes and uniforms.
		Shader* shader = GraphicsManager::getInstance().loadShader("shaders/Sprite.shader");
		shaderProgram = shader->getProgram();
		aPosition = glGetAttribLocation(shaderProgram, "aPosition");
		aTexture = glGetAttribLocation(shaderProgram, "aTexture");
		uProjection = glGetUniformLocation(shaderProgram, "uProjection");
		uTexture = glGetUniformLocation(shaderProgram, "u_texture");
		// Loads sprites.
		for (std::vector<Sprite*>::iterator it = sprites.begin(); it < sprites.end(); ++it) {
			if ((*it)->load() != STATUS_OK) goto ERROR;
		}
		return STATUS_OK;
	ERROR:
		LOG_ERROR("Error loading sprite batch");
		return STATUS_ERROR;
	}
    void draw() {
		// Selects sprite shader and passes its parameters.
		glUseProgram(shaderProgram);
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, GraphicsManager::getInstance().getProjectionMatrix());
		glUniform1i(uTexture, 0);
		// Indicates to OpenGL how position and uv coordinates are stored.
		glEnableVertexAttribArray(aPosition);
		glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Sprite::Vertex), &(vertices[0].x));
		glEnableVertexAttribArray(aTexture);
		glVertexAttribPointer(aTexture, 2, GL_FLOAT, GL_FALSE, sizeof(Sprite::Vertex), &(vertices[0].u));
		// Activates transparency.
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Renders all sprites in batch.
		const int32_t vertexPerSprite = 4;
		const int32_t indexPerSprite = 6;
		float timeStep = TimeManager::getInstance().getFrameElapsedTime();
		int32_t spriteCount = sprites.size();
		int32_t currentSprite = 0, firstSprite = 0;
		while (bool canDraw = (currentSprite < spriteCount)) {
			// Switches texture.
			Sprite* sprite = sprites[currentSprite];
			GLuint currentTextureId = sprite->textureId;
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, sprite->textureId);
			// Generate sprite vertices for current textures.
			do {
				sprite = sprites[currentSprite];
				if (sprite->textureId == currentTextureId) {
					Sprite::Vertex* spriteVertices = (&vertices[currentSprite * 4]);
					sprite->draw(spriteVertices, timeStep);
				} else {
					break;
				}
			} while (canDraw == (++currentSprite < spriteCount));
			// Renders sprites each time texture changes.
			glDrawElements(GL_TRIANGLES, (currentSprite - firstSprite) * indexPerSprite, GL_UNSIGNED_SHORT, &indexes[firstSprite * indexPerSprite]);
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
    std::vector<Sprite*> sprites;
    std::vector<Sprite::Vertex> vertices;
    std::vector<GLushort> indexes;
    GLuint shaderProgram;
    GLuint aPosition; GLuint aTexture;
    GLuint uProjection; GLuint uTexture;
};

#endif