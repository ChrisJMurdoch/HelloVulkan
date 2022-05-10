
#include "utility/io.hpp"

#include <exception>

std::vector<char> io::readFile(std::string const &filename, std::ios_base::openmode const &traits)
{
    // Open file
    std::ifstream file(filename, traits);
    if (!file.is_open())
        throw std::exception("File not found");

    // Read file
    std::vector<char> data(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    return data;
}
