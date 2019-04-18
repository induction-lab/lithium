#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <string>
#include <cassert>
#include <vector>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>


class Geometry;
class Shader;
class Texture;

class Renderable {
private:
    Geometry*	m_pGeometry;
    Shader*		m_pShader;
public:
    Renderable():
        m_pGeometry(NULL),
        m_pShader(NULL) {
    }
    ~Renderable() {}
    void SetGeometry(Geometry* pGeometry) {
        m_pGeometry = pGeometry;
    }
    Geometry* GetGeometry() {
        return m_pGeometry;
    }
    void SetShader(Shader* pShader) {
        m_pShader = pShader;
    }
    Shader* GetShader() {
        return m_pShader;
    }
};

class Texture {
public:
    struct Header {
        unsigned int	m_width;
        unsigned int	m_height;
        unsigned int	m_bytesPerPixel;
        unsigned int	m_dataSize;
        Header():
            m_width(0),
            m_height(0),
            m_bytesPerPixel(0),
            m_dataSize(0) {

        }
        Header(const Header& header) {
            m_width			= header.m_width;
            m_height		= header.m_height;
            m_bytesPerPixel = header.m_bytesPerPixel;
            m_dataSize		= header.m_dataSize;
        }
    };
private:
    GLuint		m_id;
    Header		m_header;
    void*		m_pImageData;
public:
    Texture(): m_id(GL_INVALID_VALUE) {}
    ~Texture() {}
    void SetData(Header& header, void* pImageData) {
        m_header		= header;
        m_pImageData	= pImageData;
    }
    void Init() {
        GLint	packBits		= 4;
        GLint	internalFormat	= GL_RGBA;
        GLenum	format			= GL_RGBA;
        switch (m_header.m_bytesPerPixel) {
        case 1: {
            packBits		= 1;
            internalFormat	= GL_ALPHA;
            format			= GL_ALPHA;
        }
        break;
        };
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, packBits);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_header.m_width, m_header.m_height, 0, format, GL_UNSIGNED_BYTE, m_pImageData);
    }
};

class Shader {
private:
    void LoadShader(GLuint id, std::string& shaderCode) {
        static const uint32_t NUM_SHADERS = 1;
        const GLchar* pCode = shaderCode.c_str();
        GLint length = shaderCode.length();
        glShaderSource(id, NUM_SHADERS, &pCode, &length);
        glCompileShader(id);
        glAttachShader(m_programId, id);
    }
protected:
    GLuint			m_vertexShaderId;
    GLuint			m_fragmentShaderId;
    GLint			m_programId;
    std::string		m_vertexShaderCode;
    std::string		m_fragmentShaderCode;
    bool			m_isLinked;
public:
    Shader():
        m_vertexShaderId(GL_INVALID_VALUE),
        m_fragmentShaderId(GL_INVALID_VALUE),
        m_programId(GL_INVALID_VALUE) {
    }
    ~Shader() {}
    void Link() {
        m_programId = glCreateProgram();
        m_vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
        LoadShader(m_vertexShaderId, m_vertexShaderCode);
        m_fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
        LoadShader(m_fragmentShaderId, m_fragmentShaderCode);
        glLinkProgram(m_programId);
        m_isLinked = true;
    }
    void Setup(Renderable& renderable) {
        glUseProgram(m_programId);
    }
};

class Geometry {
private:
    static const unsigned int NAME_MAX_LENGTH = 16;
    char		m_name[NAME_MAX_LENGTH];
    int			m_numVertices;
    int			m_numIndices;
    void*		m_pVertices;
    void*		m_pIndices;
    int			m_numVertexPositionElements;
    int			m_numTexCoordElements;
    int			m_vertexStride;
public:
    Geometry():
        m_numVertices(0),
        m_numIndices(0),
        m_pVertices(NULL),
        m_pIndices(NULL),
        m_numVertexPositionElements(0),
        m_numTexCoordElements(0),
        m_vertexStride(0) {
    }
    ~Geometry() {}
    void SetName(const char* name) {
        strcpy(m_name, name);
    }
    void SetNumVertices(const int numVertices) {
        m_numVertices = numVertices;
    }
    void SetNumIndices(const int numIndices) {
        m_numIndices = numIndices;
    }
    const char* GetName() const {
        return m_name;
    }
    const int GetNumVertices() const {
        return m_numVertices;
    }
    const int GetNumIndices() const {
        return m_numIndices;
    }
    void* GetVertexBuffer() const {
        return m_pVertices;
    }
    void* GetIndexBuffer() const {
        return m_pIndices;
    }
    void SetVertexBuffer(void* pVertices) {
        m_pVertices = pVertices;
    }
    void SetIndexBuffer(void* pIndices) {
        m_pIndices = pIndices;
    }
    void SetNumVertexPositionElements(const int numVertexPositionElements) {
        m_numVertexPositionElements = numVertexPositionElements;
    }
    int GetNumVertexPositionElements() const {
        return m_numVertexPositionElements;
    }
    void SetNumTexCoordElements(const int numTexCoordElements) {
        m_numTexCoordElements = numTexCoordElements;
    }
    int	GetNumTexCoordElements() const {
        return m_numTexCoordElements;
    }
    void SetVertexStride(const int vertexStride) {
        m_vertexStride = vertexStride;
    }
    int GetVertexStride() const {
        return m_vertexStride;
    }
};

