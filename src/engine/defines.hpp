#ifndef BVE_DEFINES
#define BVE_DEFINES

#define BBAPI __attribute((visibility("default")))

// higher numbers waste more memory

#define SAMPLER_PER_IMAGE       4 // Upper limit for samplers on one image
#define VIEW_PER_IMAGE          4 // Upper limit for views on one image
#define MAX_BINDINGS_PER_LAYOUT 8
#endif
