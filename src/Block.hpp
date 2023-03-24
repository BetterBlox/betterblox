#ifndef BLOCK_H
#define BLOCK_H

#include <glm/glm.hpp>
#include <cstdio>

class Block {
private:
    int blockType;
    glm::vec3 position;

public:
    Block(glm::vec3 position = glm::vec3(1), int blockType = 0) {
        this->blockType = blockType;
        this->position = position;
    }

    Block(const Block &block) {
        position = block.position;
        blockType = block.blockType;
    }

    Block operator=(const Block &block) {
        return *this;
    }

    glm::vec3 getPosition() const {
        return position;
    }

    int getBlockType() {
        return blockType;
    }

    bool operator==(const Block &rhs) const {
        return this->position.x == rhs.position.x && this->position.y == rhs.position.y &&
               this->position.z == rhs.position.z;
    }

    friend class std::hash<Block>;
};

namespace std {
    template<>
    class hash<Block> {
    public:
        std::size_t operator()(const Block &b) const {
            using std::size_t;
            using std::hash;
            using std::string;

            return ((hash<float>()(b.getPosition().x) ^ (hash<float>()(b.getPosition().y) << 1)) >> 1) ^
                   (hash<float>()(b.getPosition().z) << 1);
        }
    };
}

#endif