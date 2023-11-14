#include "model.hpp"

// Placeholder for actually loading a model.
// I should also have functions for generating
// simple primitives here.
BBError loadModel(Model *model, char *dir)
{
    // TODO:
    // Placeholder quad instead of model loading
    printf("\nLoading models....");
    // TODO: MALLOC, NOT FREED
    model->vertices = (Vertex*)malloc(sizeof(Vertex) * 4);
    if (model->vertices == NULL)
        return BB_ERROR_MEM;
    Vertex *verts = model->vertices;
    verts[0].position = {-0.5f, -0.5f};
    verts[0].texCoord = {1.0f, 0.0f};
    verts[1].position = {0.5f, -0.5f};
    verts[1].texCoord = {0.0f, 0.0f};
    verts[2].position = {0.5f, 0.5f};
    verts[2].texCoord = {0.0f, 1.0f};
    verts[3].position = {-0.5f, 0.5f};
    verts[3].texCoord = {1.0f, 1.0f};
    return BB_ERROR_OK;
}
