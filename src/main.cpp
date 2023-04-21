#include "BetterBlox.hpp"

int main() {
    try {
        BetterBlox game;
        game.run();
    }
    catch (std::runtime_error &err) {
        std::cerr << "==> Exception Thrown <==" << std::endl;
        std::cerr << "Message:  " << err.what() << std::endl;
        std::exit(-1);
    }
}
