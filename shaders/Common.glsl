#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout (rgba32f, binding = 0) uniform image2D u_Image;

uniform ivec2 u_Resolution;

const float PI = 3.14159265359;
