#include "bve_window.hpp"
#include "bve_cleanup.hpp"
#include "bve_device.hpp"
namespace bve
{
// more things will... eventually be listed here!
void cleanup(CleanupList *list)
{
    destroyDevice(list->device);
    closeWindow(list->device->window);
}
}
