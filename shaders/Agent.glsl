struct Agent
{
    vec2 position;
    float heading;
    float cooldown;
    int sensor_offset;
    int sensor_number;

    float _padding1;
    float _padding2;
};

layout (std430, binding = 1) buffer Agents
{
    Agent agents[];
};
