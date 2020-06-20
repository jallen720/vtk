#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform matrixes
{
    mat4 MVP;
}
Matrixes;

layout(location = 0) in vec3 InPosition;
layout(location = 1) in vec4 InColor;
layout(location = 0) out vec4 OutColor;

void
main()
{
    gl_Position = Matrixes.MVP * vec4(InPosition, 1);
    OutColor = InColor;
}
