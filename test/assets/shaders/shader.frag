#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 OutColor;

void
main()
{
    OutColor = vec4(1, 1, 1, 1);
}
