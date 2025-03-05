#pragma once

#include <glad/glad.h>

namespace OpenGL
{
    class Object
    {
    public:
        [[nodiscard]] GLuint GetHandle() const;

    protected:
        GLuint m_Handle;
    };
}
