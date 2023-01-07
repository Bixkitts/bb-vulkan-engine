#include "bve_model.hpp"

#include <cassert>
#include <vulkan/vulkan_core.h>
#include <cstring>
#include <iostream>

namespace bve
{
    std::vector<Model*> loadModels(Device* device)
    {
        std::cout<<"Loading models....\n";

        //placeholder model loader
        Model *model1 = new Model
        {
            {{-0.5f, -0.5f}},
            {{0.5f, -0.5f}},
            {{-0.5f, 0.5f}},
            {{0.5f, 0.5f}},
            {{0.5f, -0.5f}},
           {{-0.5f, 0.5f}}
        };
        //placeholder model loader
        Model *model2 = new Model
        {
            {{-0.9f, -0.9f}},
            {{-0.9f, -0.8f}},
            {{-0.8f, -0.85f}}
        };

        
        std::vector<Model*> models;
        models.push_back(model1); 
        models.push_back(model2); 
        
        return models;
    }


}
