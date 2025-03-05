struct Sensor
{
    float angle;
    float extent;

    float _padding1;
    float _padding2;
};

layout (std430, binding = 2) buffer Sensors
{
    Sensor sensors[];
};
