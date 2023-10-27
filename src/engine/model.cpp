#include <cassert>
#include <vulkan/vulkan_core.h>
#include <cstring>
#include <iostream>

#include "model.hpp"

// Placeholders for actually loading models from a file
std::vector<Model*> loadModels(char* dir)
{
    std::cout<<"Loading models....\n";

    //placeholder model loader
    Model *model1 = new Model;
    model1->vertices={
        {{-0.5f, -0.5f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f},  {0.0f, 0.0f}},
        {{0.5f, 0.5f},   {0.0f, 1.0f}},
        {{-0.5f, 0.5f},  {1.0f, 1.0f}}
    };
    model1->indeces={0,1,2,2,3,0};
    //placeholder model loader
    Model *model2 = new Model;
    model2->vertices={
        {{-0.9f, -0.9f}, {1.0f, 0.0f}},
        {{-0.9f, -0.8f}, {0.0f, 0.0f}},
        {{-0.8f, -0.85f}, {0.0f, 1.0f}}};
    model2->indeces={2,1,0};
    
    std::vector<Model*> models;
    models.push_back(model1); 
    models.push_back(model2); 
    
    return models;
}

Model *loadModel(char *dir)
{
    std::cout<<"Loading models....\n";
    Model *model = new Model;
    model->vertices={
        {{-0.5f, -0.5f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f},  {0.0f, 0.0f}},
        {{0.5f, 0.5f},   {0.0f, 1.0f}},
        {{-0.5f, 0.5f},  {1.0f, 1.0f}}
    };
    return model;
}
