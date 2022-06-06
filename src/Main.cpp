#include "bve_window.hpp"
#include "first_app.hpp"
#include "bve_pipeline.hpp"

//std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

using namespace bve;

int main()
{

    try
    {
        runAppWithWindow(openWindow(800, 600, "BveWindow"));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}
