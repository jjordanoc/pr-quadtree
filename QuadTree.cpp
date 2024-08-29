#include "QuadTree.h"

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

bool QuadNode::insert(std::shared_ptr<QuadNode> node, const std::shared_ptr<Particle> &p) {
    if (node->_isLeaf && node->getParticles().size() >= 1) {
        // add to particles but overflows
        node->addToBucket(p);

        // create 4 regions and link to parent
        subdivide();

        // insert the particles in the children
        for (const std::shared_ptr<Particle> &childParticle: node->getParticles()) {
            if (children[0]->boundary.contains(childParticle->getPosition())) {
                insert(children[0], childParticle);
            } else if (children[1]->boundary.contains(childParticle->getPosition())) {
                insert(children[1], childParticle);
            } else if (children[2]->boundary.contains(childParticle->getPosition())) {
                insert(children[2], childParticle);
            } else if (children[3]->boundary.contains(childParticle->getPosition())) {
                insert(children[3], childParticle);
            } else {
                throw std::runtime_error("Child has no candidate node.");
            }
        }
        node->clearParticles();
        node->_isLeaf = false;
    } else if (!node->isLeaf()) {
        if (node->children[0]->boundary.contains(p->getPosition())) {
            insert(node->children[0], p);
        } else if (node->children[1]->boundary.contains(p->getPosition())) {
            insert(node->children[1], p);
        } else if (node->children[2]->boundary.contains(p->getPosition())) {
            insert(node->children[2], p);
        } else if (node->children[3]->boundary.contains(p->getPosition())) {
            insert(node->children[3], p);
        } else {
//            throw std::runtime_error("Child has no candidate node.");
            return false;
        }
    } else {
        // just add particle
        node->addToBucket(p);
    }
    return true;

}

bool QuadNode::insert(const std::shared_ptr<Particle> &particle) {
    return insert(std::shared_ptr<QuadNode>(this), particle);
}

void QuadNode::updateNode() {

}

void QuadNode::addToBucket(const std::shared_ptr<Particle> &particle) {
    particles.push_back(particle);
}
