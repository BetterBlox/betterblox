#ifndef STRUCTURES_H
#define STRUCTURES_H

// Dependencies
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Block.hpp"
#include "Inventory.hpp"

#include <cmath>
#include <vector>

class Structures {
private:
    constexpr static int treeHeight = 6;
    constexpr static int radius = 2;
public:
    static std::vector<Block> staticStructTree(glm::vec3 point, std::string file);
};

std::vector<Block> Structures::staticStructTree(glm::vec3 point, std::string file) {
    std::vector<Block> tree;
    for(int i = 1; i < treeHeight; i++){
        glm::vec3 treePosition;
        treePosition.x = point.x;
        treePosition.z = point.z;
        treePosition.y = point.y + i;
        tree.push_back(Block(treePosition, CONTAINER));
        if(i == treeHeight - 1)
        for(float x = treePosition.x - radius; x <= treePosition.x + radius; x++) {
            for(float y = treePosition.y - radius; y <= treePosition.y + radius; y++) {
                for(float z = treePosition.z - radius; z <= treePosition.z + radius; z++) {
                    // Calculate the distance between the current point and the center of the sphere
                    float distance = sqrt(pow(x - treePosition.x, 2) + pow(y - treePosition.y, 2) + pow(z - treePosition.z, 2));
                    // If the distance is less than or equal to the radius, add the point to the sphere
                    std::filesystem::path path{file};

                    glm::vec3 leaf;
                    leaf.x = x;
                    leaf.z = z;
                    leaf.y = y;
                    if(distance <= radius && std::filesystem::exists(path)) {
                        tree.push_back(Block(leaf,BEDROCK));
                    }
                }
            }
        }
    }
    return tree;
}

#endif