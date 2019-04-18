#ifndef __SHADER_H__
#define __SHADER_H__

#include "Resource.h"

class Shader {
private:
    GLuint programId;
public:
    Shader():
        programId(0),
        positionAttribLocation(0),
        texcoordAttribLocation(0),
        modelMatrixUniformLocation(0),
        cameraProjViewMatrixLocation(0) {
        //
    }
    ~Shader() {
        if (programId != 0) {
            glDeleteProgram(programId);
            programId = 0;
        }
    }
    status loadFromFile(const char* path) {
        Resource resource(path);
        LOG_INFO("Loading Shader: %s", resource.getPath());
        GLuint vertexShader, fragmentShader;
        GLint result;
        char infoLog[256];
        // Opens Shader file.
        if (resource.open() != STATUS_OK) {
            LOG_ERROR("Error open shader resource");
            resource.close();
            return STATUS_ERROR;
        }
        // Reads Shader file.
        GLint shaderLength = resource.getLength();
        char *shaderBuffer = new char[shaderLength];
        if (resource.read(shaderBuffer, shaderLength) != STATUS_OK) {
            LOG_ERROR("Error reading shader resource");
            resource.close();
            SAFE_DELETE_ARRAY(shaderBuffer);
            return STATUS_ERROR;
        }
        resource.close();
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
        SAFE_DELETE_ARRAY(shaderBuffer);
        // Builds the shader program.
        programId = glCreateProgram();
        glAttachShader(programId, vertexShader);
        glAttachShader(programId, fragmentShader);
        glLinkProgram(programId);
        glGetProgramiv(programId, GL_LINK_STATUS, &result);
        // Once linked, shaders are useless.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if (result == GL_FALSE) {
            glGetProgramInfoLog(programId, sizeof(infoLog), 0, infoLog);
            LOG_ERROR("Shader program error: %s", infoLog);
            goto ERROR;
        }
        return STATUS_OK;
ERROR:
        resource.close();
        SAFE_DELETE_ARRAY(shaderBuffer);
        if (vertexShader > 0) glDeleteShader(vertexShader);
        if (fragmentShader > 0) glDeleteShader(fragmentShader);
        return STATUS_ERROR;
    }
    void apply() {
        glUseProgram(programId);
    }	
    GLuint getProgramId() {
        return programId;
    }
    void SetUniform1f(const char* uniform_name, float u) {
        glUniform1f(glGetUniformLocation(programId, uniform_name), u);
    }
    void SetUniform1i(const char* uniform_name, int u) {
        glUniform1i(glGetUniformLocation(programId, uniform_name), u);
    }
    void SetUniform2f(const char* uniform_name, float u, float v) {
        glUniform2f(glGetUniformLocation(programId, uniform_name), u, v);
    }
    void SetUniform2i(const char* uniform_name, int u, int v) {
        glUniform2i(glGetUniformLocation(programId, uniform_name), u, v);
    }
    void SetUniform3f(const char* uniform_name, float u, float v, float w) {
        glUniform3f(glGetUniformLocation(programId, uniform_name), u, v, w);
    }
    void SetUniform3i(const char* uniform_name, int u, int v, int w) {
        glUniform3i(glGetUniformLocation(programId, uniform_name), u, v, w);
    }
    void SetUniform4f(const char* uniform_name, float u, float v, float w, float x) {
        glUniform4f(glGetUniformLocation(programId, uniform_name), u, v, w, x);
    }
    void SetUniform4i(const char* uniform_name, int u, int v, int w, int x) {
        glUniform4i(glGetUniformLocation(programId, uniform_name), u, v, w, x);
    }
    void SetUniformMatrix(const char* uniform_name, Matrix matrix) {
        glUniformMatrix4fv(glGetUniformLocation(programId, uniform_name), 1, false, matrix.data());
    }
    void SetUniformMatrix(int uniformIndex, Matrix& matrix) {
        glUniformMatrix4fv(uniformIndex, 1, false, matrix.data());
    }
    void getAttribAndUniformLocations() {
        positionAttribLocation = glGetAttribLocation(programId, "a_position");
        texcoordAttribLocation = glGetAttribLocation(programId, "a_texCoord");
        modelMatrixUniformLocation = glGetUniformLocation(programId, "u_modelMatrix");
        cameraProjViewMatrixLocation = glGetUniformLocation(programId, "u_cameraMatrix");
    }
    int positionAttribLocation;
    int texcoordAttribLocation;
    int modelMatrixUniformLocation;
    int cameraProjViewMatrixLocation;
};

#endif
