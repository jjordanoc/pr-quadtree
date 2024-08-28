#include <iostream>
#include <queue>

class Rect;

class Point2D {
public:
    Point2D();

    Point2D(double x, double y) : x(x), y(y) {}

    double getX() const {
        return x;
    }

    void setX(double x) {
        Point2D::x = x;
    }

    double getY() const {
        return y;
    }

    void setY(double y) {
        Point2D::y = y;
    }

    bool operator==(const Point2D &rhs) const {
        return std::tie(x, y) == std::tie(rhs.x, rhs.y);
    }

    bool operator!=(const Point2D &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const Point2D &rhs) const {
        return std::tie(x, y) < std::tie(rhs.x, rhs.y);
    }

    bool operator>(const Point2D &rhs) const {
        return rhs < *this;
    }

    bool operator<=(const Point2D &rhs) const {
        return !(rhs < *this);
    }

    bool operator>=(const Point2D &rhs) const {
        return !(*this < rhs);
    }

    double distance(const Point2D &pt) const {
        return sqrt(pow(pt.x - x, 2) + pow(pt.y - y, 2));
    }


    friend std::ostream &operator<<(std::ostream &os, const Point2D &d) {
        os << "x: " << d.x << " y: " << d.y;
        return os;
    }

    Point2D operator*(double scalar) const {
        return {x * scalar, y * scalar};
    }

    Point2D operator+(const Point2D &pt) const {
        return {x + pt.x, y + pt.y};
    }


    Point2D operator-(const Point2D &pt) const {
        return {x - pt.x, y - pt.y};
    }

private:
    double x, y;

};

class Rect {
public:
    Rect();

    Rect(const Point2D &pMin, const Point2D &pMax) : pMin(pMin), pMax(pMax) {}

    static double distance(Rect rect, Point2D p) {
        double dx = std::max(std::max(rect.getPMin().getX() - p.getX(), p.getX() - rect.getPMax().getX()), 0.);
        double dy = std::max(std::max(rect.getPMin().getY() - p.getY(), p.getY() - rect.getPMax().getY()), 0.);

        return sqrt(pow(dx, 2) + pow(dy, 2));
    }

    const Point2D &getPMin() {
        return pMin;
    }

    void setPMin(const Point2D &pMin) {
        Rect::pMin = pMin;
    }

    const Point2D &getPMax() {
        return pMax;
    }

    void setPMax(const Point2D &pMax) {
        Rect::pMax = pMax;
    }

    bool contains(const Point2D &pt) {
        if (pt.getX() >= pMin.getX() && pt.getX() <= pMax.getX() && pt.getY() >= pMin.getY() &&
            pt.getY() <= pMax.getY()) {
            return true;
        }
        return false;
    }

    std::vector<Rect> split() {
        Point2D center = (pMax - pMin) * 0.5;
        Rect r1 = Rect(pMin + Point2D(0, center.getY()), pMin + Point2D(0, center.getY()) + center);
        Rect r2 = Rect(pMin + center, pMax);
        Rect r3 = Rect(pMin, center + pMin);
        Rect r4 = Rect(pMin + Point2D(center.getX(), 0), pMin + Point2D(center.getX(), 0) + center);
        return {r1, r2, r3, r4};
    }

    friend std::ostream &operator<<(std::ostream &os, const Rect &rect) {
        os << "pMin: " << rect.pMin << " pMax: " << rect.pMax;
        return os;
    }

private:
    Point2D pMin, pMax;


};

class Particle {
public:
    Particle();

    Particle(const Point2D &position) : position(position), velocity(Point2D(0, 0)) {}

    Particle(const Point2D &position, const Point2D &velocity) : position(position), velocity(velocity) {}

    Point2D &getPosition() {
        return position;
    }

    void setPosition(const Point2D &position) {
        Particle::position = position;
    }

    const Point2D &getVelocity() const {
        return velocity;
    }

    void setVelocity(const Point2D &velocity) {
        Particle::velocity = velocity;
    }

    void updatePosition(double timestep, const Rect &board) {

    }

    friend std::ostream &operator<<(std::ostream &os, const Particle &particle) {
        os << "position: " << particle.position << " velocity: " << particle.velocity;
        return os;
    }

private:
    Point2D position;
    Point2D velocity;

};



//class QuadNode;

class QuadNode {
    std::vector<Particle> particles;

    Rect limits;

    QuadNode *parent;
    bool isLeaf = true;

public:
    QuadNode *children[4]; // NW, NE, SW, SE

    explicit QuadNode(const Rect &limits) : limits(limits) {}

    Rect getLimits() {
        return limits;
    }

    void setLimits(const Rect &limits) {
        QuadNode::limits = limits;
    }

    void insert(Particle &p) {
        particles.push_back(p);
    }

    void updateNode() {

    }

    QuadNode *getParent() const {
        return parent;
    }

    void setParent(QuadNode *parent) {
        QuadNode::parent = parent;
    }

    bool isLeafNode() const {
        return isLeaf;
    }

    void setIsLeaf(bool isLeaf) {
        QuadNode::isLeaf = isLeaf;
    }

    size_t particleCount() {
        return particles.size();
    }

    std::vector<Particle> &getParticles() {
        return particles;
    }
};

class QuadTree {
private:
    Rect limits;
    QuadNode *root;
    size_t bucketSize = 1;

    struct KNNTreePair {
        KNNTreePair(QuadNode *_node, Point2D _query) {
            distToQuery = Rect::distance(_node->getLimits(), _query);
            node = _node;
        }
//        KNNTreePair(double distToQuery, QuadNode *node) : distToQuery(distToQuery), node(node) {}

