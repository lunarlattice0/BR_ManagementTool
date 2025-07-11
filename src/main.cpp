#include "brlmt.hpp"
#include <filesystem>
#include <iostream>
#include <memory>

int main() {
    if (!std::filesystem::exists("./libbrlmt_payload.dll")) {
        std::cerr << "Missing brlmt_payload.dll" << std::endl;
        return 1;
    }

    std::cout << "Injecting to Brick Rigs, please wait..." << std::endl;
    std::unique_ptr<Game> gamePtr = std::make_unique<Game>();
    std::cout << "Injection success!" << std::endl;
    return 0;
}
