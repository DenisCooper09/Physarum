#include "Common.glsl"
#include "Agent.glsl"

const float DECAY = 0.995f;

void main()
{
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);
    vec4 color = imageLoad(u_Image, coordinate);
    color.r *= DECAY;
    imageStore(u_Image, coordinate, color);
}
