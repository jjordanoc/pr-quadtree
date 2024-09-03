#include <iostream>
#include <set>
#include <random>
#include <vector>
#include "QuadTree.h"

std::vector<std::shared_ptr<Particle>> generateRandomParticles(int n, const Rect& boundary, NType maxVelocityMagnitude) {
    std::vector<std::shared_ptr<Particle>> particles;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDistX(boundary.getPmin().getX().getValue(), boundary.getPmax().getX().getValue());
    std::uniform_real_distribution<float> posDistY(boundary.getPmin().getY().getValue(), boundary.getPmax().getY().getValue());
    std::uniform_real_distribution<float> velDist(-maxVelocityMagnitude.getValue(), maxVelocityMagnitude.getValue());

    for (int i = 0; i < n; ++i) {
        NType x = NType(posDistX(gen));
        NType y = NType(posDistY(gen));
        Point2D position(x, y);

        NType vx = NType(velDist(gen));
        NType vy = NType(velDist(gen));
        Point2D velocity(vx, vy);

        auto particle = std::make_shared<Particle>(position, velocity);
        particles.push_back(particle);
    }

    return particles;
}

// Test 1: Verify all data is indexed
void traverseTree(QuadNode* node, std::set<std::shared_ptr<Particle>>& foundParticles) {
    if (node->isLeaf()) {
        for (const auto& particle : node->getParticles()) {
            foundParticles.insert(particle);
        }
    } else {
        for (const auto& child : node->getChildren()) {
            if (child) {
                traverseTree(child.get(), foundParticles);
            }
        }
    }
}

bool verifyAllDataIndexed(QuadNode* rootNode, const std::set<std::shared_ptr<Particle>>& insertedParticles) {
    std::set<std::shared_ptr<Particle>> foundParticles;
    traverseTree(rootNode, foundParticles);
    return foundParticles == insertedParticles;
}

