#include "Common.glsl"

layout (rgba32f, binding = 3) uniform image2D u_Ouput;

void main()
{
    ivec2 coordinates = ivec2(gl_GlobalInvocationID.xy);
    imageStore(u_Ouput, coordinates, vec4(mix(vec3(0), vec3(1), imageLoad(u_Image, coordinates).r), 1));
}
