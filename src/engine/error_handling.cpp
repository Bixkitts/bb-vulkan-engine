#include "error_handling.h"
#include <stdio.h>

static const char *errorMessages[BB_ERROR_TYPE_COUNT] = 
{
    "No error\n",
    "Unknown error\n",
    "Memory error, most likely failed allocation\n",
    "Descriptor pool failure\n",
    "Descriptor failure\n",
    "Descriptor set failure\n",
    "Descriptor layout failure\n",
    "Shader module failure\n",
    "Pipeline creation failure\n",
    "Pipeline layout creation failure\n",
    "Image creation failure\n",
    "Image view creation failure\n",
    "Image sampler creation failure\n",
    "Image layout transition failure\n",
    "GPU Buffer creation failure\n",
    "Command buffer recording failure\n",
    "Command buffer creation failure\n",
    "Command buffer run failure\n",
    "Swap chain image acquisition error\n"
};

void pBBError(BBError error)
{
    const char *errorString = errorMessages[error];
    fprintf(stderr, "\nEncountered error: ");
    fprintf(stderr, "%s", errorString);
}
