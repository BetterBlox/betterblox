#include "BetterBlox.hpp"
// #include "utils/RuntimeError.hpp"

int main() {
    try {
        BetterBlox game;
        game.run();
    }
    // catch (RuntimeError &err) {
    //     std::cerr << "==> Exception Thrown <==" << std::endl;
    //     std::cerr << "Message:  " << err.what() << std::endl;
    //     std::cerr << "File:  " << err.get_location().file_name() << std::endl;
    //     std::cerr << "Line:  " << err.get_location().line() << std::endl;
    //     std::exit(-1);
    // }
    catch (std::runtime_error &err) {
        std::cerr << "==> Exception Thrown <==" << std::endl;
        std::cerr << "Message:  " << err.what() << std::endl;
        std::exit(-1);
    }
}
