#ifndef QUADTREE_H
#define QUADTREE_H

#include "Particle.h"
#include "Rect.h"
#include <vector>
#include <memory>
#include <array>

//class QuadTree;

class QuadNode {
private:
    std::vector<std::shared_ptr<Particle>> particles;
    std::array<std::shared_ptr<QuadNode>, 4> children; // NW, NE, SW, SE
    Rect boundary;
    QuadNode *parent;
    bool _isLeaf;

    void addToBucket(const std::shared_ptr<Particle> &particle);

    bool propagate(const std::shared_ptr<Particle> &particle);

    void subdivide();

    void relocateParticle(const std::shared_ptr<Particle> &particle);

    void removeEmptyNode();

//    bool insert(std::shared_ptr<QuadNode> node, const std::shared_ptr<Particle> &p);

public:
    QuadNode(NType xmin, NType ymin, NType xmax, NType ymax, QuadNode *parent = nullptr)
            : boundary(Point2D(xmin, ymin), Point2D(xmax, ymax)), parent(parent), _isLeaf(true) {}

    explicit QuadNode(const Rect &boundary, QuadNode *parent = nullptr)
            : boundary(boundary), parent(parent), _isLeaf(true) {}

    bool insert(const std::shared_ptr<Particle> &particle);

    void updateNode();

    // Getters
    const std::vector<std::shared_ptr<Particle>> &getParticles() const { return particles; }

    const std::shared_ptr<QuadNode> &getChild(size_t index) const { return children[index]; }

    const std::array<std::shared_ptr<QuadNode>, 4> &getChildren() const { return children; }

    const Rect &getBoundary() const { return boundary; }
//    const QuadNode* getParent() const { return parent; }

    // Setters
//    void setParent(std::shared_ptr<QuadNode> parent);
    void clearParticles() { particles.clear(); }

    bool isLeaf() const { return _isLeaf; }
};


class QuadTree {
private:
    std::shared_ptr<QuadNode> root;

    struct KNNTreePair {
        KNNTreePair(std::shared_ptr<QuadNode> _node, Point2D _query) {
            node = _node;
            distToQuery = node->getBoundary().distance(_query).getValue();
        }

        float distToQuery;
        std::shared_ptr<QuadNode> node;

        bool operator<(const KNNTreePair &rhs) const {
            return distToQuery < rhs.distToQuery;
        }

        bool operator>(const KNNTreePair &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const KNNTreePair &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const KNNTreePair &rhs) const {
            return !(*this < rhs);
        }
    };

    struct KNNParticlePair {
        KNNParticlePair(std::shared_ptr<Particle> particle, Point2D query) : particle(particle) {
            distToQuery = query.distance(particle->getPosition()).getValue();
        }

        float distToQuery;
        std::shared_ptr<Particle> particle;

        bool operator<(const KNNParticlePair &rhs) const {
            return distToQuery < rhs.distToQuery;
        }

        bool operator>(const KNNParticlePair &rhs) const {
            return rhs < *this;
        }

        bool operator<=(const KNNParticlePair &rhs) const {
            return !(rhs < *this);
        }

        bool operator>=(const KNNParticlePair &rhs) const {
            return !(*this < rhs);
        }
    };

public:
    static size_t bucketSize;

    // Constructors
    QuadTree(NType xmin, NType ymin, NType xmax, NType ymax, size_t bucketSize)
            : root(std::make_shared<QuadNode>(Rect(Point2D(xmin, ymin), Point2D(xmax, ymax)))) {
        QuadTree::bucketSize = bucketSize;
    }

    QuadTree(const Rect &boundary, size_t bucketSize)
            : root(std::make_shared<QuadNode>(boundary)) {
        QuadTree::bucketSize = bucketSize;
    }

    QuadTree(NType xmin, NType ymin, NType xmax, NType ymax)
            : root(std::make_shared<QuadNode>(Rect(Point2D(xmin, ymin), Point2D(xmax, ymax)))) {}

    QuadTree(const Rect &boundary)
            : root(std::make_shared<QuadNode>(boundary)) {}

    void insert(const std::vector<std::shared_ptr<Particle>> &particles);

    const std::shared_ptr<QuadNode> &getRoot() const;

    std::vector<std::shared_ptr<Particle>> knn(Point2D query, size_t k);

    void updateTree();
};

#endif // QUADTREE_H
