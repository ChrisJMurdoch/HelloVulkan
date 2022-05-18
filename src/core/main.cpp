
#include "display/display.hpp"

#include <iostream>

int main()
{
    try
    {
        Display display{1000, 600, "HelloVulkan", BufferingStrategy::TripleBuffering, true};
        while (!display.shouldClose())
            display.tick();
    }
    catch (std::exception const &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cerr << "Application shut down gracefully." << std::endl;
    return EXIT_SUCCESS;
}
