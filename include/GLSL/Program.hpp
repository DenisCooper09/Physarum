#pragma once

#include "Shader.hpp"

namespace GLSL
{
    class Program : public OpenGL::Object
    {
    public:
        explicit Program();

        ~Program();

    public:
        void Use() const;
        static void Unuse();

        void Attach(const Source &source) const;

        void Link() const;

        void SetUniform(const std::string &name, GLint x, GLint y);
    };
}
