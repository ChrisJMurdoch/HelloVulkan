
#include "display/display.hpp"

#include <iostream>

int main(int argc, char **argv)
{
    bool disableValidationLayers = (argc>=2) && (strcmp(argv[1], "noval")==0);
    try
    {
        Display display{1000, 600, "HelloVulkan", BufferingStrategy::TripleBuffering, !disableValidationLayers};
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
