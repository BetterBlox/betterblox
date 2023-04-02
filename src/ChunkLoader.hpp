//
// Created by Jason Choi on 3/31/23.
//

#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <set>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Block.hpp"

using namespace std;

class chunk {
private:
    int waterLevel = 5;
public:
    void write_file(glm::vec3 position, int block_id, int x, int z) {
        string file = find_file(x, z, false);
        string line;
        fstream ofs(file , ios::app);
        if(!ofs.is_open()) cerr << "Save file not open! " << file << endl;
        unordered_set<Block> n = read_file(file);
        if(n.find(Block(position,block_id)) == n.end())
            ofs << position.x << "|" << position.y << "|" << position.z << "|" << block_id << endl;
        ofs.close();
    }
    void write_file(glm::vec3 position, int block_id, int x, int z, fstream &ofs) {
        string file = find_file(x, z, false);
        string line;
        if(!ofs.is_open()) cerr << "Save file not open! " << file << endl;
        unordered_set<Block> n = read_file(file);
        if(n.find(Block(position,block_id)) == n.end())
            ofs << position.x << "|" << position.y << "|" << position.z << "|" << block_id << endl;
    }

    unordered_set<Block> read_file(string file) {
        unordered_set<Block> p;
        vector<string > chunks;
        ifstream ifs(file);
        glm::vec3 position;
        vector<float> line_data;
        if(!ifs.is_open())
            cerr << "Cannot Read File: " << file << endl;
        string data, l;
        while (getline(ifs, data)) {
            istringstream iss(data);
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

    string find_file(int x, int z, bool trueFile) {
        string file="Chunk";
        file.append("(");
        if(!trueFile) {
            if (x < 0) file.append(to_string((x - 16) / 16));
            else file.append(to_string(x / 16));
            file.append(",");
            if (z < 0) file.append(to_string((z - 16) / 16));
            else file.append(to_string(z / 16));
        }
        else{
            file.append(to_string(x));
            file.append(",");
            file.append(to_string(z));
        }
        file.append(")");
        file.append(".txt");
        return file;
    }
    bool check_file(string file) {
        ifstream ifs(file);
        return ifs.is_open();
    }

    string get_chunks(int x, int z) {
        string xz = "";
        if (x < 0) xz.append(to_string((x - 16) / 16));
        else xz.append(to_string(x / 16));
        if (z < 0) xz.append(to_string((z - 16) / 16));
        else xz.append(to_string(z / 16));

    }
    void placeCube(glm::vec3 position, int blockType) {
        std::unordered_set<Block>::iterator ip;
        position.x = (float )round(position.x);
        position.y = (float )round(position.y);
        position.z = (float )round(position.z);

        write_file(position, blockType, position.x, position.z);
    }
    void placeCube(glm::vec3 position, int blockType, fstream &ofs) {
        std::unordered_set<Block>::iterator ip;
        position.x = (float )round(position.x);
        position.y = (float )round(position.y);
        position.z = (float )round(position.z);
        write_file(position, blockType, position.x, position.z,ofs);
    }

    void updateTerrain(int startPosx, int startPosz, fstream &ofs) {
        float h = perlin((float)startPosx * 0.15f, (float)startPosz* 0.15f);
        if (h > waterLevel)
            placeCube(glm::vec3(startPosx, h, startPosz), BEDROCK,ofs);
        else
            placeCube(glm::vec3(startPosx, waterLevel, startPosz), WATER,ofs);
    }
    void updateChunk(int relativex, int relativez) {
        fstream ofs(find_file(relativex, relativez, true), ios::app);
        if (relativex >= 0 && relativez >= 0) {    // first quadrant
            for (int i = relativex * 16; i < (relativex + 1) * 16; i++) {
                for (int j = relativez * 16; j < (relativez + 1) * 16; j++) {
                    updateTerrain(i, j, ofs);
                }
            }
        }
        else if (relativex < 0 && relativez >= 0) {     // second quadrant
            for (int i = (relativex + 1) * 16; i > relativex * 16; i--) {
                for (int j = relativez * 16; j < (relativez + 1) * 16; j++) {
                    if (i == 0) continue;
                    updateTerrain(i, j, ofs);
                }
            }
        }
        else if (relativex < 0 && relativez < 0) {     // third quadrant
            for (int i = (relativex + 1) * 16; i > relativex * 16; i--) {
                for (int j = (relativez + 1) * 16; j > relativez * 16; j--) {
                    if (i == 0 || j == 0) continue;
                    updateTerrain(i, j, ofs);
                }
            }
        }
        else if (relativex >= 0 && relativez < 0) {     // fourth quadrant
            for (int i = relativex * 16; i < (relativex + 1) * 16; i++) {
                for (int j = (relativez + 1) * 16; j > relativez * 16; j--) {
                    if (j == 0)continue;
                    updateTerrain(i, j, ofs);
                }
            }
        }
        ofs.close();
    }
};


#endif //CHUNKLOADER_H