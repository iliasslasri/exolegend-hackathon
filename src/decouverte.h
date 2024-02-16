#ifndef DECOUVERTE_H
#define DECOUVERTE_H
# include "gladiator.h"
#include <cmath>
#undef abs

#define CELL_SIZE 3.0/14


/************************* Vect2 class ***********************/
class Vector2 {
public:
    Vector2() : _x(0.), _y(0.) {}
    Vector2(float x, float y) : _x(x), _y(y) {}

    float norm1() const { return std::abs(_x) + std::abs(_y); }
    float norm2() const { return std::sqrt(_x*_x+_y*_y); }
    void normalize() { _x/=norm2(); _y/=norm2(); }
    Vector2 normalized() const { Vector2 out=*this; out.normalize(); return out; }

    Vector2 operator-(const Vector2& other) const { return {_x-other._x, _y-other._y}; }
    Vector2 operator+(const Vector2& other) const { return {_x+other._x, _y+other._y}; }
    Vector2 operator*(float f) const { return {_x*f, _y*f}; }

    bool operator==(const Vector2& other) const { return std::abs(_x-other._x) < 1e-5 && std::abs(_y-other._y)<1e-5; }
    bool operator!=(const Vector2& other) const { return !(*this == other); }

    float x() const { return _x;}
    float y() const { return _y;}

    float dot(const Vector2& other) const { return _x*other._x + _y*other._y; }
    float cross(const Vector2& other) const { return _x*other._y - _y*other._x; }
    float angle(const Vector2& m) const { return std::atan2(cross(m), dot(m)); }
    float angle() const { return std::atan2(_y, _x); }
private:
    float _x, _y;
};


Vector2 getAccessibleNeighbor(Gladiator *gladiator);
void getIJfromXY(float x, float y, int *t);
Vector2 getXYfromIJ(int i, int j);

float moduloPi(float);
bool aim(Gladiator* , const Vector2& , bool );
// function returns true if the direction of the robot has a wall in it
bool hasWall(Gladiator* gladiator, const MazeSquare* nearestSquare, const float angle);
#endif // DECOUVERTE_H