#ifndef __GRAPHICSMANAGER_H__
#define __GRAPHICSMANAGER_H__

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "Singleton.h"
#include "Texture.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "Material.h"
#include "Mesh.h"

#include <map>
#include <vector>
#include <algorithm>

const int32_t DEFAULT_RENDER_WIDTH  = 480;
const int32_t DEFAULT_RENDER_HEIGHT = 480;

class GraphicsComponent {
public:
    virtual status initialize() = 0;
    virtual void draw() = 0;
    virtual ~GraphicsComponent() {};
};

class GraphicsManager: public Singleton<GraphicsManager> {
private:
    // Display properties.
    int32_t screenWidth;
    int32_t screenHeight;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    // Graphics resources.
    std::vector<GraphicsComponent*> components;
    std::map<const char*, Texture*> textures;
    std::map<const char*, Shader*> shaders;
    std::vector<VertexBuffer*> vertexBuffers;
    // Rendering resources.
    GLint screenFrameBuffer;
    GLuint renderFrameBuffer;
    struct RenderVertex {
        GLfloat x, y, u, v;
    };	
    Texture renderTexture;
    Shader renderShader;
	VertexBuffer renderVertexBuffer;
    GLuint aPosition, aTexture, uTexture;	
    GLfloat projectionMatrix[4][4];	
    int32_t renderWidth;
    int32_t renderHeight;	
	bool isInitializedFlag;
public:
    GraphicsManager():
		isInitializedFlag(false),
        screenWidth(0), screenHeight(0),
        display(EGL_NO_DISPLAY),
        surface(EGL_NO_CONTEXT),
        context(EGL_NO_SURFACE) {
        LOG_DEBUG("Creating GraphicsManager.");
    }
    ~GraphicsManager() {
        LOG_DEBUG("Destructing GraphicsManager.");
        if (renderFrameBuffer != 0) {
            glDeleteFramebuffers(1, &renderFrameBuffer);
            renderFrameBuffer = 0;
        }
		clear();
        // Destroys OpenGL context.
        if (display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (context != EGL_NO_CONTEXT) {
                eglDestroyContext(display, context);
                context = EGL_NO_CONTEXT;
            }
            if (surface != EGL_NO_SURFACE) {
                eglDestroySurface(display, surface);
                surface = EGL_NO_SURFACE;
            }
            eglTerminate(display);
            display = EGL_NO_DISPLAY;
        }
    }
    status initialize() {
        LOG_DEBUG("Starting GraphicsManager.");
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
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (display == EGL_NO_DISPLAY) goto ERROR;
        if (!eglInitialize(display, &majorVersion, &minorVersion)) goto ERROR;
        // Selects the first OpenGL configuration found.
        LOG_DEBUG("Selecting a display config.");
        if (!eglChooseConfig(display, attributes, &config, 1, &numConfigs) || (numConfigs <= 0)) goto ERROR;
        // Reconfigures the Android window with the EGL format.
        LOG_DEBUG("Configuring window format.");
        if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) goto ERROR;
        ANativeWindow_setBuffersGeometry(application->window, 0, 0, format);
        // Creates the display surface.
        LOG_DEBUG("Initializing the display.");
        surface = eglCreateWindowSurface(display, config, application->window, NULL);
        if (surface == EGL_NO_SURFACE) goto ERROR;
        // Create a context
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);
        if (context == EGL_NO_CONTEXT) goto ERROR;
        // Activates the display surface.
        LOG_DEBUG("Activating the display.");
        if (!eglMakeCurrent(display, surface, surface, context)
                || !eglQuerySurface(display, surface, EGL_WIDTH, &screenWidth)
                || !eglQuerySurface(display, surface, EGL_HEIGHT, &screenHeight)
                || (screenWidth <= 0) || (screenHeight <= 0)) goto ERROR;
        // Set vsync.
        // eglSwapInterval(display, 0);
        // Displays information about OpenGL.
        LOG_DEBUG("OpenGL render context information:\n"
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
        LOG_DEBUG("Screen dimensions: %d x %d", screenWidth, screenHeight);
		if (!isInitializedFlag) {
			if (createRenderBuffer(DEFAULT_RENDER_WIDTH, DEFAULT_RENDER_HEIGHT) != STATUS_OK) return STATUS_ERROR;
			isInitializedFlag = true;
		}
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error while starting GraphicsManager.");
        return STATUS_ERROR;
    }
    void suspend() {
		LOG_DEBUG("Stopping GraphicsManager.");
        if (display != EGL_NO_DISPLAY) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
            surface = EGL_NO_SURFACE;
        }
    }
    status update() {
        // Uses the offscreen FBO for scene rendering.
        glBindFramebuffer(GL_FRAMEBUFFER, renderFrameBuffer);
        glViewport(0, 0, renderWidth, renderHeight);
        glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // Render graphic components.
        for (std::vector<GraphicsComponent*>::iterator it = components.begin(); it < components.end(); ++it) {
            (*it)->draw();
        }
        // The FBO is rendered and scaled into the screen.
        glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, screenWidth, screenHeight);
        // Select the offscreen texture as source.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderTexture.getTextureId());
        renderShader.apply();
        glUniform1i(uTexture, 0);
        // Indicates to OpenGL how position and uv coordinates are stored.
        glBindBuffer(GL_ARRAY_BUFFER, renderVertexBuffer.getBufferId());
        glEnableVertexAttribArray(aPosition);
        glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (GLvoid*) 0);
        glEnableVertexAttribArray(aTexture);
        glVertexAttribPointer(aTexture, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (GLvoid*) (sizeof(GLfloat) * 2));
        // Renders the offscreen buffer into screen.
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // Restores device state.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Shows the result to the user.
        if (eglSwapBuffers(display, surface) != EGL_TRUE) {
			EGLint error = eglGetError();
            if (error == EGL_BAD_NATIVE_WINDOW) {
				if (application->window != NULL) {
					suspend();
					initialize();
				}
			} else {
				LOG_ERROR("Error %d swapping buffers.", eglGetError());
				return STATUS_ERROR;
			}			
        }
		return STATUS_OK;		
    }
    int32_t getScreenWidth() {
        return screenWidth;
    }
    int32_t getScreenHeight() {
        return screenHeight;
    }
    /*
    Location renderToScreen(int x, int y) {
        float nx = x * ((float)renderWidth / (float)screenWidth);
        float ny = ((float)screenHeight - y) * ((float)renderHeight / (float)screenHeight);
        return Location(nx, ny);
    }
    */	
    void clear() {
        // Releases graphics components.
        for (std::vector<GraphicsComponent*>::iterator it = components.begin(); it < components.end(); ++it) {
            SAFE_DELETE(*it);
        }
        components.clear();
        // Releases textures.
        for (std::map<const char*, Texture*>::iterator it = textures.begin(); it != textures.end(); ++it) {
            SAFE_DELETE(it->second);
        };
        textures.clear();
        // Releases shaders.
        for (std::map<const char*, Shader*>::iterator it = shaders.begin(); it != shaders.end(); ++it) {
            SAFE_DELETE(it->second);
        };
        shaders.clear();
        // Releases vertex buffers.
        for (std::vector<VertexBuffer*>::iterator it = vertexBuffers.begin(); it < vertexBuffers.end(); ++it) {
            SAFE_DELETE(*it);
        };
        vertexBuffers.clear();
    }	
    status createRenderBuffer(int width, int height) {
        LOG_DEBUG("Loading offscreen buffer.");
        const RenderVertex vertices[] = {
            {-1.0f, -1.0f, 0.0f, 0.0f },
            {-1.0f,  1.0f, 0.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f, 0.0f },
            { 1.0f,  1.0f, 1.0f, 1.0f }
        };
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &screenFrameBuffer);
        // Creates a texture for off-screen rendering.
        if (renderTexture.createFromData(NULL, width, height, GL_RGB, GL_LINEAR, GL_CLAMP_TO_EDGE) != STATUS_OK) goto ERROR;
        // Attaches the texture to the new framebuffer.
        glGenFramebuffers(1, &renderFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, renderFrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture.getTextureId(), 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Creates the shader used to render texture to screen.
        if (renderShader.loadFromFile("shaders/Render.shader") != STATUS_OK) goto ERROR;
        // Creates and retrieves shader attributes and uniforms.
        aPosition = glGetAttribLocation(renderShader.getProgramId(),"a_Position");
        aTexture  = glGetAttribLocation(renderShader.getProgramId(), "a_Texture");
        uTexture  = glGetUniformLocation(renderShader.getProgramId(),"u_Texture");
        // Creates the vertex buffer
        if (renderVertexBuffer.createFromData(vertices, sizeof(vertices)) != STATUS_OK) goto ERROR;
        LOG_DEBUG("Render dimensions: %d x %d", width, height);
        glViewport(0, 0, width, height);
        renderWidth = width;
        renderHeight = height;
        // Prepares the projection matrix.
        memset(projectionMatrix[0], 0, sizeof(projectionMatrix));
        projectionMatrix[0][0] =  2.0f / GLfloat(width);
        projectionMatrix[1][1] =  2.0f / GLfloat(height);
        projectionMatrix[2][2] = -1.0f;
        projectionMatrix[3][0] = -1.0f;
        projectionMatrix[3][1] = -1.0f;
        projectionMatrix[3][2] =  0.0f;
        projectionMatrix[3][3] =  1.0f;
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error while loading offscreen buffer");
        return STATUS_ERROR;
    }
    void registerComponent(GraphicsComponent* component) {
        if (component->initialize() != STATUS_OK) return;
        components.push_back(component);
    }
    Texture* addTexture(const char* path, int filter, int mode) {
        // Reuse texture, if already loaded
        std::map<const char*, Texture*>::iterator it = textures.find(path);
        if (it != textures.end()) return it->second;
        // Appends a new texture to the texture map.
        Texture* texture = new Texture();
        if (texture->loadFromFile(path, filter, mode) != STATUS_OK) goto ERROR;
        textures.insert(std::pair<const char*, Texture*>(path, texture));
        return texture;
ERROR:
        SAFE_DELETE(texture);
        return NULL;
    }
    Shader* addShader(const char* path) {
        // Finds out if shader already loaded.
        std::map<const char*, Shader*>::iterator it = shaders.find(path);
        if (it != shaders.end()) return it->second;
        // Appends a new shader to the shader map.
        Shader* shader = new Shader();
        if (shader->loadFromFile(path) != STATUS_OK) goto ERROR;
        shaders.insert(std::pair<const char*, Shader*>(path, shader));
        return shader;
ERROR:
        SAFE_DELETE(shader);
        return NULL;
    }
    VertexBuffer* addVertexBuffer(const void* buffer, int32_t bufferSize) {
        // Appends a new vertex buffer.
		VertexBuffer* vertexBuffer = new VertexBuffer();
        if (vertexBuffer->createFromData(buffer, bufferSize) != STATUS_OK) goto ERROR;
        vertexBuffers.push_back(vertexBuffer);
        return vertexBuffer;
ERROR:
        SAFE_DELETE(vertexBuffer)
        return NULL;
    }
    GLfloat* getProjectionMatrix() {
        return projectionMatrix[0];
    }
};

#endif
