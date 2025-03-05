#include "Common.glsl"
#include "Agent.glsl"

uniform float u_Decay = 0.995f;

void main()
{
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    vec4 color = imageLoad(u_Image, coordinate);
    color.r *= u_Decay;
    imageStore(u_Image, coordinate, color);
}
