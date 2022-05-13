
#include "display/display.hpp"

int main()
{
    try
    {
        Display display{1000, 500};
        display.run();
    }
    catch (std::exception const &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
