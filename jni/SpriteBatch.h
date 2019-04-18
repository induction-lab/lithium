#ifndef __SPRITEBATCH_H__
#define __SPRITEBATCH_H__

#include <vector>

#include "GraphicsManager.h"
#include "TimeManager.h"
#include "Sprite.h"

class SpriteBatch: public GraphicsComponent {
public:
    SpriteBatch():
        sprites(), vertices(), indexes(),
        shaderProgram(0),
        aPosition(0), aTexture(0), uProjection(0), uTexture(0), uColor(0), uOpaque(0) {
        GraphicsManager::getInstance()->registerComponent(this);
    };
    ~SpriteBatch() {
        for (std::vector<Sprite*>::iterator it = sprites.begin(); it < sprites.end(); ++it) {
            delete (*it);
        }
        sprites.clear();
    };
    Sprite* registerSprite(const char* texturePath, int32_t width, int32_t height) {
        int32_t spriteCount = sprites.size();
        // Points to 1st vertex.
        int32_t index = spriteCount * 4;
        // Precomputes the index buffer.
        indexes.push_back(index+0);
        indexes.push_back(index+1);
        indexes.push_back(index+2);
        indexes.push_back(index+2);
        indexes.push_back(index+1);
        indexes.push_back(index+3);
        for (int i = 0; i < 4; ++i) {
            vertices.push_back(Sprite::Vertex());
        }
        // Appends a new sprite to the sprite array.
        Sprite* sprite = new Sprite(texturePath, width, height);
        sprites.push_back(sprite);
        sprite->load();
        return sprite;
    };
    status load() {
        // Creates and retrieves shader attributes and uniforms.
        Shader* shader = GraphicsManager::getInstance()->loadShader("shaders/Sprite.shader");
        shaderProgram = shader->getProgramId();
        aPosition = glGetAttribLocation(shaderProgram, "aPosition");
        aTexture = glGetAttribLocation(shaderProgram, "aTexture");
        uProjection = glGetUniformLocation(shaderProgram, "uProjection");
        uTexture = glGetUniformLocation(shaderProgram, "uTexture");
        uColor = glGetUniformLocation(shaderProgram, "uColor");
        uOpaque = glGetUniformLocation(shaderProgram, "uOpaque");
        // Loads sprites.
        for (std::vector<Sprite*>::iterator it = sprites.begin(); it < sprites.end(); ++it) {
            if ((*it)->load() != STATUS_OK) goto ERROR;
        }
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error loading sprite batch");
        return STATUS_ERROR;
    };
    void draw() {
        // Selects sprite shader and passes its parameters.
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(uProjection, 1, GL_FALSE, GraphicsManager::getInstance()->getProjectionMatrix());
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
        float timeStep = TimeManager::getInstance()->getFrameElapsedTime();
        int32_t spriteCount = sprites.size();
        int32_t currentSprite = 0, firstSprite = 0;
        while (bool canDraw = (currentSprite < spriteCount)) {
            // Switches texture.
            Sprite* sprite = sprites[currentSprite];
            GLuint currentTextureId = sprite->textureId;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, sprite->textureId);
            // Sprite color & opaque.
            Vector currentColor = sprite->color;
            float currentOpaque = sprite->opaque;
            glUniform3fv(uColor, 1, sprite->color.data());
            glUniform1fv(uOpaque, 1, &sprite->opaque);
            // Generate sprite vertices for current textures.
            do {
                sprite = sprites[currentSprite];
                if (
                    sprite->color == currentColor &&
                    sprite->opaque == currentOpaque &&
                    sprite->textureId == currentTextureId
                ) {
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
    };
private:
    std::vector<Sprite*> sprites;
    std::vector<Sprite::Vertex> vertices;
    std::vector<GLushort> indexes;
    GLuint shaderProgram;
    GLuint aPosition, aTexture, uProjection, uTexture, uColor, uOpaque;
};

#endif // __SPRITEBATCH_H__