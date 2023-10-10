#version 450

layout (location = 0) in vec2 fragPosition; 
layout (location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(0.5+fragPosition.x, 1.0+fragPosition.x, 0.0+fragPosition.x, 1.0);
    outColor = texture(texSampler, fragTexCoord);
}
