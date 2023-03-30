//
// Created by Jason Choi on 3/4/23.
//

#ifndef BBX_STORAGE_BLOCK_H
#define BBX_STORAGE_BLOCK_H
#include <unordered_set>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Block.hpp"
using namespace std;

class chunk {
public:
    void write_file(glm::vec3 , int, int, int);
    unordered_set<Block> read_file(string);
    static string find_file(int, int, bool);
    bool check_file(string);
    string get_chunks(int, int);
};


#endif //BBX_STORAGE_BLOCK_H