        double distToQuery;
        QuadNode *node;

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
        KNNParticlePair(Particle particle, Point2D query) : particle(particle) {
            distToQuery = query.distance(particle.getPosition());
        }

//        KNNParticlePair(double distToQuery, Particle particle) : distToQuery(distToQuery), particle(particle) {}

        double distToQuery;
        Particle particle;

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

    void insert(QuadNode *node, Particle &p) {
        if (node->isLeafNode() && node->particleCount() >= bucketSize) {
            // add to particles but overflows
            node->insert(p);

            // create 4 regions and link to parent
            auto splitRegions = node->getLimits().split();
            auto *nw = new QuadNode(splitRegions[0]);
            auto *ne = new QuadNode(splitRegions[1]);
            QuadNode *sw = new QuadNode(splitRegions[2]);
            QuadNode *se = new QuadNode(splitRegions[3]);
            node->children[0] = nw;
            node->children[1] = ne;
            node->children[2] = sw;
            node->children[3] = se;

            // insert the particles in the children
            for (Particle &childParticle: node->getParticles()) {
                if (nw->getLimits().contains(childParticle.getPosition())) {
                    insert(nw, childParticle);
                } else if (ne->getLimits().contains(childParticle.getPosition())) {
                    insert(ne, childParticle);
                } else if (sw->getLimits().contains(childParticle.getPosition())) {
                    insert(sw, childParticle);
                } else if (se->getLimits().contains(childParticle.getPosition())) {
                    insert(se, childParticle);
                } else {
                    throw std::runtime_error("Child has no candidate node.");
                }
            }
            node->getParticles().clear();
            node->setIsLeaf(false);
        } else if (!node->isLeafNode()) {
            if (node->children[0]->getLimits().contains(p.getPosition())) {
                insert(node->children[0], p);
            } else if (node->children[1]->getLimits().contains(p.getPosition())) {
                insert(node->children[1], p);
            } else if (node->children[2]->getLimits().contains(p.getPosition())) {
                insert(node->children[2], p);
            } else if (node->children[3]->getLimits().contains(p.getPosition())) {
                insert(node->children[3], p);
            } else {
                throw std::runtime_error("Child has no candidate node.");
            }
        } else {
            // just add particle
            node->insert(p);
        }

    }
//
//    void knn(QuadNode *node, Point2D query, size_t k, std::priority_queue<KNNPair> &heap) {
//
//    }
public:


    explicit QuadTree(const Rect &limits) : limits(limits) {}

    void insert(std::vector<Particle> &particles) {
        for (Particle &p: particles) {
            if (root == nullptr) {
                root = new QuadNode(limits);
                root->insert(p);
            } else {
                insert(root, p);
            }
        }
    }

    std::vector<Particle> knn(Point2D query, size_t k) {
        // best-first search the leaves and prune
        if (root == nullptr || k == 0) {
            return {};
        }
        std::priority_queue<KNNParticlePair, std::vector<KNNParticlePair>, std::less<>> maxHeap;
        std::priority_queue<KNNTreePair, std::vector<KNNTreePair>, std::greater<>> pq;
        pq.push(KNNTreePair(root, query));
        while (!pq.empty()) {
            KNNTreePair curr = pq.top();
            pq.pop();
            if (!curr.node->isLeafNode()) {
                // cant prune
                if (maxHeap.size() < k) {
                    pq.push(KNNTreePair(curr.node->children[0], query));
                    pq.push(KNNTreePair(curr.node->children[1], query));
                    pq.push(KNNTreePair(curr.node->children[2], query));
                    pq.push(KNNTreePair(curr.node->children[3], query));
                }
                    // can prune, if its further than the worst nearest no need to check
                else {
                    if (Rect::distance(curr.node->children[0]->getLimits(), query) < maxHeap.top().distToQuery) {
                        pq.push(KNNTreePair(curr.node->children[0], query));
                    } else if (Rect::distance(curr.node->children[1]->getLimits(), query) < maxHeap.top().distToQuery) {
                        pq.push(KNNTreePair(curr.node->children[1], query));
                    } else if (Rect::distance(curr.node->children[2]->getLimits(), query) < maxHeap.top().distToQuery) {
                        pq.push(KNNTreePair(curr.node->children[2], query));
                    } else if (Rect::distance(curr.node->children[3]->getLimits(), query) < maxHeap.top().distToQuery) {
                        pq.push(KNNTreePair(curr.node->children[3], query));
                    }
                }
            } else {
                for (Particle &p: curr.node->getParticles()) {
                    if (maxHeap.size() < k) {
                        maxHeap.push(KNNParticlePair(p, query));
                    } else if (maxHeap.top().distToQuery > query.distance(p.getPosition())) {
                        maxHeap.pop();
                        maxHeap.push(KNNParticlePair(p, query));
                    }
                }
            }

        }
        std::vector<Particle> topK;
        while (!maxHeap.empty()) {
            topK.push_back(maxHeap.top().particle);
            maxHeap.pop();
        }
        return topK;
    }


};


int main() {
//    auto a = Rect(Point2D(0, 0), Point2D(100, 100));
//    auto res = a.split();
//    for (auto i: res) {
//        std::cout << i << std::endl;
//    }
//    std::cout << "Split NW" << std::endl;
//    auto res2 = res[0].split();
//    for (auto i: res2) {
//        std::cout << i << std::endl;
//    }
    std::vector<Particle> particles = {Particle(Point2D(35, 42)), Particle(Point2D(52, 10)), Particle(Point2D(62, 77)),
                                       Particle(Point2D(82, 65)), Particle(Point2D(5, 45))};
    QuadTree quadTree = QuadTree(Rect(Point2D(0, 0), Point2D(100, 100)));
    quadTree.insert(particles);
    auto res = quadTree.knn(Point2D(51, 10), 2);
    for (auto e: res) {
        std::cout << e << std::endl;
    }
    return 0;
}
