#ifndef BVE_DRAW_FRAME
#define BVE_DRAW_FRAME

#include <glm/matrix.hpp>

#include "entity.h"
#include "swap_chain.hpp"

// TODO: stdlib shit... uuuurgh
BBError drawFrame           (const SwapChain swapchain, 
                             const BBEntityArray entities,
                             const uint64_t entityCount);
#endif
