#ifndef __SHADER_H__
#define __SHADER_H__

#include "Resource.h"

class Shader {
public:
    Shader(android_app* application, const char* path) :
            mResource(application, path) {
		//
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
            LOG_ERROR("Error open shader resource");            
            mResource.close();
            return STATUS_ERROR;
        }
        // Reads Shader file.
        GLint shaderLength = mResource.getLength();
        char *shaderBuffer = new char[shaderLength];
        if (mResource.read(shaderBuffer, shaderLength) != STATUS_OK) {
            LOG_ERROR("Error reading shader resource");  
            mResource.close();
            delete[] shaderBuffer;
            return STATUS_ERROR;                        
        }
        mResource.close();
        const char *shaderStrings[2] = {NULL, shaderBuffer};
        GLint stringsLengths[2] = {0, shaderLength};
        // Builds the vertex shader.
        shaderStrings[0] = "#define VERTEX\n";
        stringsLengths[0] = strlen(shaderStrings[0]);
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 2, shaderStrings, stringsLengths);
        glCompileShader(vertexShader);
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            glGetShaderInfoLog(vertexShader, sizeof(infoLog), 0, infoLog);
            LOG_ERROR("Vertex shader error: %s", infoLog);
            goto ERROR;
        }
        // Builds the fragment shader.
        shaderStrings[0] = "#define FRAGMENT\n";
        stringsLengths[0] = strlen(shaderStrings[0]);
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 2, shaderStrings, stringsLengths);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) {
            glGetShaderInfoLog(fragmentShader, sizeof(infoLog), 0, infoLog);
            LOG_ERROR("Fragment shader error: %s", infoLog);
            goto ERROR;
        }
        delete[] shaderBuffer;        
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
        delete[] shaderBuffer;
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
