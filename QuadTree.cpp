#include "QuadTree.h"
#include <queue>

size_t QuadTree::bucketSize = 6;

void QuadTree::updateTree() {
    root->updateNode();
}

const std::shared_ptr<QuadNode> &QuadTree::getRoot() const { return root; }

void QuadTree::insert(const std::vector<std::shared_ptr<Particle>> &particles) {
    for (const auto &particle: particles) {
        root->insert(particle);
    }
}

std::vector<std::shared_ptr<Particle>> QuadTree::knn(Point2D query, size_t k) {
    // best-first search the leaves and prune
    if (root == nullptr || k == 0) {
        return {};
    }
    std::priority_queue<KNNParticlePair, std::vector<KNNParticlePair>, std::less<>> maxHeap;
    std::priority_queue<KNNTreePair, std::vector<KNNTreePair>, std::greater<>> pq;
    pq.emplace(root, query);
    while (!pq.empty()) {
        KNNTreePair curr = pq.top();
        pq.pop();
        if (!curr.node->isLeaf()) {
            auto children = curr.node->getChildren();
            // cant prune
            if (maxHeap.size() < k) {
                for (const auto &child: children) {
                    pq.emplace(child, query);
                }
            } else {
                // can prune, if its further than the worst nearest no need to check
                for (const auto &child: children) {
                    if (child->getBoundary().distance(query).getValue() <= maxHeap.top().distToQuery) {
                        pq.emplace(child, query);
                    }
                }
            }
        } else {
            for (const std::shared_ptr<Particle> &p: curr.node->getParticles()) {
                if (maxHeap.size() < k) {
                    maxHeap.emplace(p, query);
                } else if (maxHeap.top().distToQuery > query.distance(p->getPosition()).getValue()) {
                    maxHeap.pop();
                    maxHeap.emplace(p, query);
                }
            }
        }

    }

    std::vector<std::shared_ptr<Particle>> topK;
//    std::cout << "max in knn is: " << maxHeap.top().distToQuery << std::endl;
//    topK.reserve(k);
    while (!maxHeap.empty()) {
        topK.push_back(maxHeap.top().particle);
        maxHeap.pop();
    }
//    std::reverse(topK.begin(), topK.end());
    return topK;
}

void QuadNode::subdivide() {
    auto splitRegions = boundary.split();
    std::shared_ptr<QuadNode> nw = std::make_shared<QuadNode>(splitRegions[0], this);
    std::shared_ptr<QuadNode> ne = std::make_shared<QuadNode>(splitRegions[1], this);
    std::shared_ptr<QuadNode> sw = std::make_shared<QuadNode>(splitRegions[2], this);
    std::shared_ptr<QuadNode> se = std::make_shared<QuadNode>(splitRegions[3], this);
    children[0] = nw;
    children[1] = ne;
    children[2] = sw;
    children[3] = se;
}

bool QuadNode::insert(const std::shared_ptr<Particle> &particle) {
    if (_isLeaf && particles.size() >= QuadTree::bucketSize) {
        // add to particles but overflows
        addToBucket(particle);

        // create 4 regions asand link to parent
        subdivide();

        auto particlesCopy = particles;

        // update current node
        clearParticles();
        _isLeaf = false;

        // insert the particles in the children
        for (const std::shared_ptr<Particle> &childParticle: particlesCopy) {
            bool didInsert = false;
            for (const auto &child: children) {
                if (child->boundary.contains(childParticle->getPosition())) {
                    child->insert(childParticle);
                    didInsert = true;
                }
            }
            if (!didInsert) {
                relocateParticle(childParticle);
            }
        }


    } else if (!_isLeaf) {
        bool didInsert = false;
        for (const auto &child: children) {
            if (child->boundary.contains(particle->getPosition())) {
                child->insert(particle);
                didInsert = true;
            }
        }
        if (!didInsert) {
            relocateParticle(particle);
        }
    } else {
        // just add particle
        addToBucket(particle);
    }
    return true;
}

void QuadNode::updateNode() {
    if (!_isLeaf) {
        // update children
        for (const auto &child: children) {
            child->updateNode();
        }
    } else {
        for (size_t i = 0; i < particles.size(); ++i) {
            auto particle = particles[i];
            if (!boundary.contains(particle->getPosition())) {
                // remove from leaf
                particles.erase(particles.begin() + i);
                // if empty, collapse
//                if (particles.empty()) {
//                    removeEmptyNode();
//                }
                // relocate recursively
                relocateParticle(particle);
                // might split
                updateNode();

            }
        }
    }


}

void QuadNode::addToBucket(const std::shared_ptr<Particle> &particle) {
    particles.push_back(particle);
}

void QuadNode::relocateParticle(const std::shared_ptr<Particle> &particle) {
    // note: root always contains all particles
    if (boundary.contains(particle->getPosition())) {
        insert(particle);
    } else {
        parent->relocateParticle(particle);
    }
}

void QuadNode::removeEmptyNode() {
    // if only one brother exists and its a leaf, collapse recursively
    QuadNode *onlyBrother = nullptr;
    for (const auto &smartBrother: parent->children) {
        QuadNode *brother = smartBrother.get();
        if (brother == this || brother == nullptr) continue;
        // cant collapse if non leaf brother exists or if another leaf brother exists
        if (!brother->_isLeaf || onlyBrother != nullptr) return;
        onlyBrother = brother;
    }
    parent = onlyBrother;
    parent->removeEmptyNode();
}


