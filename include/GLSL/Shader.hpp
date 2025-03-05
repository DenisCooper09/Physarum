#pragma once

#include <iostream>
#include <filesystem>

#include "Object.hpp"

namespace GLSL
{
    enum ShaderType : GLenum
    {
        Header,
        Vertex   = GL_VERTEX_SHADER,
        Fragment = GL_FRAGMENT_SHADER,
        Compute  = GL_COMPUTE_SHADER,
    };

    struct Source
    {
        std::string content;
        ShaderType  type{};
    };

    struct SourceSpec
    {
        std::filesystem::path filename;
        ShaderType            type{};
    };

    class Shader : public OpenGL::Object
    {
    public:
        explicit Shader(ShaderType type);

        ~Shader();

    public:
        void Compile(const std::string &source) const;
    };
}
