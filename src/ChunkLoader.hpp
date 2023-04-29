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
#include <math.h>
#include <mutex>
#include <filesystem>
#include <bitset>
#include <cstring>

// Header Files
#include "Block.hpp"
#include "Inventory.hpp"
#include "perlin.hpp"

union BlockInfo {
    struct {
        uint64_t attr : 8;
        uint64_t id : 7;
        uint64_t z : 20;
        uint64_t x : 20;
        uint64_t y : 7;
        uint64_t z_ : 1;
        uint64_t x_ : 1;
    } bits;
    uint64_t result;
};
class ChunkLoader {
private:

    constexpr static int water_level = 5;
public:
    static std::mutex past;
    static std::mutex present;
    static std::mutex future;
    static void writeFile(glm::vec3 position, int block_id, int x, int z);
    static void deleteBlock(glm::vec3 block, int block_id, const std::string& file);
    static void readFile(std::string file, std::unordered_set<Block> &);
    static std::string findFile(int x, int z, bool true_file);
    static bool checkFile(std::string file);
    static void placeCube(glm::vec3 position, int block_type);
    static void updateTerrain(int start_pos_x, int start_pos_z);
    static void updateChunk(int relative_x, int relative_z);
};

void ChunkLoader::writeFile(glm::vec3 position, int block_id, int x, int z) {
    // // NEG        Y          X          Z          ID        ATTR/RESERVED
    // // 63 -- 62 | 61 -- 55 | 54 -- 35 | 34 -- 15 | 14 -- 8 | 7 -- 0
    std::string file = findFile(x, z, false);
    std::string line;
    std::fstream  ofs(file, std::ios::app | std::ios::binary);
    if(!ofs.is_open()) std::cerr << "Save file not open! " << file << std::endl;
    uint64_t x_ = (x < 0) ? 1 : 0;
    uint64_t z_ = (z < 0) ? 1 : 0;
    uint64_t y = (int)round(position.y);
    uint64_t attr = 0;
    x = abs(x);
    z = abs(z);
    BlockInfo encode_b{attr, (uint64_t)block_id, static_cast<uint64_t>(z), static_cast<uint64_t>(x), y, z_, x_};
    std::bitset<64> binary_value(encode_b.result);
    ofs.write((char *)&encode_b, sizeof(BlockInfo));
    ofs.close();
    if (!ofs.good()) {
        std::cerr << "Error occurred at writing time!" << std::endl;
    }
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
void ChunkLoader::readFile(std::string file, std::unordered_set<Block> & p) {
    // std::lock_guard<std::mutex> guard(future);
    std::vector<std::string > chunks;
    glm::vec3 position;
    std::string l, data;
    std::vector<float> line_data;
    BlockInfo decode_b;
    std::ifstream ifs(file, std::ios::out | std::ios::binary);
    if(!ifs.is_open())
        std::cerr << "Cannot Read File: " << file << std::endl;

    while (ifs.read((char *)&decode_b, sizeof (decode_b))) {
        position.x = ((decode_b.bits.x_) ? (int)decode_b.bits.x * -1 : decode_b.bits.x);
        position.y = decode_b.bits.y;
        position.z = ((decode_b.bits.z_) ? (int)decode_b.bits.z * -1 : decode_b.bits.z);
        p.insert(Block(position, (int) decode_b.bits.id));
        std::memset(&decode_b, 0, sizeof(decode_b));
    }
    // future.unlock();
    if (ifs.eof()) {
        std::cout << "Reached the end of the file." << std::endl;
    } else if (ifs.fail()) {
        std::cerr << "Failed to read the file." << std::endl;
    }
    ifs.close();
    return;
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
    file.append(".bin");
    return file;
}

bool ChunkLoader::checkFile(std::string path) {
    std::filesystem::path file{path};
    return std::filesystem::exists(file);
}

void ChunkLoader::placeCube(glm::vec3 position, int block_type) {
    std::unordered_set<Block>::iterator ip;
    position.x = (float )round(position.x);
    position.y = (float )round(position.y);
    position.z = (float )round(position.z);

    writeFile(position, block_type, position.x, position.z);
}

void ChunkLoader::updateTerrain(int start_pos_x, int start_pos_z) {
    float h = perlin((float)(start_pos_x - 20) * 0.15f, (float)(start_pos_z - 20) * 0.15f);
    if (h > water_level)
        placeCube(glm::vec3(start_pos_x, h, start_pos_z), GRASS);
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
std::mutex ChunkLoader::future;
#endif
