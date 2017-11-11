#include "utils.h"
#include <fstream>
#include <sstream>

std::string readFile(const std::string& path) {
    std::ifstream stream(path);
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return buffer.str();
}
