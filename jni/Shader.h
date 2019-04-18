#ifndef __SHADER_H__
#define __SHADER_H__

#include "Resource.h"

class Shader {
public:
    Shader(android_app* pApplication, const char* pPath) :
            mResource(pApplication, pPath) {
    }
    const char* getPath() {
        return mResource.getPath();
    }
    status load() {
        LOG_INFO("Loading Shader: %s", mResource.getPath());
        GLuint vertexShader, fragmentShader;
        GLint result;
        char infoLog[256];
        // Opens Shader file.
        if (mResource.open() != STATUS_OK) {
            LOG_ERROR("Error open shader resource.");            
            mResource.close();
            return STATUS_ERROR;
        }
        // Reads Shader file.
        GLint lShaderLength = mResource.getLength();
        char *lShaderBuffer = new char[lShaderLength];
        if (mResource.read(lShaderBuffer, lShaderLength) != STATUS_OK) {
            LOG_ERROR("Error loading shader.");            
            mResource.close();
            delete[] lShaderBuffer;
            return STATUS_ERROR;                        
        }
        mResource.close();
        const char *lShaderStrings[2] = {NULL, lShaderBuffer};
        GLint lStringsLengths[2] = {0, lShaderLength};
        // Builds the vertex shader.
        lShaderStrings[0] = "#define VERTEX\n";
        lStringsLengths[0] = strlen(lShaderStrings[0]);
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 2, lShaderStrings, lStringsLengths);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            glGetShaderInfoLog(vertexShader, sizeof(infoLog), 0, infoLog);
            LOG_ERROR("Vertex shader error: %s", infoLog);
            goto ERROR;
        }
        // Builds the fragment shader.
        lShaderStrings[0] = "#define FRAGMENT\n";
        lStringsLengths[0] = strlen(lShaderStrings[0]);
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 2, lShaderStrings, lStringsLengths);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            glGetShaderInfoLog(fragmentShader, sizeof(infoLog), 0, infoLog);
            LOG_ERROR("Fragment shader error: %s", infoLog);
            goto ERROR;
        }
        delete[] lShaderBuffer;        
        // Builds the shader program.
        mProgram = glCreateProgram();
        glAttachShader(mProgram, vertexShader);
        glAttachShader(mProgram, fragmentShader);
        glLinkProgram(mProgram);
        glGetProgramiv(mProgram, GL_LINK_STATUS, &result);
        // Once linked, shaders are useless.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if (result == GL_FALSE) {
            glGetProgramInfoLog(mProgram, sizeof(infoLog), 0, infoLog);
            LOG_ERROR("Shader program error: %s", infoLog);
            goto ERROR;
        }
        return STATUS_OK;
ERROR:
        mResource.close();
        delete[] lShaderBuffer;
        if (vertexShader > 0) glDeleteShader(vertexShader);
        if (fragmentShader > 0) glDeleteShader(fragmentShader);
        return STATUS_ERROR;
    }
    status unload() {
        if (mProgram != 0) {
            glDeleteProgram(mProgram);
            mProgram = 0;
        }
        return STATUS_OK;
    }
	GLuint getProgram() {
		return mProgram;
	}
private:
    friend class GraphicService;
private:
    Resource mResource;
    GLuint mProgram;
};

#endif
