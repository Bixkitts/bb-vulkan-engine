#include "bb-renderer-core.h"
// STD
#include <cstdlib>
#include <iostream>
int main()
{
    try{
        BBWindow window = NULL;
        openWindow       (&window, 1024, 768, "BveWindow");
        runAppWithWindow (window);
    }
    catch (const std::exception &e){
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