class Renderer: public Task {
private:
    android_app*	m_pState;
    EGLDisplay		m_display;
    EGLContext		m_context;
    EGLSurface		m_surface;
    int				m_width;
    int				m_height;
    bool			m_initialized;
    typedef std::vector<Shader*>		ShaderVector;
    typedef ShaderVector::iterator		ShaderVectorIterator;
    typedef std::vector<Texture*>		TextureVector;
    typedef TextureVector::iterator		TextureVectorIterator;
    typedef std::vector<Renderable*>	RenderableVector;
    typedef RenderableVector::iterator	RenderableVectorIterator;
    RenderableVector	m_renderables;
    TextureVector		m_textures;
    ShaderVector		m_shaders;
    void Draw(Renderable* pRenderable) {
        assert(pRenderable);
        if (pRenderable) {
            Geometry* pGeometry = pRenderable->GetGeometry();
            Shader* pShader = pRenderable->GetShader();
            assert(pShader && pGeometry);
            if (pShader && pGeometry) {
                pShader->Setup(*pRenderable);
                glDrawElements(GL_TRIANGLES, pGeometry->GetNumIndices(), GL_UNSIGNED_SHORT, pGeometry->GetIndexBuffer());
            }
        }
    }
public:
    explicit Renderer(android_app* pState, const unsigned int priority):
        Task(priority),
        m_display(EGL_NO_DISPLAY),
        m_context(EGL_NO_CONTEXT),
        m_surface(EGL_NO_SURFACE),
        m_width(0.0f),
        m_height(0.0f) {
		LOG_INFO("Creating Renderer"); 
        m_pState = pState;
        m_renderables.reserve(16);
    }
    ~Renderer() {
		LOG_INFO("Destructing Renderer");
	};
    void Init() {
		LOG_INFO("Initialize Renderer");
        // initialize OpenGL ES and EGL
        const EGLint attribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
        };
        EGLint format;
        EGLint numConfigs;
        EGLConfig config;
        m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(m_display, NULL, NULL);
        eglChooseConfig(m_display, attribs, &config, 1, &numConfigs);
        eglGetConfigAttrib(m_display, config, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(m_pState->window, 0, 0, format);
        m_surface = eglCreateWindowSurface(m_display, config, m_pState->window, NULL);
        EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };
        m_context = eglCreateContext(m_display, config, NULL, contextAttribs);
        eglMakeCurrent(m_display, m_surface, m_surface, m_context);
        eglQuerySurface(m_display, m_surface, EGL_WIDTH, &m_width);
        eglQuerySurface(m_display, m_surface, EGL_HEIGHT, &m_height);
        for (TextureVectorIterator iter = m_textures.begin(); iter != m_textures.end(); ++iter) {
            Texture* pCurrent = *iter;
            pCurrent->Init();
        }
        for (ShaderVectorIterator iter = m_shaders.begin(); iter != m_shaders.end(); ++iter) {
            Shader* pCurrent = *iter;
            pCurrent->Link();
        }
        m_initialized = true;
    }
    void Destroy() {
        m_initialized = false;
        if (m_display != EGL_NO_DISPLAY) {
            eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (m_context != EGL_NO_CONTEXT) {
                eglDestroyContext(m_display, m_context);
            }
            if (m_surface != EGL_NO_SURFACE) {
                eglDestroySurface(m_display, m_surface);
            }
            eglTerminate(m_display);
        }
        m_display = EGL_NO_DISPLAY;
        m_context = EGL_NO_CONTEXT;
        m_surface = EGL_NO_SURFACE;
    }
    void AddRenderable(Renderable* pRenderable) {
        m_renderables.push_back(pRenderable);
    }
    void RemoveRenderable(Renderable* pRenderable) {
        for (RenderableVectorIterator iter = m_renderables.begin(); iter != m_renderables.end(); ++iter) {
            Renderable* pCurrent = *iter;
            if (pCurrent == pRenderable) {
                m_renderables.erase(iter);
                break;
            }
        }
    }
    void AddShader(Shader* pShader) {
        assert(pShader);
        if (m_initialized) {
            pShader->Link();
        }
        m_shaders.push_back(pShader);
    }
    void RemoveShader(Shader* pShader) {
        for (ShaderVectorIterator iter = m_shaders.begin(); iter != m_shaders.end(); ++iter) {
            Shader* pCurrent = *iter;
            if (pCurrent == pShader) {
                m_shaders.erase(iter);
                break;
            }
        }
    }
    void AddTexture(Texture* pTexture) {
        assert(pTexture);
        if (m_initialized) pTexture->Init();
        m_textures.push_back(pTexture);
    }
    void RemoveTexture(Texture* pTexture) {
        for (TextureVectorIterator iter = m_textures.begin(); iter != m_textures.end(); ++iter) {
            Texture* pCurrent = *iter;
            if (pCurrent == pTexture) {
                m_textures.erase(iter);
                break;
            }
        }
    }
    bool Start() {
        return true;
    }
    void OnSuspend() {}
    void Update() {
        if (m_initialized) {
            glClearColor(0.95f, 0.95f, 0.95f, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            for (RenderableVectorIterator iter = m_renderables.begin(); iter != m_renderables.end(); ++iter) {
                Renderable* pRenderable = *iter;
                if (pRenderable) {
                    Draw(pRenderable);
                }
            }
            eglSwapBuffers(m_display, m_surface);
        }
    }
    void OnResume() {}
    void Stop() {}
    bool IsInitialized() {
        return m_initialized;
    }
};


#endif
