#ifndef __GRAPHICSMANAGER_H__
#define __GRAPHICSMANAGER_H__

#include <android_native_app_glue.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "Texture.h"
#include "Shader.h"

#include <vector>

const int32_t DEFAULT_RENDER_WIDTH  = 360;

class GraphicsComponent {
public:
    virtual status load() = 0;
    virtual void draw() = 0;
};

class GraphicsManager {
public:
    GraphicsManager(android_app* pApplication) :
            mApplication(pApplication),
			mRenderWidth(0), mRenderHeight(0),
			mScreenWidth(0), mScreenHeight(0),
			mProjectionMatrix(),
            mComponents(),
            mTextures(),
            mShaders(),
			mScreenFrameBuffer(0),
			mRenderFrameBuffer(0),
			mRenderVertexBuffer(0),
			mRenderTexture(0),
			mRenderShaderProgram(0),
			aPosition(0),
			aTexture(0),
			uTexture(0),
            mDisplay(EGL_NO_DISPLAY),
            mSurface(EGL_NO_CONTEXT),
            mContext(EGL_NO_SURFACE) {
        LOG_INFO("Creating GraphicsManager");
    }
    ~GraphicsManager() {
        LOG_INFO("Destructing GraphicsManager");
		mComponents.clear();
    }
    int32_t getRenderWidth() { return mRenderWidth; }
    int32_t getRenderHeight() { return mRenderHeight; }
    int32_t getScreenWidth() { return mScreenWidth; }
    int32_t getScreenHeight() { return mScreenHeight; }
    status start() {
        LOG_INFO("Starting GraphicsManager");
        EGLint format, numConfigs, result;
        EGLConfig config;
        EGLint majorVersion, minorVersion;
        // Defines display requirements. 16bits mode here.
        const EGLint attributes[] = {
            EGL_LEVEL,           0,
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE,       5,
            EGL_GREEN_SIZE,      6,
            EGL_RED_SIZE,        5,
            EGL_DEPTH_SIZE,      16,
            EGL_NONE
        };
        // Request an OpenGL ES 2 context.
        const EGLint contextAttributes[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };
        // Retrieves a display connection and initializes it.
        LOG_DEBUG("Connecting to the display.");
        mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (mDisplay == EGL_NO_DISPLAY) goto ERROR;
        if (!eglInitialize(mDisplay, &majorVersion, &minorVersion)) goto ERROR;
        // Selects the first OpenGL configuration found.
        LOG_DEBUG("Selecting a display config.");
        if (!eglChooseConfig(mDisplay, attributes, &config, 1, &numConfigs) || (numConfigs <= 0)) goto ERROR;
        // Reconfigures the Android window with the EGL format.
        LOG_DEBUG("Configuring window format.");
        if (!eglGetConfigAttrib(mDisplay, config, EGL_NATIVE_VISUAL_ID, &format)) goto ERROR;
        ANativeWindow_setBuffersGeometry(mApplication->window, 0, 0, format);
        // Creates the display surface.
        LOG_DEBUG("Initializing the display.");
        mSurface = eglCreateWindowSurface(mDisplay, config, mApplication->window, NULL);
        if (mSurface == EGL_NO_SURFACE) goto ERROR;
        // Create a context
        mContext = eglCreateContext(mDisplay, config, EGL_NO_CONTEXT, contextAttributes);
        if (mContext == EGL_NO_CONTEXT) goto ERROR;
        // Activates the display surface.
        LOG_DEBUG("Activating the display.");
        if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext)
                || !eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &mScreenWidth)
                || !eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &mScreenHeight)
                || (mScreenWidth <= 0) || (mScreenHeight <= 0)) goto ERROR;
		LOG_INFO("Screen dimensions: %d x %d", mScreenWidth, mScreenHeight);				
		// Defines and initializes offscreen surface.
		if (initializeRenderBuffer() != STATUS_OK) goto ERROR;
		glViewport(0, 0, mRenderWidth, mRenderHeight);
		// Prepares the projection matrix.
		memset(mProjectionMatrix[0], 0, sizeof(mProjectionMatrix));
		mProjectionMatrix[0][0] =  2.0f / GLfloat(mRenderWidth);
		mProjectionMatrix[1][1] =  2.0f / GLfloat(mRenderHeight);
		mProjectionMatrix[2][2] = -1.0f;
		mProjectionMatrix[3][0] = -1.0f;
		mProjectionMatrix[3][1] = -1.0f;
		mProjectionMatrix[3][2] =  0.0f;
		mProjectionMatrix[3][3] =  1.0f;		
		// Z-Buffer is useless as we are ordering draw calls ourselves.
		glDisable(GL_DEPTH_TEST);
        // Displays information about OpenGL.
        LOG_INFO("OpenGL render context information:\n"
                 "Renderer       : %s\n"
                 "Vendor         : %s\n"
                 "Version        : %s\n"
                 "GLSL version   : %s\n"
                 "OpenGL version : %d.%d\n",
                 (const char*)glGetString(GL_RENDERER),
                 (const char*)glGetString(GL_VENDOR),
                 (const char*)glGetString(GL_VERSION),
                 (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION),
                 majorVersion, minorVersion
                );
		return STATUS_OK;
	ERROR:
        LOG_ERROR("Error while starting GraphicsManager");
        stop();
        return STATUS_ERROR;
    }
	status loadResources() {
		LOG_INFO("GraphicsManager: Loads graphics components");
		// Loads graphics components.
		for (std::vector<GraphicsComponent*>::iterator componentIt = mComponents.begin(); componentIt < mComponents.end(); ++componentIt) {
			if ((*componentIt)->load() != STATUS_OK) return STATUS_ERROR;
		}		
	}
    void stop() {
        LOG_INFO("Stopping GraphicsManager");
        // Releases textures.
        for (std::vector<Texture*>::iterator textureIt = mTextures.begin(); textureIt < mTextures.end(); ++textureIt) {
			(*textureIt)->unload();
			delete (*textureIt);
		};
		mTextures.clear();
        // Releases shaders.        
		for (std::vector<Shader*>::iterator shaderIt = mShaders.begin(); shaderIt < mShaders.end(); ++shaderIt) {
			(*shaderIt)->unload();
			delete (*shaderIt);
		};
		mShaders.clear();
		// Releases vertex buffers.
		for (std::vector<GLuint>::iterator vertexBufferIt = mVertexBuffers.begin(); vertexBufferIt < mVertexBuffers.end(); ++vertexBufferIt) {
			glDeleteBuffers(1, &(*vertexBufferIt));
		};
		mVertexBuffers.clear();
		// Releases offscreen rendering resources.
		// Vertex buffer are released by the loops above.
		if (mRenderFrameBuffer != 0) {
			glDeleteFramebuffers(1, &mRenderFrameBuffer);
			mRenderFrameBuffer = 0;
		}
		if (mRenderTexture != 0) {
			glDeleteTextures(1, &mRenderTexture);
			mRenderTexture = 0;
		}
		if (mRenderShaderProgram != 0) {
			glDeleteProgram(mRenderShaderProgram);
			mRenderShaderProgram = 0;
		}		
        // Destroys OpenGL context.
        if (mDisplay != EGL_NO_DISPLAY) {
            eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (mContext != EGL_NO_CONTEXT) {
                eglDestroyContext(mDisplay, mContext);
                mContext = EGL_NO_CONTEXT;
            }
            if (mSurface != EGL_NO_SURFACE) {
                eglDestroySurface(mDisplay, mSurface);
                mSurface = EGL_NO_SURFACE;
            }
            eglTerminate(mDisplay);
            mDisplay = EGL_NO_DISPLAY;
        }
    }
    status update() {
		// Uses the offscreen FBO for scene rendering.
		glBindFramebuffer(GL_FRAMEBUFFER, mRenderFrameBuffer);
		glViewport(0, 0, mRenderWidth, mRenderHeight);
		glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// Render graphic components.
		std::vector<GraphicsComponent*>::iterator componentIt;
		for (componentIt = mComponents.begin(); componentIt < mComponents.end(); ++componentIt) {
			(*componentIt)->draw();
		}
		// The FBO is rendered and scaled into the screen.
		glBindFramebuffer(GL_FRAMEBUFFER, mScreenFrameBuffer);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, mScreenWidth, mScreenHeight);
		// Select the offscreen texture as source.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mRenderTexture);
		glUseProgram(mRenderShaderProgram);
		glUniform1i(uTexture, 0);
		// Indicates to OpenGL how position and uv coordinates are stored.
		glBindBuffer(GL_ARRAY_BUFFER, mRenderVertexBuffer);
		glEnableVertexAttribArray(aPosition);
		glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (GLvoid*) 0);
		glEnableVertexAttribArray(aTexture);
		glVertexAttribPointer(aTexture, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (GLvoid*) (sizeof(GLfloat) * 2));
		// Renders the offscreen buffer into screen.
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// Restores device state.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Shows the result to the user.
		if (eglSwapBuffers(mDisplay, mSurface) != EGL_TRUE) {
			LOG_ERROR("Error %d swapping buffers.", eglGetError());
			return STATUS_ERROR;
		} else {
			return STATUS_OK;
		}		
    }
	status initializeRenderBuffer() {
		LOG_INFO("Loading offscreen buffer");
		const RenderVertex vertices[] = {
			{-1.0f, -1.0f, 0.0f, 0.0f },
			{-1.0f,  1.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f, 0.0f },
			{ 1.0f,  1.0f, 1.0f, 1.0f }
		};
		float screenRatio = float(mScreenHeight) / float(mScreenWidth);
		mRenderWidth = DEFAULT_RENDER_WIDTH;
		mRenderHeight = float(mRenderWidth) * screenRatio;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &mScreenFrameBuffer);
		// Creates a texture for off-screen rendering.
		glGenTextures(1, &mRenderTexture);
		glBindTexture(GL_TEXTURE_2D, mRenderTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mRenderWidth, mRenderHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
		// Attaches the texture to the new framebuffer.
		glGenFramebuffers(1, &mRenderFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, mRenderFrameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderTexture, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Creates the vertex buffer
		mRenderVertexBuffer = loadVertexBuffer(vertices, sizeof(vertices));
		if (mRenderVertexBuffer == 0) goto ERROR;
		// Creates the shader used to render texture to screen.
		Shader* shader;
		shader = loadShader("shaders/Render.shader");
		if (shader == NULL) goto ERROR;
		mRenderShaderProgram = shader->getProgram();
		// Creates and retrieves shader attributes and uniforms.
		aPosition = glGetAttribLocation(mRenderShaderProgram,"aPosition");
		aTexture = glGetAttribLocation(mRenderShaderProgram, "aTexture");
		uTexture = glGetUniformLocation(mRenderShaderProgram,"uTexture");
		LOG_INFO("Render dimensions: %d x %d", mRenderWidth, mRenderHeight);		
		return STATUS_OK;
	ERROR:
		LOG_ERROR("Error while loading offscreen buffer");
		return STATUS_ERROR;
	}
    void registerComponent(GraphicsComponent* pComponent) {
        mComponents.push_back(pComponent);
    }
    Texture* loadTexture(const char* pPath) {
		// Finds out if texture already loaded.
		for (std::vector<Texture*>::iterator TextureIt = mTextures.begin(); TextureIt < mTextures.end(); ++TextureIt) {
			if (strcmp(pPath, (*TextureIt)->getPath()) == 0) return (*TextureIt);
        }
        // Appends a new texture to the texture array.
		Texture* texture = new Texture(mApplication, pPath);
		if (texture->load() != STATUS_OK) goto ERROR;
		mTextures.push_back(texture);
		return texture;
	ERROR:
		delete texture;
		return NULL;
    }
    Shader* loadShader(const char* pPath) {
		// Finds out if shader already loaded.
		for (std::vector<Shader*>::iterator shaderIt = mShaders.begin(); shaderIt < mShaders.end(); ++shaderIt) {
			if (strcmp(pPath, (*shaderIt)->getPath()) == 0) return (*shaderIt);
		}
        // Appends a new shader to the shader array.
        Shader* shader = new Shader(mApplication, pPath);
		if (shader->load() != STATUS_OK) goto ERROR;
        mShaders.push_back(shader);
        return shader;
	ERROR:
		delete shader;
		return NULL;
    }
	GLuint loadVertexBuffer(const void* pVertexBuffer, int32_t pVertexBufferSize) {
		GLuint vertexBuffer;
		// Upload specified memory buffer into OpenGL.
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, pVertexBufferSize, pVertexBuffer, GL_STATIC_DRAW);
		// Unbinds the buffer.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		if (glGetError() != GL_NO_ERROR) goto ERROR;
		mVertexBuffers.push_back(vertexBuffer);
		return vertexBuffer;
	ERROR:
		LOG_ERROR("Error loading vertex buffer.");
		if (vertexBuffer > 0) glDeleteBuffers(1, &vertexBuffer);
		return 0;
	}
	GLfloat* getProjectionMatrix() {
		return mProjectionMatrix[0];
	}
private:
    android_app* mApplication;
    struct RenderVertex {
        GLfloat x, y, u, v;
    };	
    // Display properties.
    int32_t mRenderWidth; int32_t mRenderHeight;
    int32_t mScreenWidth; int32_t mScreenHeight;
	GLfloat mProjectionMatrix[4][4];
    EGLDisplay mDisplay;
    EGLSurface mSurface;
    EGLContext mContext;
    // Graphics resources.
    std::vector<GraphicsComponent*> mComponents;
    std::vector<Texture*> mTextures;
    std::vector<Shader*> mShaders;
	std::vector<GLuint> mVertexBuffers;
    // Rendering resources.
    GLint mScreenFrameBuffer;
    GLuint mRenderFrameBuffer;
	GLuint mRenderVertexBuffer;
    GLuint mRenderTexture;
	GLuint mRenderShaderProgram;
    GLuint aPosition;
	GLuint aTexture;
	GLuint uTexture;
};

#endif
