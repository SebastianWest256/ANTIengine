#include "render.hpp"
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 700;
void basic_setupTriangle(GLuint& VAO, GLuint& VBO, GLenum drawType) {
    float vertices[] = {

        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, drawType);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void basic_drawTriangle(GLuint& VAO) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
void batch_setupTriangles(GLuint& VAO, GLuint& VBO, const std::vector<float>& vertices, GLenum drawType) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), drawType);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}
void batch_drawTriangles(GLuint& VAO, int vertexCount) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}
void instance_setupTriangle(GLuint& VAO, GLuint& VBO, GLuint& instanceVBO, glm::vec2* positions, int numInstances, GLenum drawType) {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, drawType);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numInstances, positions, drawType);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void instance_drawTriangle(GLuint& VAO, int numInstances) {
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, numInstances);
    glBindVertexArray(0);
}
void basic_setupCircle(GLuint& VAO, GLuint& VBO, float radius, int segments, GLenum drawType) {
    std::vector<float> vertices;
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), drawType);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void basic_drawCircle(GLuint& VAO, int segments) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);
    glBindVertexArray(0);
}
void batch_setupCircles(GLuint& VAO, GLuint& VBO, const std::vector<CircleParams>& circles, int segments, GLenum drawType) {
    std::vector<float> vertices;
    for (const auto& circle : circles) {
        vertices.push_back(circle.centerX);
        vertices.push_back(circle.centerY);
        vertices.push_back(0.0f);
        for (int i = 0; i <= segments; i++) {
            float theta = 2.0f * M_PI * float(i) / float(segments);
            float x = circle.centerX + circle.radius * cosf(theta);
            float y = circle.centerY + circle.radius * sinf(theta);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(0.0f);
        }
    }
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), drawType);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void batch_drawCircles(GLuint& VAO, int segments, int circleCount) {
    glBindVertexArray(VAO);
    for (int c = 0; c < circleCount; c++) {
        glDrawArrays(GL_TRIANGLE_FAN, c * (segments + 2), segments + 2);
    }
    glBindVertexArray(0);
}
void setupCircleInstances(GLuint& VAO, GLuint& VBO, GLuint& instanceVBO,
    float radius, int segments,
    const glm::vec2* positions, int numInstances, GLenum drawType) {
    std::vector<float> vertices;
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
    }
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), drawType);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numInstances, positions, drawType);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void instance_drawCircle(GLuint& VAO, int segments, int numInstances) {
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, segments + 2, numInstances);
    glBindVertexArray(0);
}

void batch_setupSquares(GLuint& VAO, GLuint& VBO, const std::vector<float>& vertices, GLenum drawType) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), drawType);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}
void batch_drawSquares(GLuint& VAO, int vertexCount) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}
void instance_setupSquare(GLuint& VAO, GLuint& VBO, GLuint& instanceVBO, glm::vec2* positions, int numInstances, GLenum drawType) {
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, drawType);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numInstances, positions, drawType);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void instance_drawSquare(GLuint& VAO, int numInstances) {
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, numInstances);
    glBindVertexArray(0);
}
void setupLine(GLuint& VAO, GLuint& VBO, const glm::vec2& start, const glm::vec2& end, GLenum drawType) {
    float vertices[] = {
        start.x, start.y, 0.0f,
        end.x, end.y, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, drawType);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void drawLine(GLuint& VAO) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}
void setup_line(line* line) {
    float w_ratio = float(SCREEN_HEIGHT) / float(SCREEN_WIDTH);
    setupLine(line->VAO, line->VBO, glm::vec2(0, 0), glm::vec2((line->x1 - line->x0) * w_ratio, line->y1 - line->y0), line->drawType);
}
void draw_line(line line, camera camera) {
    float w_ratio = float(SCREEN_HEIGHT) / float(SCREEN_WIDTH);
    *camera.trans = glm::translate(glm::mat4(1.0f), glm::vec3(((line.x0 * w_ratio) - camera.x) * camera.zoom, (line.y0 - camera.y) * camera.zoom, line.z0));
    *camera.trans = glm::scale(*camera.trans, glm::vec3(camera.zoom, camera.zoom, 1.0f));
    glUniformMatrix4fv(*camera.transformLoc, 1, GL_FALSE, glm::value_ptr(*camera.trans));
    drawLine(line.VAO);
}
void basic_setupRect(GLuint& VAO, GLuint& VBO, GLenum drawType, float vertices[]) {
    float rect_vertices[18];
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), vertices, drawType);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void basic_drawRect(GLuint& VAO) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
void setup_rect(rect* rect) {

    float vertices[18] = {};

    float w_ratio = float(SCREEN_HEIGHT) / float(SCREEN_WIDTH);

    vertices[0] = (-rect->w / 2) * w_ratio;
    vertices[1] = -rect->h / 2;
    vertices[3] = (rect->w / 2) * w_ratio;
    vertices[4] = -rect->h / 2;
    vertices[6] = (-rect->w / 2) * w_ratio;
    vertices[7] = rect->h / 2;
    vertices[9] = (rect->w / 2) * w_ratio;
    vertices[10] = -rect->h / 2;
    vertices[12] = (rect->w / 2) * w_ratio;
    vertices[13] = rect->h / 2;
    vertices[15] = (-rect->w / 2) * w_ratio;
    vertices[16] = rect->h / 2;

    basic_setupRect(rect->VAO, rect->VBO, rect->drawType, vertices);

}
void draw_rect(rect rect, camera camera) {

    float w_ratio = float(camera.SCREEN_HEIGHT) / float(camera.SCREEN_WIDTH);

    *camera.trans = glm::translate(glm::mat4(1.0f), glm::vec3(((rect.x * w_ratio) - camera.x) * camera.zoom, (rect.y - camera.y) * camera.zoom, rect.z));
    *camera.trans = glm::scale(*camera.trans, glm::vec3(camera.zoom, camera.zoom, 1.0f));
    glUniformMatrix4fv(*camera.transformLoc, 1, GL_FALSE, glm::value_ptr(*camera.trans));

    basic_drawRect(rect.VAO);

}
