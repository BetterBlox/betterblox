//
// Created by Jason Choi on 3/4/23.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include "block.h"

using namespace std;

void block::write_file(string file, int x, int y, int z, int block_id) {
    vector<vector<int> > chunk = read_file(file);
    for(int i = 0; i < chunk.size(); i++){
        if(chunk[i][2] == x && chunk[i][3] == y && chunk[i][4] == z && chunk[i][5] == block_id) break;
    }
    ofstream ofs(file, ios::app);
    if(!ofs.is_open()) cerr << "Save file not open!";

    ofs << x/16 << "|" << y/16 << "|" << x << "|" << y << "|" << z << "|" << block_id << endl;
    ofs.close();
}
vector<vector<int> > block::read_file(string file) {
    ifstream ifs(file);
    vector<int> line_data;
    vector<vector<int> > chunk_data;
    if(!ifs.is_open()) cerr << "Cannot Read File!" << endl;
    string data, l;
    while(getline(ifs, data)){
        istringstream iss(data);
        while(getline(iss, l, '|')){
            line_data.push_back(stoi(l));
        }
        chunk_data.push_back(line_data);
        line_data.clear();
    }
    ifs.close();
    return chunk_data;
}
