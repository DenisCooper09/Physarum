#include "Common.glsl"
#include "Agent.glsl"

void main()
{
    uint i = gl_GlobalInvocationID.x;

    vec2 delta = vec2(sin(agents[i].heading), cos(agents[i].heading)) * 5.0f + agents[i].position;
    delta = mod(delta, u_Resolution);

    ivec2 idelta = ivec2(delta);

    vec4 status = imageLoad(u_Image, idelta);

    if (status.g > 0 && idelta != ivec2(agents[i].position))
    {
        agents[i].heading *= -1.0f;
        return;
    }

    status.g = 0;
    status.r = 1.0f;
    imageStore(u_Image, ivec2(agents[i].position), status);

    agents[i].position = delta;

    status.g = 1;
    status.r = 1.0f;
    imageStore(u_Image, ivec2(agents[i].position), status);
}
