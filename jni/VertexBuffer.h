#ifndef __VERTEX_BUFFER_H__
#define __VERTEX_BUFFER_H__

class VertexBuffer {
private:
    GLuint bufferId;
	void* buffer;
	int32_t size;
public:
    VertexBuffer():
        bufferId(0),
        buffer(NULL),
        size(0) {
        //
    }
    ~VertexBuffer() {
        if (bufferId != 0) {
            glDeleteBuffers(1, &bufferId);
			LOG_DEBUG("Vertex buffer id:%d is dead.", bufferId);			
            bufferId = 0;
        }		
    }
    status createFromData(const void* buffer, int32_t bufferSize) {
        LOG_DEBUG("Create %d x 32 bits sized vertex buffer.", bufferSize);
        // Upload specified memory buffer into OpenGL.
        glGenBuffers(1, &bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glBufferData(GL_ARRAY_BUFFER, size, buffer, GL_STATIC_DRAW);
        // Unbinds the buffer.
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (glGetError() != GL_NO_ERROR) goto ERROR;		
		LOG_DEBUG("Vertex buffer id:%d is available.", bufferId);
        return STATUS_OK;
ERROR:
        LOG_ERROR("Error creating vertex buffer.");
        return STATUS_ERROR;
    }
	GLuint getBufferId() {
		return bufferId;
	}
};

#endif
