#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H

// Dependencies
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// STL
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

// Header Files
#include "Block.hpp"
#include "Inventory.hpp"
#include "perlin.hpp"

class ChunkLoader {
private:
    int water_level = 5;
public:
    void writeFile(glm::vec3 position, int block_id, int x, int z);
    void deleteBlock(glm::vec3 block, int block_id, const std::string& file);
    std::unordered_set<Block> readFile(std::string file);
    std::string findFile(int x, int z, bool true_file);
    bool checkFile(std::string file);
    void placeCube(glm::vec3 position, int block_type);
    void updateTerrain(int start_pos_x, int start_pos_z);
    void updateChunk(int relative_x, int relative_z);
};

void ChunkLoader::writeFile(glm::vec3 position, int block_id, int x, int z) {
    std::string file = findFile(x, z, false);
    std::string line;
    std::fstream ofs(file , std::ios::app);
    if(!ofs.is_open()) std::cerr << "Save file not open! " << file << std::endl;
    std::unordered_set<Block> n = readFile(file);
    if(n.find(Block(position,block_id)) == n.end())
        ofs << position.x << "|" << position.y << "|" << position.z << "|" << block_id << std::endl;
    ofs.close();
}

void ChunkLoader::deleteBlock(glm::vec3 block, int block_id, const std::string& file) {
    std::ifstream iof(file);
    std::string temp = "temp.txt";
    std::ofstream iofs(temp);

    if (!iof.is_open()) {
        std::cerr << "db: Cannot Read File: " << file << std::endl;
        return;
    }
    else
        std::cerr << "File Open: " << file << std::endl;
    if (!iofs.is_open()) {
        std::cerr << "db: Cannot Read File: " << temp << std::endl;
        return;
    }
    else {
        std::cerr << "File Open: "<< temp << std::endl;
    }
    std::string data;
    std::string block_string = "";
    block_string += std::to_string((int)round(block.x));
    block_string.push_back('|');
    block_string += std::to_string((int)round(block.y));
    block_string.push_back('|');
    block_string += std::to_string((int)round(block.z));
    block_string.push_back('|');
    block_string += std::to_string(block_id);
    while(getline(iof, data)) {
        if (data != block_string)
            iofs << data << std::endl;
    }
    iof.close();
    iofs.close();
    std::remove(file.c_str());
    std::rename(temp.c_str(), file.c_str());
}

std::unordered_set<Block> ChunkLoader::readFile(std::string file) {
    std::unordered_set<Block> p;
    std::vector<std::string > chunks;
    std::ifstream ifs(file);
    glm::vec3 position;
    std::vector<float> line_data;
    if(!ifs.is_open())
        std::cerr << "Cannot Read File: " << file << std::endl;
    std::string data, l;
    while (getline(ifs, data)) {
        std::istringstream iss(data);
        while (getline(iss, l, '|')) {
            line_data.push_back(stof(l));
        }
        position.x = line_data[0];
        position.y = line_data[1];
        position.z = line_data[2];
        p.insert(Block(position, (int) line_data[3]));
        line_data.clear();
    }
    ifs.close();
    return p;
}

std::string ChunkLoader::findFile(int x, int z, bool true_file) {
    std::string file="Chunk";
    file.append("(");
    if(!true_file) {
        if (x < 0) file.append(std::to_string((x - 16) / 16));
        else file.append(std::to_string(x / 16));
        file.append(",");
        if (z < 0) file.append(std::to_string((z - 16) / 16));
        else file.append(std::to_string(z / 16));
    }
    else{
        file.append(std::to_string(x));
        file.append(",");
        file.append(std::to_string(z));
    }
    file.append(")");
    file.append(".txt");
    return file;
}

bool ChunkLoader::checkFile(std::string file) {
    std::ifstream ifs(file);
    return ifs.is_open();
}

void ChunkLoader::placeCube(glm::vec3 position, int block_type) {
    std::unordered_set<Block>::iterator ip;
    position.x = (float )round(position.x);
    position.y = (float )round(position.y);
    position.z = (float )round(position.z);

    writeFile(position, block_type, position.x, position.z);
}

void ChunkLoader::updateTerrain(int start_pos_x, int start_pos_z) {
    float h = perlin((float)start_pos_x * 0.15f, (float)start_pos_z * 0.15f);
    if (h > water_level)
        placeCube(glm::vec3(start_pos_x, h, start_pos_z), BEDROCK);
    else
        placeCube(glm::vec3(start_pos_x, water_level, start_pos_z), WATER);
}

void ChunkLoader::updateChunk(int relative_x, int relative_z) {
    std::fstream ofs(findFile(relative_x, relative_z, true), std::ios::app);
    if (relative_x >= 0 && relative_z >= 0) {    // first quadrant
        for (int i = relative_x * 16; i < (relative_x + 1) * 16; i++) {
            for (int j = relative_z * 16; j < (relative_z + 1) * 16; j++) {
                updateTerrain(i, j);
            }
        }
    }
    else if (relative_x < 0 && relative_z >= 0) {     // second quadrant
        for (int i = (relative_x + 1) * 16; i > relative_x * 16; i--) {
            for (int j = relative_z * 16; j < (relative_z + 1) * 16; j++) {
                if (i == 0) continue;
                updateTerrain(i, j);
            }
        }
    }
    else if (relative_x < 0 && relative_z < 0) {     // third quadrant
        for (int i = (relative_x + 1) * 16; i > relative_x * 16; i--) {
            for (int j = (relative_z + 1) * 16; j > relative_z * 16; j--) {
                if (i == 0 || j == 0) continue;
                updateTerrain(i, j);
            }
        }
    }
    else if (relative_x >= 0 && relative_z < 0) {     // fourth quadrant
        for (int i = relative_x * 16; i < (relative_x + 1) * 16; i++) {
            for (int j = (relative_z + 1) * 16; j > relative_z * 16; j--) {
                if (j == 0)continue;
                updateTerrain(i, j);
            }
        }
    }
    ofs.close();
}

#endif
