#include "model.hpp"

static BBError initModel(Model model, uint32_t vertexCount, uint32_t indexCount);

static BBError initModel(Model model, uint32_t vertexCount, uint32_t indexCount)
{
    model->vertices = (Vertex*)malloc(sizeof(Vertex) * vertexCount);
    if (model->vertices == NULL) {
        return BB_ERROR_MEM;
    }
    model->indeces  = (uint32_t*)malloc(sizeof(uint32_t) * indexCount);
    if (model->indeces == NULL) {
        free (model->vertices);
        return BB_ERROR_MEM;
    }
    return BB_ERROR_OK;
}
// Placeholder for actually loading a model.
// I should also have functions for generating
// simple primitives here.
BBError loadModel(Model outModel, const char *dir)
{
    BBError er       = BB_ERROR_OK;
    Vertex *verts    = NULL;
    
    // TODO:
    // Actually load a model instead of a quad
    printf    ("\nLoading models....\n");
    er =
    initModel (outModel, 4, 4);
    verts = outModel->vertices;
    verts[0].position = {-0.5f, -0.5f};
    verts[0].texCoord = {1.0f, 0.0f};
    verts[1].position = {0.5f, -0.5f};
    verts[1].texCoord = {0.0f, 0.0f};
    verts[2].position = {0.5f, 0.5f};
    verts[2].texCoord = {0.0f, 1.0f};
    verts[3].position = {-0.5f, 0.5f};
    verts[3].texCoord = {1.0f, 1.0f};
    return er;
}

void destroyModel (Model model)
{
    if (model->vertices != NULL) {
        free (model->vertices);
    }
    if (model->indeces != NULL) {
        free (model->indeces);
    }
    return;
}
