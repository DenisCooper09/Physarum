#include "Common.glsl"

void main()
{
    ivec2 coordinate = ivec2(gl_GlobalInvocationID.xy);

    float sum = 0.0f;

    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            vec2 delta = mod(vec2(x, y) + coordinate, u_Resolution);

            sum += imageLoad(u_Image, ivec2(delta)).r;
        }
    }

    vec4 color = imageLoad(u_Image, coordinate);
    color.r = sum / 9;

    imageStore(u_Image, coordinate, color);
}
