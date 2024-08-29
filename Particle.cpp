#include "Particle.h"
const NType Particle::timeStep = 1.5;

void Particle::updatePosition(const Rect& boundary) {
    NType remainingTime = timeStep;
    
    while (remainingTime > 0) {
        Point2D newPosition = position + (velocity * remainingTime);
        bool hitXBoundary = false, hitYBoundary = false;

        // Verificar los límites en X
        if (newPosition.getX() < boundary.getPmin().getX()) {
            remainingTime *= (position.getX() - boundary.getPmin().getX()) / (position.getX() - newPosition.getX());
            velocity.setX(-velocity.getX());
            newPosition.setX(boundary.getPmin().getX());
            hitXBoundary = true;
        } else if (newPosition.getX() > boundary.getPmax().getX()) {
            remainingTime *= (boundary.getPmax().getX() - position.getX()) / (newPosition.getX() - position.getX());
            velocity.setX(-velocity.getX());
            newPosition.setX(boundary.getPmax().getX());
            hitXBoundary = true;
        }

        // Verificar los límites en Y
        if (newPosition.getY() < boundary.getPmin().getY()) {
            remainingTime *= (position.getY() - boundary.getPmin().getY()) / (position.getY() - newPosition.getY());
            velocity.setY(-velocity.getY());
            newPosition.setY(boundary.getPmin().getY());
            hitYBoundary = true;
        } else if (newPosition.getY() > boundary.getPmax().getY()) {
            remainingTime *= (boundary.getPmax().getY() - position.getY()) / (newPosition.getY() - position.getY());
            velocity.setY(-velocity.getY());
            newPosition.setY(boundary.getPmax().getY());
            hitYBoundary = true;
        }

        // Actualizar la posición final
        position = newPosition;

        // Si no rebotamos, salimos del bucle
        if (!hitXBoundary && !hitYBoundary) {
            break;
        }
    }
}