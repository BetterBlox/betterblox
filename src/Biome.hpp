#pragma once
#ifndef BIOME_H
#define BIOME_H

#include <vector>
#include <string>
#include <unordered_map>

class Biome {
private:
    std::unordered_map<int, int> related_blocks; // The int will take the id of the blocks that are related to the landscape of the Biome. The value is how populous the block is.
    std::string name;
public:
    Biome(const std::string &name, std::unordered_map<int, int> blocks = std::unordered_map<int, int>()) {
        this->name = name;
        this->related_blocks = blocks;
    }
};

#endif BIOME_H