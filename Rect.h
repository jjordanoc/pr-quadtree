#ifndef RECT_H
#define RECT_H

#include "Point.h"

class Rect {
private:
    Point2D pmin, pmax;
public:
    Rect() = default;

    Rect(const Point2D &pmin, const Point2D &pmax) : pmin(pmin), pmax(pmax) {}

    ~Rect() = default;

    Point2D getPmin() const { return pmin; }

    Point2D getPmax() const { return pmax; }

    Point2D getCenter() const { return (pmin + pmax) / 2.0f; }

    NType distance(const Point2D &p) const {
        NType dx = NType::max(std::max(pmin.getX() - p.getX(), p.getX() - pmax.getX()), NType(0.));
        NType dy = NType::max(std::max(pmin.getY() - p.getY(), p.getY() - pmax.getY()), NType(0.));

        return NType::sqrt((dx * dx) + (dy * dy));
    }

    bool contains(const Point2D &point) const {
        return point.getX() >= pmin.getX() && point.getX() <= pmax.getX() && point.getY() >= pmin.getY() &&
               point.getY() <= pmax.getY();
    }

    bool intersects(const Rect &other) const {
        return pmin.getX() < other.pmax.getX() && pmax.getX() > other.pmin.getX() &&
               pmin.getY() < other.pmax.getY() && pmax.getY() > other.pmin.getY();
    }

    bool isWithin(const Rect &other) const {
        return pmin.getX() >= other.pmin.getX() && pmax.getX() <= other.pmax.getX() &&
               pmin.getY() >= other.pmin.getY() && pmax.getY() <= other.pmax.getY();
    }

    bool isValid() const {
        return pmin.getX() <= pmax.getX() && pmin.getY() <= pmax.getY();
    }

    bool operator==(const Rect &rect) const {
        return pmin == rect.pmin && pmax == rect.pmax;
    }

    bool operator!=(const Rect &rect) const {
        return !(*this == rect);
    }

    std::vector<Rect> split() {
        Point2D center = (pmax - pmin) * 0.5;
        Rect r1 = Rect(pmin + Point2D(0, center.getY()), pmin + Point2D(0, center.getY()) + center);
        Rect r2 = Rect(pmin + center, pmax);
        Rect r3 = Rect(pmin, center + pmin);
        Rect r4 = Rect(pmin + Point2D(center.getX(), 0), pmin + Point2D(center.getX(), 0) + center);
        return {r1, r2, r3, r4};
    }

    // Print
    friend std::ostream &operator<<(std::ostream &os, const Rect &rect) {
        os << "[Pmin:" << rect.pmin << ", Pmax:" << rect.pmax << "]";
        return os;
    }
};

#endif // RECT_H