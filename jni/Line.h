#ifndef __LINE_H__
#define __LINE_H__

/* Simple OpenGL rounded corner line */

#include <vector>

class Line: public GraphicsComponent {
public:
    Line(float thickness):
        thickness(thickness),
        color(Vector(1.0f, 1.0f, 1.0f)),
        opaque(1.0f),
        shaderProgram(0),
        aPosition(0), uProjection(0), uColor(0), uOpaque(0) {
        LOG_DEBUG("Create Line.");
        GraphicsManager::getInstance()->registerComponent(this);
    };
    ~Line() {
        LOG_DEBUG("Delete Line.");
        glDeleteBuffers(1, &vbo);
    }
    status load() {
        Shader* shader = GraphicsManager::getInstance()->loadShader("shaders/Line.shader");
        shaderProgram = shader->getProgramId();
        aPosition = glGetAttribLocation(shaderProgram, "aPosition");
        uProjection = glGetUniformLocation(shaderProgram, "uProjection");
        uColor = glGetUniformLocation(shaderProgram, "uColor");
        uOpaque = glGetUniformLocation(shaderProgram, "uOpaque");
        return STATUS_OK;
    };
    void draw() {
        if (vertices.size() < 6) return;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(uProjection, 1, GL_FALSE, GraphicsManager::getInstance()->getProjectionMatrix());
        glEnableVertexAttribArray(aPosition);
        glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glUniform3fv(uColor, 1, color.data());
        glUniform1fv(uOpaque, 1, &opaque);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glUseProgram(0);
        glDisableVertexAttribArray(aPosition);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    };
    void addPoint(Vector v) {
        LOG_DEBUG("Add line point at %f %f", v.x, v.y);
        points.push_back(v);
        vertices.clear();
        for(int i = 0; i < points.size(); ++i) {
            int a = ((i-1) < 0) ? 0 : (i-1);
            int b = i;
            int c = ((i+1) >= points.size()) ? points.size()-1 : (i+1);
            int d = ((i+2) >= points.size()) ? points.size()-1 : (i+2);
            drawSegment(points[a], points[b], points[c], points[d]);
        }
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * 2 * sizeof(float), &vertices[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    };
    void drawSegment(Vector p0, Vector p1, Vector p2, Vector p3) {
        // Skip if zero length.
        if (p1 == p2) return;
        // 1) Define the line between the two points.
        Vector line = Normalize(p2 - p1);
        // 2) Find the normal vector of this line.
        Vector normal = Normalize(Vector(-line.y, line.x, 0.0f));
        // 3) Find the tangent vector at both the end points:
        //	  - if there are no segments before or after this one, use the line itself.
        //    - otherwise, add the two normalized lines and average them by normalizing again.
        Vector tangent1 = (p0 == p1) ? line : Normalize(Normalize(p1 - p0) + line);
        Vector tangent2 = (p2 == p3) ? line : Normalize(Normalize(p3 - p2) + line);
        // 4) Find the miter line, which is the normal of the tangent.
        Vector miter1 = Vector(-tangent1.y, tangent1.x, 0.0f);
        Vector miter2 = Vector(-tangent2.y, tangent2.x, 0.0f);
        // Find length of miter by projecting the miter onto the normal,
        // take the length of the projection, invert it and multiply it by the thickness:
        // length = thickness * ( 1 / |normal|.|miter| )
        float length1 = thickness / (normal | miter1);
        float length2 = thickness / (normal | miter2);
        // Finally, draw segment.
		vertices.push_back(Vector2(p1 - length1 * miter1));
        vertices.push_back(Vector2(p2 - length2 * miter2));
		vertices.push_back(Vector2(p1 + length1 * miter1));
		vertices.push_back(Vector2(p1 + length1 * miter1));
        vertices.push_back(Vector2(p2 + length2 * miter2));
        vertices.push_back(Vector2(p2 - length2 * miter2));
    };
    float thickness;
    Vector color;
    float opaque;
private:
    std::vector<Vector> points;
    std::vector<Vector2> vertices;
    GLuint shaderProgram;
    GLuint aPosition, uProjection, uColor, uOpaque;
	GLuint vbo; // vertex buffer
};

#endif // __LINE_H__
