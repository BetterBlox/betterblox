#include "BetterBlox.hpp"

int main() {
    try {
        BetterBlox game;
        game.run();
    }
    catch (std::runtime_error &err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return -1;
    }
}
