#ifndef RENDER_HPP
#define RENDER_HPP
#include <SDL.h>
#include <glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <random>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/type_ptr.hpp>
#include "structs.hpp"
void basic_setupTriangle(GLuint& VAO, GLuint& VBO, GLenum drawType);
void basic_drawTriangle(GLuint& VAO);
void batch_setupTriangles(GLuint& VAO, GLuint& VBO, const std::vector<float>& vertices, GLenum drawType);
void batch_drawTriangles(GLuint& VAO, int vertexCount);
void instance_setupTriangle(GLuint& VAO, GLuint& VBO, GLuint& instanceVBO, glm::vec2* positions, int numInstances, GLenum drawType);
void instance_drawTriangle(GLuint& VAO, int numInstances);
void basic_setupRect(GLuint& VAO, GLuint& VBO, GLenum drawType, float verticies[]);
void basic_drawRect(GLuint& VAO);
void setup_rect(rect* rect);
void draw_rect(rect rect, camera camera);
void setup_line(line* line);
void draw_line(line line, camera camera);
void basic_setupCircle(GLuint& VAO, GLuint& VBO, float radius, int segments, GLenum drawType);
void basic_drawCircle(GLuint& VAO, int segments);
void batch_setupCircles(GLuint& VAO, GLuint& VBO, const std::vector<CircleParams>& circles, int segments, GLenum drawType);
void batch_drawCircles(GLuint& VAO, int segments, int circleCount);
void setupCircleInstances(GLuint& VAO, GLuint& VBO, GLuint& instanceVBO,
    float radius, int segments,
    const glm::vec2* positions, int numInstances, GLenum drawType);
void instance_drawCircle(GLuint& VAO, int segments, int numInstances);
void batch_setupSquares(GLuint& VAO, GLuint& VBO, const std::vector<float>& vertices, GLenum drawType);
void batch_drawSquares(GLuint& VAO, int vertexCount);
void instance_setupSquare(GLuint& VAO, GLuint& VBO, GLuint& instanceVBO, glm::vec2* positions, int numInstances, GLenum drawType);
void instance_drawSquare(GLuint& VAO, int numInstances);
void setupLine(GLuint& VAO, GLuint& VBO, const glm::vec2& start, const glm::vec2& end, GLenum drawType);
void drawLine(GLuint& VAO);
#endif