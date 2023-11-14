#include "model.hpp"

// Placeholder for actually loading a model.
// I should also have functions for generating
// simple primitives here.
Model *loadModel(char *dir)
{
    // Placeholder quad instead of model loading
    printf("\nLoading models....");
    Model *model = new Model;
    model->vertices={
        {{-0.5f, -0.5f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f},  {0.0f, 0.0f}},
        {{0.5f, 0.5f},   {0.0f, 1.0f}},
        {{-0.5f, 0.5f},  {1.0f, 1.0f}}
    };
    return model;
}
