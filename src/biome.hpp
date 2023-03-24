#pragma once
#ifndef BIOME_H
#define BIOME_H

#include <vector>
#include <string>
#include <unordered_map>

class biome {
private:
    std::unordered_map<int, int> relatedBlocks; // The int will take the id of the blocks that are related to the landscape of the biome. The value is how populous the block is.
    std::string name;
public:
    biome(const std::string &name, std::unordered_map<int, int> blocks = std::unordered_map<int, int>()) {
        this->name = name;
        this->relatedBlocks = blocks;
    }
};

#endif BIOME_H