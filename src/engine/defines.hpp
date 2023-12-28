#ifndef BVE_DEFINES
#define BVE_DEFINES


// higher numbers waste more memory

#define BBAPI __attribute((visibility("default")))

#define SAMPLER_PER_IMAGE       4 // Upper limit for samplers on one image
#define QUEUES_MAX              4 // Max amount of device queues
#define VIEW_PER_IMAGE          4 // Upper limit for views on one image
#define MAX_BINDINGS_PER_LAYOUT 8
// TODO: Maybe make this dynamic one day....
#define MAX_FRAMES_IN_FLIGHT    2 // Amount of frame buffers on the swap chain
#endif
