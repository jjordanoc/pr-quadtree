#ifndef POINT_H
#define POINT_H

#include "DataType.h"
#include <iostream>

class Point2D
{
private:
    NType x,y;

public:
    Point2D(): x(0), y(0) {}
    Point2D(NType x, NType y): x(x), y(y) {}
    ~Point2D() = default;

    NType getX() const { return x; }
    NType getY() const { return y; }

    void setX(NType x) { this->x = x; }
    void setY(NType y) { this->y = y; }

    NType distance(const Point2D& p) const {
        return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2));
    }

    bool operator==(const Point2D& p) const {
        return x == p.x && y == p.y;
    }

    bool operator!=(const Point2D& p) const {
        return !(*this == p);
    }

    Point2D operator-(const Point2D& p) const {
        return Point2D(x - p.x, y - p.y);
    }
    Point2D operator+(const Point2D& p) const {
        return Point2D(x + p.x, y + p.y);
    }
    Point2D operator*(NType scalar) const {
        return Point2D(x * scalar, y * scalar);
    }
    Point2D operator/(NType scalar) const {
        return Point2D(x / scalar, y / scalar);
    }
    
    // Print
    friend std::ostream& operator<<(std::ostream& os, const Point2D& p) {
        os << "(" << p.x.getValue() << "," << p.y.getValue() << ")";
        return os;
    }
};


#endif // POINT_H