// Test 2: Verify internal nodes with children are not leaves
bool traverseAndCheckInternalNodes(QuadNode* node) {
    if (!node->isLeaf()) {
        for (const auto& child : node->getChildren()) {
            if (child && node->isLeaf()) {
                return false;
            }
        }
        for (const auto& child : node->getChildren()) {
            if (child) {
                if (!traverseAndCheckInternalNodes(child.get())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyInternalNodesNotLeaf(QuadNode* rootNode) {
    return traverseAndCheckInternalNodes(rootNode);
}

// Test 3: Verify leaf nodes have no children
bool traverseAndCheckLeafNodes(QuadNode* node) {
    if (node->isLeaf()) {
        for (const auto& child : node->getChildren()) {
            if (child) {
                return false;
            }
        }
    } else {
        for (const auto& child : node->getChildren()) {
            if (child) {
                if (!traverseAndCheckLeafNodes(child.get())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyLeafNodesHaveNoChildren(QuadNode* rootNode) {
    return traverseAndCheckLeafNodes(rootNode);
}

// Test 4: Verify leaf nodes have no more than bucketSize elements
bool traverseAndCheckBucketSize(QuadNode* node, size_t bucketSize) {
    if (node->isLeaf()) {
        if (node->getParticles().size() > bucketSize) {
            return false;
        }
    } else {
        for (const auto& child : node->getChildren()) {
            if (child) {
                if (!traverseAndCheckBucketSize(child.get(), bucketSize)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyLeafNodesBucketSize(QuadNode* rootNode, size_t bucketSize) {
    return traverseAndCheckBucketSize(rootNode, bucketSize);
}

// Test 5: Verify child boundaries are within parent boundaries
bool traverseAndCheckBoundaries(QuadNode* node) {
    if (!node->isLeaf()) {
        for (const auto& child : node->getChildren()) {
            if (child && !child->getBoundary().isWithin(node->getBoundary())) {
                return false;
            }
        }
        for (const auto& child : node->getChildren()) {
            if (child) {
                if (!traverseAndCheckBoundaries(child.get())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyChildBoundariesWithinParent(QuadNode* rootNode) {
    return traverseAndCheckBoundaries(rootNode);
}

// Test 6: Verify no intersecting child boundaries
bool traverseAndCheckNoIntersections(QuadNode* node) {
    if (!node->isLeaf()) {
        for (int i = 0; i < 4; ++i) {
            for (int j = i + 1; j < 4; ++j) {
                if (node->getChild(i) && node->getChild(j)) {
                    if (node->getChild(i)->getBoundary().intersects(node->getChild(j)->getBoundary())) {
                        std::cout << "Intersecting boundaries: " << i << ", " << j << std::endl;
                        std::cout << "Child " << i << " boundary: " << node->getChild(i)->getBoundary() << std::endl;
                        std::cout << "Child " << j << " boundary: " << node->getChild(j)->getBoundary() << std::endl;
                        std::cout << std::endl; // debug
                        return false;
                    }
                }
            }
        }
        for (const auto& child : node->getChildren()) {
            if (child) {
                if (!traverseAndCheckNoIntersections(child.get())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyNoIntersectingChildBoundaries(QuadNode* rootNode) {
    return traverseAndCheckNoIntersections(rootNode);
}


// Test 7: Verify particles are in the correct leaf node
bool traverseAndCheckParticlesInCorrectLeaf(QuadNode* node) {
    if (node->isLeaf()) {
        for (const auto& particle : node->getParticles()) {
            if (!node->getBoundary().contains(particle->getPosition())) {
                std::cout << "Particle " << particle->getPosition() << " is out of its leaf boundary." << std::endl;
                return false;
            }
        }
    } else {
        for (const auto& child : node->getChildren()) {
            if (child) {
                if (!traverseAndCheckParticlesInCorrectLeaf(child.get())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool verifyParticlesInCorrectLeaf(QuadNode* rootNode) {
    return traverseAndCheckParticlesInCorrectLeaf(rootNode);
}


// Test 8: Verify k-NN search
bool verifyKnnSearch(QuadTree& tree, const std::vector<std::shared_ptr<Particle>>& particles, const Rect& boundary) {
    // Generar un punto de consulta aleatorio dentro del boundary
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDistX(boundary.getPmin().getX().getValue(), boundary.getPmax().getX().getValue());
    std::uniform_real_distribution<float> posDistY(boundary.getPmin().getY().getValue(), boundary.getPmax().getY().getValue());

    NType queryX = NType(posDistX(gen));
    NType queryY = NType(posDistY(gen));
    Point2D queryPoint(queryX, queryY);

    // Elegir un k aleatorio
    size_t k = std::uniform_int_distribution<size_t>(1, 10)(gen);

    // Obtener k-NN usando QuadTree
    std::vector<std::shared_ptr<Particle>> knnTree = tree.knn(queryPoint, k);

    // Obtener k-NN usando fuerza bruta
    std::vector<std::shared_ptr<Particle>> knnBruteForce = particles;
    std::sort(knnBruteForce.begin(), knnBruteForce.end(), [&queryPoint](const std::shared_ptr<Particle>& a, const std::shared_ptr<Particle>& b) {
        return queryPoint.distance(a->getPosition()) < queryPoint.distance(b->getPosition());
    });
    knnBruteForce.resize(k); // Seleccionar los primeros k vecinos más cercanos

    // Verificar si ambos resultados son equivalentes y en el mismo orden
    if (knnTree.size() != knnBruteForce.size()) {
        std::cout << "diff size" << std::endl;
        return false;
    }
    for (size_t i = 0; i < k; ++i) {
        if (knnTree[i] != knnBruteForce[i]) {
            std::cout << "diff elem" << std::endl;
            return false;
        }
    }

    return true;
}

// Run all tests
bool runTesting(QuadTree& tree, const std::vector<std::shared_ptr<Particle>>& particles, const Rect& boundary) {
    bool allTestsPassed = true;

    if (!verifyAllDataIndexed(tree.getRoot().get(), {particles.begin(), particles.end()})) {
        std::cout << "Test failed: Not all data is indexed correctly." << std::endl;
        allTestsPassed = false;
    }

    if (!verifyInternalNodesNotLeaf(tree.getRoot().get())) {
        std::cout << "Test failed: Internal nodes with children are marked as leaf." << std::endl;
        allTestsPassed = false;
    }

    if (!verifyLeafNodesHaveNoChildren(tree.getRoot().get())) {
        std::cout << "Test failed: Leaf nodes have children." << std::endl;
        allTestsPassed = false;
    }

    if (!verifyLeafNodesBucketSize(tree.getRoot().get(), QuadTree::bucketSize)) {
        std::cout << "Test failed: Leaf nodes exceed bucketSize." << std::endl;
        allTestsPassed = false;
    }

    if (!verifyChildBoundariesWithinParent(tree.getRoot().get())) {
        std::cout << "Test failed: Child boundaries are not within parent boundaries." << std::endl;
        allTestsPassed = false;
    }

    if (!verifyNoIntersectingChildBoundaries(tree.getRoot().get())) {
        std::cout << "Test failed: Child boundaries intersect." << std::endl;
        allTestsPassed = false;
    }

    if (!verifyParticlesInCorrectLeaf(tree.getRoot().get())) {
        std::cout << "Test failed: Particles are not in the correct leaf node." << std::endl;
        allTestsPassed = false;
    }

    if (!verifyKnnSearch(tree, particles, boundary)) {
        std::cout << "Test failed: k-NN search did not return the expected results." << std::endl;
        allTestsPassed = false;
    }

    return allTestsPassed;
}

int main() {
    Rect boundary(Point2D(0, 0), Point2D(100, 100));
    QuadTree tree(boundary);
    bool allTestsPassed;

    int numParticles = 200000;
    NType maxVelocity = 5.0;
    std::vector<std::shared_ptr<Particle>> particles = generateRandomParticles(numParticles, boundary, maxVelocity);
    tree.insert(particles);

    // Ejecutar pruebas
    allTestsPassed = runTesting(tree, particles, boundary);
    if (allTestsPassed) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << "Some tests failed." << std::endl;
    }

    // Mover partículas y actualizar el árbol
    std::cout << std::endl << "Updating particles..." << std::endl;
    for (auto& particle : particles) {
        particle->updatePosition(boundary);
    }
    tree.updateTree();
    allTestsPassed = runTesting(tree, particles, boundary);
    if (allTestsPassed) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << "Some tests failed." << std::endl;
    }

    return 0;
}
