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
#include "render.hpp"
#include "structs.hpp"
#include "math.hpp"
int random_int(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 700;
const int MAX_FPS = 60;
const int FRAME_DELAY = 1000 / MAX_FPS;
const float MAX_FORCE = 1;
const float DAMPENING = 0.97;
GLuint compileShader(const GLchar* source, GLenum shaderType);
GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader);
std::string readShaderSource(const std::string& filePath);
GLuint createShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
GLuint pixelationShaderProgram;
void setupPixelationShader() {
    pixelationShaderProgram = createShaderProgram("pixelVertShader.vert", "pixelFragShader.frag");
}
void renderFullScreenQuad(GLuint quadVAO) {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

class creature {
public:
    std::vector<joint> joints;
    std::vector<spring> connections;
    std::vector<foot> feet;
    line constraint;
    int direction = 0;
    bool moving = false;
    void create_con(int a, int b, float length, float strength) {
        connections.emplace_back(a, b, length, strength);
        joints[a].con_indexes.push_back(connections.size() - 1);
        joints[b].con_indexes.push_back(connections.size() - 1);
    }
    void init() {
        float spring_strength = 0.05;
        int w = 100, h = 2;
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                joints.emplace_back(j, i, 0.1, 0.1, 0, 0, 1);
            }
        }
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                for (int k = 0; k < h; k++) {
                    for (int l = 0; l < w; l++) {
                        if (i != k || j != l) {
                            if (std::sqrt((k - i) * (k - i) + (l - j) * (l - j)) < 2.1) {
                                create_con(i * w + j, k * w + l, std::sqrt((k - i) * (k - i) + (l - j) * (l - j)), spring_strength);
                            }
                        }
                    }
                }
            }
        }
        for (int i = 0; i < w; i++) {
            joints.emplace_back(i, -0.5, 0.1, 0.1, 0, 0, 1);
            feet.emplace_back(w * h + i, 0.04);
            create_con(w * h + i, i, 0.2, spring_strength);
        }

        for (int i = 0; i < joints.size(); i++) {
            setup_rect(&joints[i].rect);
        }

    }
    void draw(camera camera) {
        for (int i = 0; i < joints.size(); i++) {
            draw_rect(joints[i].rect, camera);
        }
        for (int i = 0; i < connections.size(); i++) {
            constraint.x0 = joints[connections[i].a].x;
            constraint.y0 = joints[connections[i].a].y;
            constraint.x1 = joints[connections[i].b].x;
            constraint.y1 = joints[connections[i].b].y;
            glDeleteVertexArrays(1, &constraint.VAO);
            glDeleteBuffers(1, &constraint.VBO);
            setup_line(&constraint);
            draw_line(constraint, camera);
        }
    }
    int get_other_index(int joint_index, int con_index) {
        if (connections[joints[joint_index].con_indexes[con_index]].a == joint_index) {
            return connections[joints[joint_index].con_indexes[con_index]].b;
        }
        else {
            return connections[joints[joint_index].con_indexes[con_index]].a;
        }
    }
    float get_joint_center(int joint_index) {
        float avg_joint_x = 0;
        for (int i = 0; i < joints[joint_index].con_indexes.size(); i++) {
            avg_joint_x += joints[get_other_index(joint_index, i)].x;
        }
        return avg_joint_x / float(joints[joint_index].con_indexes.size());
    }
    bool is_standing() {
        bool standing = false;
        for (int i = 0; i < feet.size(); i++) {
            if (feet[i].gripping) {
                standing = true;
            }
        }
        return standing;
    }
    bool is_only_standing(int index) {
        bool only_standing = true;
        for (int i = 0; i < feet.size(); i++) {
            if (feet[i].gripping && i != index) {
                only_standing = false;
            }
        }
        return only_standing;
    }
    void tick_connections() {
        std::cout << connections.size() << "\n";
        for (int i = 0; i < connections.size(); i++) {
            float dx = joints[connections[i].a].x - joints[connections[i].b].x;
            float dy = joints[connections[i].a].y - joints[connections[i].b].y;
            float distance = std::max(double(std::sqrt(dx * dx + dy * dy)), 0.00001);
            float displacement = distance - connections[i].length;
            float force = displacement * connections[i].strength;
            float force_x = force * (dx / distance);
            float force_y = force * (dy / distance);
            joints[connections[i].a].xv -= force_x;
            joints[connections[i].a].yv -= force_y;
            joints[connections[i].b].xv += force_x;
            joints[connections[i].b].yv += force_y;
        }
    }
    void tick_joints() {
        for (int i = 0; i < joints.size(); i++) {
            joints[i].xv = std::max(-MAX_FORCE, std::min(MAX_FORCE, joints[i].xv));
            joints[i].yv = std::max(-MAX_FORCE, std::min(MAX_FORCE, joints[i].yv));
            joints[i].x += joints[i].xv / joints[i].m;
            joints[i].y += joints[i].yv / joints[i].m;
            joints[i].xv *= DAMPENING;
            joints[i].yv *= DAMPENING;
            joints[i].rect.x = joints[i].x;
            joints[i].rect.y = joints[i].y;
        }
    }
    void tick_gripping() {
        for (int i = 0; i < feet.size(); i++) {
            if (feet[i].gripping) {
                joints[feet[i].index].xv = 0;
                joints[feet[i].index].yv = 0;
                for (int j = 0; j < joints[feet[i].index].con_indexes.size(); j++) {
                    joints[get_other_index(feet[i].index, j)].yv += feet[i].strength;
                }
            }
        }
    }
    void tick() {
        float r_dist = 0.2;
        tick_connections();
        if (moving) {
            for (int i = 0; i < feet.size(); i++) {
                if ((joints[feet[i].index].x - get_joint_center(feet[i].index)) * direction > r_dist) {
                    feet[i].ready = true;
                }
                if ((joints[feet[i].index].x - get_joint_center(feet[i].index)) * -direction > r_dist && !is_only_standing(i)) {
                    feet[i].ready = false;
                    feet[i].gripping = false;
                }
                if (!feet[i].gripping) {
                    if (feet[i].ready) {
                        joints[feet[i].index].yv -= feet[i].strength;
                    }
                    else {
                        joints[feet[i].index].xv += direction * feet[i].strength;
                        joints[feet[i].index].yv += feet[i].strength / 10.0;
                    }

                }
                else {
                    for (int j = 0; j < joints[feet[i].index].con_indexes.size(); j++) {
                        joints[get_other_index(feet[i].index, j)].xv += direction * feet[i].strength;
                    }
                }
            }
        }
        else {
            for (int i = 0; i < feet.size(); i++) {
                //feet[i].ready = true;
            }
        }
        for (int i = 0; i < feet.size(); i++) {
            if (joints[feet[i].index].grounded && feet[i].ready) {
                feet[i].gripping = true;
            }
        }
        tick_gripping();
        tick_joints();

    }
    void move(int x, int y) {
        direction = x;
        if (x == 0) {
            moving = false;
        }
        else {
            moving = true;
        }
    }
};
int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window* window = SDL_CreateWindow("OpenGL with SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error initializing GLEW!" << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    GLuint shaderProgram = createShaderProgram("vertexShader.vert", "fragmentShader.frag");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //setup
    GLuint basic_triangleVAO, basic_triangleVBO;
    basic_setupTriangle(basic_triangleVAO, basic_triangleVBO, GL_STATIC_DRAW);
    //b
    std::vector<float> batch_trianglesVertices = {
        -2.5f, -3.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,

        -1.5f,  1.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,

        -0.5f, -1.0f, 0.0f,
         1.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    GLuint batchVAO, batchVBO;
    batch_setupTriangles(batchVAO, batchVBO, batch_trianglesVertices, GL_STATIC_DRAW);
    //i
    int numInstances = 2;
    glm::vec2 instancePositions[] = {
    glm::vec2(-0.5f, 0.0f),
    glm::vec2(1.5f, 1.0f),
    };
    GLuint instance_triangleVAO, instance_triangleVBO;
    GLuint instance_instanceVBO;
    instance_setupTriangle(instance_triangleVAO, instance_triangleVBO, instance_instanceVBO, instancePositions, numInstances, GL_STATIC_DRAW);
    //circle
    GLuint basic_circleVAO, basic_circleVBO;
    float basic_circleRadius = 1.5f;
    int basic_circleSegments = 12;
    basic_setupCircle(basic_circleVAO, basic_circleVBO, basic_circleRadius, basic_circleSegments, GL_STATIC_DRAW);
    //b
    std::vector<CircleParams> batch_circles = {
    {0.0f, 0.0f, 0.5f},
    {1.0f, 1.0f, 0.3f}
    };
    GLuint batch_CirclesVAO, batch_CirclesVBO;
    int batch_circleSegments = 50;
    batch_setupCircles(batch_CirclesVAO, batch_CirclesVBO, batch_circles, batch_circleSegments, GL_STATIC_DRAW);
    //i
    GLuint circleVAO, circleVBO, circleInstanceVBO;
    float circleRadius = 0.5f;
    int circleSegments = 50;
    glm::vec2 circle_instancePositions[] = { {0.0f, 0.0f},
    {1.0f, 1.0f} };
    int circle_numInstances = sizeof(circle_instancePositions) / sizeof(glm::vec2);
    setupCircleInstances(circleVAO, circleVBO, circleInstanceVBO,
        circleRadius, circleSegments,
        circle_instancePositions, circle_numInstances, GL_STATIC_DRAW);
    //  
    float rect_vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    //s
    GLuint basic_squareVAO, basic_squareVBO;
    basic_setupRect(basic_squareVAO, basic_squareVBO, GL_STATIC_DRAW, rect_vertices);
    //b
    std::vector<float> squareVertices = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,

        -0.2f, -0.7f, 0.0f,
         0.2f, -0.7f, 0.0f,
         0.2f,  0.7f, 0.0f,
         0.2f,  0.7f, 0.0f,
        -0.2f,  0.7f, 0.0f,
        -0.2f, -0.7f, 0.0f,

    };
    GLuint batchSquaresVAO, batchSquaresVBO;
    batch_setupSquares(batchSquaresVAO, batchSquaresVBO, squareVertices, GL_STATIC_DRAW);
    //i
    GLuint instance_squareVAO, instance_squareVBO, squareInstanceVBO;
    glm::vec2 squarePositions[] = { {0.0f, 0.0f},
    {1.0f, 1.0f} };
    int numSquareInstances = sizeof(squarePositions) / sizeof(glm::vec2);
    instance_setupSquare(instance_squareVAO, instance_squareVBO, squareInstanceVBO, squarePositions, numSquareInstances, GL_STATIC_DRAW);
    //  
    GLuint lineVAO, lineVBO;
    glm::vec2 lineStart = glm::vec2(-0.5f, -0.5f);
    glm::vec2 lineEnd = glm::vec2(0.5f, 0.5f);
    setupLine(lineVAO, lineVBO, lineStart, lineEnd, GL_STATIC_DRAW);
    //
    rect aa(0, 0, 0, 1, 1);
    rect bb(1, 0, 0, 1, 1);
    setup_rect(&aa);
    setup_rect(&bb);
    line cc(0.5, 0.5, 0.0, 1.5, 0.9, 0.0);
    setup_line(&cc);
    //
    creature blob;
    blob.init();
    //setup
    bool running = true;
    SDL_Event event;
    GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    GLuint fbo, texture;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    setupPixelationShader();
    GLint pixelSizeLocation = glGetUniformLocation(pixelationShaderProgram, "pixelSize");
    GLint colorLoc = glGetUniformLocation(shaderProgram, "ourColor");
    GLint texLocation = glGetUniformLocation(pixelationShaderProgram, "screenTexture");
    glm::mat4 trans = glm::mat4(1.0f);
    Uint32 startFrame, endFrame, frameTime;
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    std::vector<int> fps_history(100, 200);
    int average_fps;
    //camera
    camera main_cam;
    main_cam.x = 0;
    main_cam.y = 0;
    main_cam.zoom = 1;
    main_cam.trans = &trans;
    main_cam.transformLoc = &transformLoc;
    main_cam.SCREEN_HEIGHT = SCREEN_HEIGHT;
    main_cam.SCREEN_WIDTH = SCREEN_WIDTH;
    int blob_x = 0, blob_y = 0;
    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_UP:
                    main_cam.y += 0.1;
                    break;
                case SDLK_DOWN:
                    main_cam.y -= 0.1;
                    break;
                case SDLK_LEFT:
                    main_cam.x -= 0.1;
                    break;
                case SDLK_RIGHT:
                    main_cam.x += 0.1;
                    break;
                case SDLK_n:
                    main_cam.zoom -= 0.1;
                    break;
                case SDLK_m:
                    main_cam.zoom += 0.1;
                    break;
                case SDLK_q:

                    break;
                case SDLK_w:
                    blob_x = 0;
                    blob_y = 1;
                    break;
                case SDLK_a:
                    blob_x = -1;
                    blob_y = 0;
                    break;
                case SDLK_s:
                    blob_x = 0;
                    blob_y = -1;
                    break;
                case SDLK_d:
                    blob_x = 1;
                    blob_y = 0;
                    break;
                }
            }
            else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                case SDLK_UP:

                    break;
                case SDLK_DOWN:

                    break;
                case SDLK_w:
                    blob_x = 0;
                    blob_y = 0;
                    break;
                case SDLK_a:
                    blob_x = 0;
                    blob_y = 0;
                    break;
                case SDLK_s:
                    blob_x = 0;
                    blob_y = 0;
                    break;
                case SDLK_d:
                    blob_x = 0;
                    blob_y = 0;
                    break;
                }
            }
        }
        start = std::chrono::high_resolution_clock::now();
        blob.move(blob_x, blob_y);
        startFrame = SDL_GetTicks();
        glm::vec4 blueColor(0.0f, 0.0f, 0.3f, 0.5f);

        trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        trans = glm::scale(trans, glm::vec3(1.0f, 1.0f, 1.0f));

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        glUniform4fv(colorLoc, 1, glm::value_ptr(blueColor));
        //basic_drawTriangle(basic_triangleVAO);
        //batch_drawTriangles(batchVAO, batch_trianglesVertices.size() / 3);
        //instance_drawTriangle(instance_triangleVAO, numInstances);
        //basic_drawCircle(basic_circleVAO, basic_circleSegments);
        //batch_drawCircles(batch_CirclesVAO, batch_circleSegments, batch_circles.size());
        //instance_drawCircle(circleVAO, circleSegments, circle_numInstances);
        //basic_drawRect(basic_squareVAO);
        //batch_drawSquares(batchSquaresVAO, squareVertices.size() / 3);
        //instance_drawSquare(instance_squareVAO, numSquareInstances);
        //draw_rect(aa, main_cam);
        //draw_rect(bb, main_cam);
        //draw_line(cc, main_cam);
        blob.tick();
        blob.draw(main_cam);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(pixelationShaderProgram);
        glUniform1f(pixelSizeLocation, 0.001);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glUniform1i(texLocation, 0);
        renderFullScreenQuad(quadVAO);
        SDL_GL_SwapWindow(window);
        end = std::chrono::high_resolution_clock::now();
        endFrame = SDL_GetTicks();
        frameTime = endFrame - startFrame;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        for (int i = 0; i < fps_history.size() - 1; i++) {
            fps_history[fps_history.size() - 1 - i] = fps_history[fps_history.size() - 2 - i];
        }
        fps_history[0] = int(1000000000.0f / double(duration.count()));
        average_fps = 0;
        for (int i = 0; i < fps_history.size(); i++) {
            average_fps += fps_history[i];
        }
        average_fps /= fps_history.size();
        std::cout << duration.count() << " ns " << average_fps << " fps " << std::endl;
    }
    glDeleteVertexArrays(1, &basic_triangleVAO);
    glDeleteBuffers(1, &basic_triangleVBO);
    glDeleteProgram(shaderProgram);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


GLuint compileShader(const GLchar* source, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}
GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    }
    return program;
}
std::string readShaderSource(const std::string& filePath) {
    std::ifstream shaderFile(filePath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
}
GLuint createShaderProgram(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    std::string vertexShaderSource = readShaderSource(vertexShaderPath);
    std::string fragmentShaderSource = readShaderSource(fragmentShaderPath);
    GLuint vertexShader = compileShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);
    GLuint shaderProgram = linkShaderProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

