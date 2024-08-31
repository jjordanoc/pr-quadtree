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
//                throw std::runtime_error("Child has no candidate node.");
//                std:: cout << "Child has no candidate node (leaf)." << std::endl;
//                std::cout << "Particle: " << *childParticle << std::endl;
                children[3]->insert(childParticle); // misplace anyway
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
//            std:: cout << "Child has no candidate node (not leaf)." << std::endl;
//            std::cout << "Particle: " << *particle << std::endl;
            children[3]->insert(particle); // misplace anyway
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
        children[0]->updateNode();
        children[1]->updateNode();
        children[2]->updateNode();
        children[3]->updateNode();
    } else {
//        auto it = particles.begin();
        for (size_t i = 0; i < particles.size(); ++i) {
            auto particle = particles[i];
            if (!boundary.contains(particle->getPosition())) {
                // remove from leaf
                particles.erase(particles.begin() + i);
                // if empty, collapse
                if (particles.empty()) {
                    removeEmptyNode();
                }
                // relocate recursively
                relocateParticle(particle);
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
