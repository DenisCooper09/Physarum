#include "Common.glsl"
#include "Agent.glsl"
#include "Random.glsl"

const float ANGLE = PI / 4;

float sensor_probe(Agent agent, float angle, float extent)
{
    float alpha = angle + agent.heading;

    vec2 position = vec2(sin(alpha), cos(alpha)) * extent + agent.position;

    position = mod(position, u_Resolution);

    return imageLoad(u_Image, ivec2(position)).r;
}

void main()
{
    uint i = gl_GlobalInvocationID.x;

    float right = sensor_probe(agents[i], ANGLE, 30.0f);
    float center = sensor_probe(agents[i], 0.0f, 50.0f);
    float left = sensor_probe(agents[i], -ANGLE, 30.0f);

    if (left > center && left > right)
    {
        agents[i].heading -= ANGLE;
    }
    else if (right > center && right > left)
    {
        agents[i].heading += ANGLE;
    }
    else if (left == right && left < center && right < center)
    {
        agents[i].heading += ((random(agents[i].position) - 0.5f) * 2.0f) * PI;
    }
}
