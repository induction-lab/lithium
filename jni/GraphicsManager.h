#ifndef __GRAPHICSMANAGER_H__
#define __GRAPHICSMANAGER_H__

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <sys/system_properties.h>

#include "Singleton.h"
#include "Texture.h"
#include "Shader.h"

#include <map>
#include <vector>

const int DEFAULT_RENDER_WIDTH  = 360;

class GraphicsComponent {
public:
    virtual status load(void) = 0;
    virtual void draw(void) = 0;
    virtual ~GraphicsComponent(void) {};
};

class GraphicsManager: public Singleton<GraphicsManager> {
public:
    GraphicsManager():
        renderWidth(0), renderHeight(0),
        screenWidth(0), screenHeight(0),
        projectionMatrix(),
        components(),
        textures(),
        shaders(),
        screenFrameBuffer(0),
        renderFrameBuffer(0),
        renderVertexBuffer(0),
        renderTexture(0),
        renderShader(0),
        aPosition(0), aTexture(0), uTexture(0),
        display(EGL_NO_DISPLAY),
        surface(EGL_NO_CONTEXT),
        context(EGL_NO_SURFACE) {
        LOG_INFO("Creating GraphicsManager.");
    };
    ~GraphicsManager() {
        LOG_INFO("Destructing GraphicsManager.");
        reset();
    };
    int getRenderWidth() {
        return renderWidth;
    };
    int getRenderHeight() {
        return renderHeight;
    };
    int getScreenWidth() {
        return screenWidth;
    };
    int getScreenHeight() {
        return screenHeight;
    };
    Vector2 screenToRender(int x, int y) {
        float nx = x * ((float)renderWidth / (float)screenWidth);
        float ny = ((float)screenHeight - y) * ((float)renderHeight / (float)screenHeight);
        return Vector2(nx, ny);
    };
    status start() {
        LOG_INFO("Starting GraphicsManager.");
        EGLint format, numConfigs;
        EGLConfig config;
        EGLint majorVersion, minorVersion;
        // Detect if we are in emulator.
        char prop[PROP_VALUE_MAX];
        __system_property_get("ro.kernel.qemu", prop);
        bool in_emulator = strtol(prop, nullptr, 0) == 1;
        LOG_DEBUG("ro.kernel.qemu = %s", prop);
        if (in_emulator) LOG_INFO("In emulator! Enabling some hacks :(");
        // Defines display requirements. 16bits mode here.
        const EGLint attributes[] = {
            EGL_CONFIG_CAVEAT,          EGL_NONE,
            EGL_RENDERABLE_TYPE,        EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE,           EGL_WINDOW_BIT,
            EGL_BUFFER_SIZE,            16,
            EGL_RED_SIZE,               5,
            EGL_GREEN_SIZE,             6,
            EGL_BLUE_SIZE,              5,
            EGL_DEPTH_SIZE,             16,
            EGL_STENCIL_SIZE,           8,
            (in_emulator ? EGL_NONE : EGL_CONFORMANT), EGL_OPENGL_ES2_BIT,      // emulator does not support this, in here, but will return conformant anyway, what?
            (in_emulator ? EGL_NONE : EGL_COLOR_BUFFER_TYPE), EGL_RGB_BUFFER,   // emulator does not support this, what?
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
        // Create a context.
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);
        if (context == EGL_NO_CONTEXT) goto ERROR;
        // Activates the display surface.
        LOG_DEBUG("Activating the display.");
        if (!eglMakeCurrent(display, surface, surface, context)
                || !eglQuerySurface(display, surface, EGL_WIDTH, &screenWidth)
                || !eglQuerySurface(display, surface, EGL_HEIGHT, &screenHeight)
                || (screenWidth <= 0) || (screenHeight <= 0)) goto ERROR;
        // Set vsync.
        eglSwapInterval(display, 0);
        // Defines and initializes offscreen surface.
        if (initializeRenderBuffer() != STATUS_OK) goto ERROR;
        glViewport(0, 0, renderWidth, renderHeight);
        // Prepares the projection matrix.
        memset(projectionMatrix[0], 0, sizeof(projectionMatrix));
        projectionMatrix[0][0] =  2.0f / GLfloat(renderWidth);
        projectionMatrix[1][1] =  2.0f / GLfloat(renderHeight);
        projectionMatrix[2][2] = -1.0f;
        projectionMatrix[3][0] = -1.0f;
        projectionMatrix[3][1] = -1.0f;
        projectionMatrix[3][2] =  0.0f;
        projectionMatrix[3][3] =  1.0f;
        // Z-Buffer is useless as we are ordering draw calls ourselves.
        glDisable(GL_DEPTH_TEST);
        // Displays information about OpenGL.
        LOG_DEBUG("OpenGL render context information:");
        LOG_DEBUG("Renderer       : %s\n", (const char*)glGetString(GL_RENDERER));
        LOG_DEBUG("Vendor         : %s\n", (const char*)glGetString(GL_VENDOR));
        LOG_DEBUG("Version        : %s\n", (const char*)glGetString(GL_VERSION));
        LOG_DEBUG("GLSL version   : %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
        LOG_DEBUG("OpenGL version : %d.%d\n", majorVersion, minorVersion);
        LOG_DEBUG("Viewport       : %d x %d", screenWidth, screenHeight);
        LOG_DEBUG("Offscreen      : %d x %d", renderWidth, renderWidth);
        if (components.size() > 0) {
            if (loadResources() != STATUS_OK) goto ERROR;
        }
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error while starting GraphicsManager.");
        return STATUS_ERROR;
    };
    void stop() {
        LOG_INFO("Stopping GraphicsManager.");
        unloadResources();
        // Releases offscreen rendering resources.
        if (renderVertexBuffer != 0) {
            glDeleteBuffers(1, &renderVertexBuffer);
            renderVertexBuffer = 0;
        }
        if (renderFrameBuffer != 0) {
            glDeleteFramebuffers(1, &renderFrameBuffer);
            renderFrameBuffer = 0;
        }
        if (renderTexture != 0) {
            glDeleteTextures(1, &renderTexture);
            renderTexture = 0;
        }
        SAFE_DELETE(renderShader);        
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
    };
    status loadResources() {
        LOG_INFO("Loads graphics components.");
        // Loads graphics components.
        for (std::vector<GraphicsComponent*>::iterator it = components.begin(); it < components.end(); ++it) {
            if ((*it)->load() != STATUS_OK) return STATUS_ERROR;
        }
        return STATUS_OK;
    };
    void unloadResources() {
        // If already released.
        if (textures.size() == 0 && shaders.size() == 0) return;
        // Releases textures.
        LOG_DEBUG("Found %d textures.", textures.size());
        for (std::map<const char*, Texture*>::iterator it = textures.begin(); it != textures.end(); ++it) {
            SAFE_DELETE(it->second);
        };
        textures.clear();
        // Releases shaders.
        LOG_DEBUG("Found %d shaders.", shaders.size());
        for (std::map<const char*, Shader*>::iterator it = shaders.begin(); it != shaders.end(); ++it) {
            SAFE_DELETE(it->second);
        };
        shaders.clear();
    };
    void registerComponent(GraphicsComponent* component) {
        LOG_DEBUG("Register GraphicsComponent %d", components.size() + 1);
        components.push_back(component);
        component->load();
    };
    void reset() {
        unloadResources();
        // Releases graphics components.
        LOG_DEBUG("Delete %d graphic components.", components.size());
        for (std::vector<GraphicsComponent*>::iterator it = components.begin(); it < components.end(); ++it) {
            SAFE_DELETE(*it);
        }
        components.clear();
    };
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
        glBindTexture(GL_TEXTURE_2D, renderTexture);
        renderShader->apply();
        glUniform1i(uTexture, 0);
        // Indicates to OpenGL how position and uv coordinates are stored.
        glBindBuffer(GL_ARRAY_BUFFER, renderVertexBuffer);
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
            LOG_ERROR("Error %d swapping buffers.", eglGetError());
            return STATUS_ERROR;
        } else {
            return STATUS_OK;
        }
    };
    status initializeRenderBuffer() {
        LOG_INFO("Loading offscreen buffer.");
        const RenderVertex vertices[] = {
            {-1.0f, -1.0f, 0.0f, 0.0f },
            {-1.0f,  1.0f, 0.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f, 0.0f },
            { 1.0f,  1.0f, 1.0f, 1.0f }
        };
        float screenRatio = float(screenHeight) / float(screenWidth);
        renderWidth = DEFAULT_RENDER_WIDTH;
        renderHeight = float(renderWidth) * screenRatio;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &screenFrameBuffer);
        // Creates a texture for off-screen rendering.
        glGenTextures(1, &renderTexture);
        glBindTexture(GL_TEXTURE_2D, renderTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderWidth, renderHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
        // Attaches the texture to the new framebuffer.
        glGenFramebuffers(1, &renderFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, renderFrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // Creates the vertex buffer.
        renderVertexBuffer = initVertexBuffer(vertices, sizeof(vertices));
        if (renderVertexBuffer == 0) goto ERROR;
        // Creates the shader used to render texture to screen.
        renderShader = new Shader();
        if (renderShader->loadFromFile("shaders/Render.shader") != STATUS_OK) goto ERROR;
        // Creates and retrieves shader attributes and uniforms.
        aPosition = glGetAttribLocation(renderShader->getProgramId(),"aPosition");
        aTexture  = glGetAttribLocation(renderShader->getProgramId(), "aTexture");
        uTexture  = glGetUniformLocation(renderShader->getProgramId(),"uTexture");
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error while loading offscreen buffer.");
        return STATUS_ERROR;
    };
    Texture* loadTexture(const char* path, int filter, int mode) {
        // Reuse texture, if already loaded.
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
    };
    Shader* loadShader(const char* path) {
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
    };
    GLuint initVertexBuffer(const void* buffer, int bufferSize) {
        GLuint vertexBuffer;
        // Upload specified memory buffer into OpenGL.
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, buffer, GL_STATIC_DRAW);
        // Unbinds the buffer.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (glGetError() != GL_NO_ERROR) goto ERROR;
        return vertexBuffer;
ERROR:
        LOG_ERROR("Error loading vertex buffer.");
        if (vertexBuffer > 0) glDeleteBuffers(1, &vertexBuffer);
        return 0;
    };
    GLfloat* getProjectionMatrix() {
        return projectionMatrix[0];
    };
private:
    struct RenderVertex {
        GLfloat x, y, u, v;
    };
    // Display properties.
    int renderWidth;
    int renderHeight;
    int screenWidth;
    int screenHeight;
    GLfloat projectionMatrix[4][4];
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    // Graphics resources.
    std::vector<GraphicsComponent*> components;
    std::map<const char*, Texture*> textures;
    std::map<const char*, Shader*> shaders;
    // Rendering resources.
    GLint screenFrameBuffer;
    GLuint renderFrameBuffer;
    GLuint renderVertexBuffer;
    GLuint renderTexture;
    Shader* renderShader;
    GLuint aPosition, aTexture, uTexture;
};

#endif //  __GRAPHICSMANAGER_H__
