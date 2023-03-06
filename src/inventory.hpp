#ifndef INVENTORY_H
#define INVENTORY_H
#include "Block.hpp"
#include <map>

enum {
    DIAMOND_ORE,
    CONTAINER,
    HAPPY_FACE,
    BEDROCK, 
    GRASS,
    WATER
};

class Inventory {
private:
    int maxBlocks;
    std::map<int, int> blocks;

public:
    Inventory(int startnum = 10) {
        blocks.insert({DIAMOND_ORE, startnum });
        blocks.insert({ CONTAINER, startnum });
        blocks.insert({ HAPPY_FACE, startnum });
        blocks.insert({ BEDROCK, startnum });
        blocks.insert({ GRASS, startnum });
        blocks.insert({ WATER, startnum });
    }
    int getBlockCount(int type) {
        return blocks.at(type);
    }
    void addBlock(int type) { // Adds one block at a time. 
        blocks.at(type) += 1;
    }
    // This will remove one block of a specified type from the inventory and return true if you have enough. If you do not have enough it will return false. 
    bool removeBlock(int type) {
        if (blocks.at(type) > 0) {
            blocks.at(type)--;
            return true;
        }
        return false;
    }
};

#endif