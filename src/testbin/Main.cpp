#include "bb-renderer-core.h"
// STD
#include <cstdlib>
#include <iostream>
int main()
{
    try{
        runAppWithWindow(openWindow(1024, 768, "BveWindow"));
    }
    catch (const std::exception &e){
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
