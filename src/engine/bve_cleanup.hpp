#pragma once

#include "bve_window.hpp"
#include "bve_device.hpp"
namespace bve
{ 
    struct CleanupList  //A list of objects that will need to have cleanup code called
    {
        Device *device;    
    };

    void cleanup(CleanupList* list);
}
