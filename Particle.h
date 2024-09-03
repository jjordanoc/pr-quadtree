#ifndef PARTICLE_H
#define PARTICLE_H

#include "Point.h"
#include "Rect.h"

class Particle {
private:
    Point2D position;
    Point2D velocity;
    static const NType timeStep;  

public:
    Particle(const Point2D& position, const Point2D& velocity)
        : position(position), velocity(velocity) {}

    Point2D getPosition() const { return position; }
    Point2D getVelocity() const { return velocity; }

    void setPosition(const Point2D& pos) { position = pos; }
    void setVelocity(const Point2D& vel) { velocity = vel; }

    void updatePosition(const Rect& boundary);

    friend std::ostream& operator<<(std::ostream& os, const Particle& p) {
        os << "Position: " << p.position ;
//        << " Velocity: " << p.velocity;
        return os;
    }
};


#endif // PARTICLE_H