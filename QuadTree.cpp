#include "QuadTree.h"

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

        // create 4 regions and link to parent
        subdivide();

        // insert the particles in the children
        for (const std::shared_ptr<Particle> &childParticle: particles) {
            if (children[0]->boundary.contains(childParticle->getPosition())) {
                children[0]->insert(childParticle);
            } else if (children[1]->boundary.contains(childParticle->getPosition())) {
                children[1]->insert(childParticle);
            } else if (children[2]->boundary.contains(childParticle->getPosition())) {
                children[2]->insert(childParticle);
            } else if (children[3]->boundary.contains(childParticle->getPosition())) {
                children[3]->insert(childParticle);
            } else {
                throw std::runtime_error("Child has no candidate node.");
            }
        }
        clearParticles();
        _isLeaf = false;
    } else if (!_isLeaf) {
        if (children[0]->boundary.contains(particle->getPosition())) {
            children[0]->insert(particle);
        } else if (children[1]->boundary.contains(particle->getPosition())) {
            children[1]->insert(particle);
        } else if (children[2]->boundary.contains(particle->getPosition())) {
            children[2]->insert(particle);
        } else if (children[3]->boundary.contains(particle->getPosition())) {
            children[3]->insert(particle);
        } else {
//            throw std::runtime_error("Child has no candidate node.");
            return false;
        }
    } else {
        // just add particle
        addToBucket(particle);
    }
    return true;
//    return insert(std::shared_ptr<QuadNode>(this), particle);
}

void QuadNode::updateNode() {

}

void QuadNode::addToBucket(const std::shared_ptr<Particle> &particle) {
    particles.push_back(particle);
}
