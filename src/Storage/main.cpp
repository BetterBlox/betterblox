//
// Created by Jason Choi on 3/4/23.
//
#include <iostream>
#include <fstream>
#include "../../../../CLionProjects/BBX_Storage/block.h"
#include <string>

using namespace std;
int main(){
    block block;
    int block_id = 0;

string file="Chunk";
    for(int x = 0; x < 16; x++){
//    for(int x = 0; x > -16; x--){
        for(int y = 0; y < 16; y++){
            for(int z = 0; z < 10; z++){
                file.append("(");
                if(x < 0) file.append(to_string((x - 16)/16));
                else file.append(to_string(x/16));
                file.append(",");
                if(x < 0) file.append(to_string((y - 16)/16));
                else file.append(to_string(y/16));
                file.append(")");
                block.write_file(file, x, y, z, block_id);
                file="Chunk";
            }
        }
    }
    vector<vector<int> > chunk;

//    for(int x = 0; x > -16; x--){
//    for(int x = 0; x < 16; x++){
//        for(int y = 0; y < 16; y++){
    int x  = 13;
    int y = 12;
            file.append("(");
            if(x < 0) file.append(to_string((x - 16)/16));
            else file.append(to_string(x/16));
            file.append(",");
            if(x < 0) file.append(to_string((y - 16)/16));
            else file.append(to_string(y/16));
            file.append(")");
            chunk = block.read_file(file);
            file="Chunk";
//        }
//    }

cout << chunk.size() << endl;
    for(int i = 0; i < chunk.size(); i++){
        for(int j = 0; j < chunk[i].size(); j++){
            cout << chunk[i][j] << ' ';
        }cout << endl;
    }
    return 0;
}
