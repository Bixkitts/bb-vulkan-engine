#version 450

layout (location = 0) in vec2 outPosition; 
layout (location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(0.5+outPosition.x, 1.0+outPosition.x, 0.0+outPosition.x, 1.0);
}
