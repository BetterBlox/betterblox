#ifndef BLOCK_H
#define BLOCK_H

#include <glm/glm.hpp>
#include <cstdio>

class Block {
private:
    int block_type;
    glm::vec3 position;

public:
    Block(glm::vec3 position = glm::vec3(1), int block_type = 0) {
        this->block_type = block_type;
        this->position = position;
    }

    Block(const Block &block) {
        position = block.position;
        block_type = block.block_type;
    }

    Block operator=(const Block &block) {
        return *this;
    }

    glm::vec3 getPosition() const {
        return position;
    }

    int getBlockType() {
        return block_type;
    }

    bool operator==(const Block &right_hand_side) const {
        return this->position.x == right_hand_side.position.x && this->position.y == right_hand_side.position.y &&
               this->position.z == right_hand_side.position.z;
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