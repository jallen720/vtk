#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 1, std140) uniform index
{
    ivec4 Data;
}
Index;
layout(set = 1, binding = 0) uniform sampler2D ColorTexture;
layout(set = 2, binding = 0, std140) uniform color
{
    vec4 Data;
}
Colors[2];

layout(location = 0) in vec2 InUV;
layout(location = 0) out vec4 OutColor;

void
main()
{
    OutColor = /* texture(ColorTexture, InUV) *  */Colors[Index.Data.x].Data;
}
