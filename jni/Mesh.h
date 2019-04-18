#ifndef __MESH_H__
#define __MESH_H__

#include <GLES2/gl2.h>
#include "Material.h"

struct Vertex {
    Vector2 position;
    Vector2 texCoords;
    Vertex() {}
    Vertex(const Vector2& pos) {
        position = pos;
    }
    Vertex(const Vector2& pos, const Vector2& texcoord) {
        position = pos;
        texCoords = texcoord;
    }
};

class Mesh {
private:
    GLuint vbo;			// vertex buffer
    int draws, stride;	// draw calls, stride
    Material *material;	// material
public:
    Mesh():
        draws(0),
        stride(0),
        vbo(0) {
        material = new Material();
    }
    ~Mesh() {
        glDeleteBuffers(1, &vbo);
        delete material;
    }
    void SetVertices(Vertex* vertices, int numVertices, const Vector2& scale) {
        float *verts = new float[numVertices * 4];
        for (int i = 0; i < numVertices; i++) {
            verts[i * 4 + 0] = vertices[i].position.x * scale.x;
            verts[i * 4 + 1] = vertices[i].position.y * scale.y;
            verts[i * 4 + 2] = vertices[i].texCoords.x;
            verts[i * 4 + 3] = vertices[i].texCoords.y;
        }
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, numVertices * 4 * sizeof(float), verts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        delete[] verts; // release verts
        draws = numVertices;
    }
    void SetMaterial(Material *mat) {
        material = mat;
    }
    Material *GetMaterial() {
        return material;
    }
    void Render(Shader *shader, int renderMode) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glEnableVertexAttribArray(shader->positionAttribLocation);
        glEnableVertexAttribArray(shader->texcoordAttribLocation);
        glVertexAttribPointer(shader->positionAttribLocation, 2, GL_FLOAT, false, stride, 0);
        glVertexAttribPointer(shader->texcoordAttribLocation, 2, GL_FLOAT, false, stride, (void*)(stride / 2));
        glDrawArrays(renderMode, 0, draws);
        glDisableVertexAttribArray(shader->positionAttribLocation);
        glDisableVertexAttribArray(shader->texcoordAttribLocation);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};

#endif
