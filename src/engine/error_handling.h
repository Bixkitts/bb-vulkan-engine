#ifndef BB_ERROR_HANDLING
#define BB_ERROR_HANDLING

typedef enum BBError
{
    BB_ERROR_OK,
    BB_ERROR_UNKNOWN,
    BB_ERROR_MEM,
    BB_ERROR_DESCRIPTOR_POOL,
    BB_ERROR_DESCRIPTOR,
    BB_ERROR_DESCRIPTOR_SET,
    BB_ERROR_DESCRIPTOR_LAYOUT,
    BB_ERROR_COUNT
}BBError;


#endif
