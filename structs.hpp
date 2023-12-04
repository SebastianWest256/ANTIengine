#ifndef STRUCTS_HPP
#define STRUCTS_HPP
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
struct camera {
    int SCREEN_WIDTH, SCREEN_HEIGHT;
    float x, y, zoom;
    glm::mat4* trans;
    GLint* transformLoc;
};
struct rect {
    GLuint VAO, VBO;
    GLenum drawType;
    float x, y, z, w, h;
    rect(float initX = 0.0f, float initY = 0.0f, float initZ = 0.0f, float initW = 1.0f, float initH = 1.0f, GLenum initDrawType = GL_STATIC_DRAW)
        : x(initX), y(initY), z(initZ), w(initW), h(initH), drawType(initDrawType) {
    }
};
struct CircleParams {
    float centerX;
    float centerY;
    float radius;
};
struct line {
    GLuint VAO, VBO;
    GLenum drawType;
    float x0, y0, z0, x1, y1, z1;
    line(float initX0 = 0.0f, float initY0 = 0.0f, float initZ0 = 0.0f, float initX1 = 0.0f, float initY1 = 0.0f, float initZ1 = 0.0f, GLenum initDrawType = GL_STATIC_DRAW)
        : x0(initX0), y0(initY0), z0(initZ0), x1(initX1), y1(initY1), z1(initZ1), drawType(initDrawType) {
    }
};
struct spring {
    int a, b;
    float length, strength;
    spring(float initA = 0.0f, float initB = 0.0f, float initLENGTH = 0.0f, float initSTRENGTH = 1.0f)
        : a(initA), b(initB), length(initLENGTH), strength(initSTRENGTH) {
    }
};
struct joint {
    float x, y, w, h;
    float xv, yv;
    float m;
    bool grounded;
    std::vector<int> con_indexes;
    rect rect;
    joint(float initX = 0.0f, float initY = 0.0f, float initW = 0.0f, float initH = 1.0f, float initXV = 1.0f, float initYV = 1.0f, float initM = 1.0f)
        : x(initX), y(initY), w(initW), h(initH), xv(initXV), yv(initYV), m(initM) {
        grounded = false;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
    }
};
struct foot {
    int index;
    float strength;
    bool ready;
    bool gripping;
    foot(int initINDEX, float initSTRENGTH) : index(initINDEX), strength(initSTRENGTH) {
        ready = true;
        gripping = false;
    }
};
#endif