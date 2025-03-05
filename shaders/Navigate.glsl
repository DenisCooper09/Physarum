#include "Common.glsl"
#include "Agent.glsl"
#include "Random.glsl"

uniform float u_SensorAngle = PI / 4.0f;
uniform float u_RotateAngle = PI / 4.0f;

uniform float u_RSensorExtent = 30.0f;
uniform float u_CSensorExtent = 50.0f;
uniform float u_LSensorExtent = 30.0f;

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

    float right = sensor_probe(agents[i], u_SensorAngle, u_RSensorExtent);
    float center = sensor_probe(agents[i], 0.0f, u_CSensorExtent);
    float left = sensor_probe(agents[i], -u_SensorAngle, u_LSensorExtent);

    if (left > center && left > right)
    {
        agents[i].heading -= u_RotateAngle;
    }
    else if (right > center && right > left)
    {
        agents[i].heading += u_RotateAngle;
    }
    else if (left == right && left < center && right < center)
    {
        agents[i].heading += ((random(agents[i].position) - 0.5f) * 2.0f) * PI;
    }
}
