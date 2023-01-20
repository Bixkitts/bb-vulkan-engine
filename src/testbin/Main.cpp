#include "bb-engine.h"

//std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

using namespace bve;

int main()
{
    try
    {
        runAppWithWindow(openWindow(1024, 768, "BveWindow"));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}
