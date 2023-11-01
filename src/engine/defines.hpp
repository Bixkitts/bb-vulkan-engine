#ifndef BVE_DEFINES
#define BVE_DEFINES

#define BBAPI __attribute((visibility("default")))

// higher numbers waste more memory

#define SAMPLER_PER_IMAGE 4 // Upper limit for samplers on one image
#define VIEW_PER_IMAGE    4 // Upper limit for views on one image

// Unused
#define DESCRIPTOR_WRITES_PER_DRAW 4 // The amount of descriptors that
                                     // can be swapped on allocated
                                     // descriptor sets between 
                                     // each draw call
#endif
