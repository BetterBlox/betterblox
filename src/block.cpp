//
// Created by Jason Choi on 3/4/23.
//

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "block.h"
#include "Block.hpp"
#include <set>

//using namespace std;

void chunk::write_file(glm::vec3 position, int block_id, int render) {
    string line;
    string file = find_file(position.x, position.z);
    fstream ofs(file , ios::app);
    if(!ofs.is_open()) cerr << "Save file not open! " << file;
    unordered_set<Block> n = read_file(position.x, position.z, render);
    if(n.find(Block(position,block_id)) == n.end()) {
        ofs << position.x << "|" << position.y << "|" << position.z << "|" << block_id << endl;
    }
    ofs.close();
}

unordered_set<Block> chunk::read_file(int x, int z, int renderDistance) {
    set<string > chunks;
    unordered_set<Block> p;
    int relativex, relativez;
    if(x < 0) relativex = ((x - 16)/16);
    else relativex = (x/16);
    if(z < 0) relativez = ((z - 16)/16);
    else relativez = (z/16);

    for(int i = -renderDistance; i <= renderDistance; i++){
        for(int j = -renderDistance; j <= renderDistance; j++){
            chunks.insert(find_file((relativex + i) * 15, (relativez + j)* 15));
        }
    }
    for(auto i : chunks){
        cerr << i << ' ';
    } cerr << endl;
    for(auto i : chunks) {
        ifstream ifs(i);
        glm::vec3 position;
        vector<float> line_data;
        if (!ifs.is_open()) cerr << "Cannot Read File: " << i << endl;
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
    }
    return p;
}

string chunk::find_file(int x, int z) {
    string file="Chunk";
    file.append("(");
    if(x < 0) file.append(to_string((x - 16)/16));
    else file.append(to_string(x/16));
    file.append(",");
    if(z < 0) file.append(to_string((z - 16)/16));
    else file.append(to_string(z/16));
    file.append(")");
    file.append(".txt");
    return file;
}
bool chunk::check_file(string file) {
    ifstream ifs(file);
    return ifs.is_open();
}