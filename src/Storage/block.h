//
// Created by Jason Choi on 3/4/23.
//

#ifndef RUST_PROJECT_BLOCK_H
#define RUST_PROJECT_BLOCK_H

#include <vector>
#include <fstream>

using namespace std;

class block {
public:
    void write_file(string, int, int, int, int);

    vector<vector<int> > read_file(string);
};


#endif //RUST_PROJECT_BLOCK_H
