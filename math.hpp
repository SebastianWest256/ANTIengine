#ifndef MATH_HPP
#define MATH_HPP
float randomFloat(float min, float max);
int randomInt(int min, int max);
float getMagnitude(float Xvel, float Yvel);
float getAngle(float Xvel, float Yvel);
float getXvel(float magnitude, float angle);
float getYvel(float magnitude, float angle);
float lerp(float start, float end, float t);
float map(float input, float inputMin, float inputMax, float targetMin, float targetMax);
double logarithm(double base, double x);
float distance(float x1, float y1, float x2, float y2);
bool randomBool(float chanceOfTrue);
float clamp(float value, float min, float max);
float randomRadian();
float randomDegree();
float radiansToDegrees(float radians);
float degreesToRadians(float degrees);
bool pointInCircle(float px, float py, float cx, float cy, float r);
bool pointInRectangle(float px, float py, float left, float top, float right, float bottom);
float wrap(float value, float min, float max);
bool circleCollide(float cx1, float cy1, float r1, float cx2, float cy2, float r2);
bool rectangleCollide(float left1, float top1, float right1, float bottom1, float left2, float top2, float right2, float bottom2);
bool almostEqual(float a, float b, float epsilon);
int orientation(float px, float py, float qx, float qy, float rx, float ry);
bool lineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
void rotatePoint(float cx, float cy, float angle, float& x, float& y);
#endif
