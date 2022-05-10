
#pragma once

#include <vector>
#include <string>
#include <fstream>

namespace io
{
    std::vector<char> readFile(std::string const &filename, std::ios_base::openmode const &traits);
}
