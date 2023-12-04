#include "math.hpp"
#include <cmath>
const double PI = 3.141592653589793238462643383279502884197;
float randomFloat(float min, float max) {
    float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return min + random * (max - min);
}
int randomInt(int min, int max) {
    return min + rand() % (max - min + 1);
}
float getMagnitude(float Xvel, float Yvel) {
    return std::sqrt(Xvel * Xvel + Yvel * Yvel);
}
float getAngle(float Xvel, float Yvel) {
    return std::atan2(Yvel, Xvel);
}
float getXvel(float magnitude, float angle) {
    return magnitude * std::cos(angle);
}
float getYvel(float magnitude, float angle) {
    return magnitude * std::sin(angle);
}
float lerp(float start, float end, float t) {
    return start + t * (end - start);
}
float map(float input, float inputMin, float inputMax, float targetMin, float targetMax) {
    float inputRange = inputMax - inputMin;
    float normalizedInput = (input - inputMin) / inputRange;
    float targetRange = targetMax - targetMin;
    float mappedValue = targetMin + (normalizedInput * targetRange);
    return mappedValue;
}
double logarithm(double base, double x) {
    return log(x) / log(base);
}
float distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}
bool randomBool(float chanceOfTrue) {
    if (chanceOfTrue < 0.0f) chanceOfTrue = 0.0f;
    if (chanceOfTrue > 1.0f) chanceOfTrue = 1.0f;
    float randomValue = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return randomValue < chanceOfTrue;
}
float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
float randomRadian() {
    return static_cast<float>(rand()) / RAND_MAX * 2.0f * PI;
}
float randomDegree() {
    return static_cast<float>(rand()) / RAND_MAX * 360.0f;
}
float radiansToDegrees(float radians) {
    return radians * (180.0f / PI);
}
float degreesToRadians(float degrees) {
    return degrees * (PI / 180.0f);
}
bool pointInCircle(float px, float py, float cx, float cy, float r) {
    float dx = px - cx;
    float dy = py - cy;
    float distanceSquared = dx * dx + dy * dy;
    return distanceSquared <= r * r;
}
bool pointInRectangle(float px, float py, float left, float top, float right, float bottom) {
    return px >= left && px <= right && py >= top && py <= bottom;
}
float wrap(float value, float min, float max) {
    float range = max - min;
    if (range == 0.0f) {
        return min;
    }
    value = value - min;
    value = fmod(value, range);
    if (value < 0.0f) {
        value += range;
    }
    return value + min;
}
bool circleCollide(float cx1, float cy1, float r1, float cx2, float cy2, float r2) {
    float dx = cx2 - cx1;
    float dy = cy2 - cy1;
    float distanceSquared = dx * dx + dy * dy;
    float radiiSum = r1 + r2;
    return distanceSquared <= radiiSum * radiiSum;
}
bool rectangleCollide(float left1, float top1, float right1, float bottom1,
    float left2, float top2, float right2, float bottom2) {
    if (left1 > right2 || left2 > right1) return false;
    if (top1 > bottom2 || top2 > bottom1) return false;
    return true;
}
bool almostEqual(float a, float b, float epsilon = 0.0001f) {
    return std::abs(a - b) < epsilon;
}
int orientation(float px, float py, float qx, float qy, float rx, float ry) {
    float val = (qy - py) * (rx - qx) - (qx - px) * (ry - qy);
    if (std::abs(val) < 1e-10) return 0;
    return (val > 0) ? 1 : 2;
}
bool lineIntersect(float x1, float y1, float x2, float y2,
    float x3, float y3, float x4, float y4) {
    int o1 = orientation(x1, y1, x2, y2, x3, y3);
    int o2 = orientation(x1, y1, x2, y2, x4, y4);
    int o3 = orientation(x3, y3, x4, y4, x1, y1);
    int o4 = orientation(x3, y3, x4, y4, x2, y2);
    if (o1 != o2 && o3 != o4) return true;
    if (o1 == 0 && (x3 - x1) * (x2 - x1) + (y3 - y1) * (y2 - y1) >= 0 &&
        (x3 - x2) * (x1 - x2) + (y3 - y2) * (y1 - y2) >= 0) return true;
    if (o2 == 0 && (x4 - x1) * (x2 - x1) + (y4 - y1) * (y2 - y1) >= 0 &&
        (x4 - x2) * (x1 - x2) + (y4 - y2) * (y1 - y2) >= 0) return true;
    if (o3 == 0 && (x1 - x3) * (x4 - x3) + (y1 - y3) * (y4 - y3) >= 0 &&
        (x1 - x4) * (x3 - x4) + (y1 - y4) * (y3 - y4) >= 0) return true;
    if (o4 == 0 && (x2 - x3) * (x4 - x3) + (y2 - y3) * (y4 - y3) >= 0 &&
        (x2 - x4) * (x3 - x4) + (y2 - y4) * (y3 - y4) >= 0) return true;
    return false;
}
void rotatePoint(float cx, float cy, float angle, float& x, float& y) {
    float tempX = x - cx;
    float tempY = y - cy;
    float rotatedX = tempX * std::cos(angle) - tempY * std::sin(angle);
    float rotatedY = tempX * std::sin(angle) + tempY * std::cos(angle);
    x = rotatedX + cx;
    y = rotatedY + cy;
}