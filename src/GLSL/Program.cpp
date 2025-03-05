#include "GLSL/Program.hpp"

namespace GLSL
{
    Program::Program() : Object()
    {
        m_Handle = glCreateProgram();
    }

    Program::~Program()
    {
        glDeleteProgram(m_Handle);
    }

    void Program::Use() const
    {
        glUseProgram(m_Handle);
    }

    void Program::Unuse()
    {
        glUseProgram(0);
    }

    void Program::Attach(const Source &source) const
    {
        Shader shader(source.type);
        shader.Compile(source.content);
        glAttachShader(m_Handle, shader.GetHandle());
    }

    void Program::Link() const
    {
        glLinkProgram(m_Handle);

        GLint program_linked;
        glGetProgramiv(m_Handle, GL_LINK_STATUS, &program_linked);

        if (!program_linked)
        {
            GLchar message[1024];
            glGetProgramInfoLog(m_Handle, 1024, nullptr, message);
            std::cerr << "Error linking program: " << message << '\n';
        }
    }

    void Program::SetUniform(const std::string &name, GLfloat x)
    {
        glUniform1f(glGetUniformLocation(m_Handle, name.c_str()), x);
    }

    void Program::SetUniform(const std::string &name, GLint x, GLint y)
    {
        glUniform2i(glGetUniformLocation(m_Handle, name.c_str()), x, y);
    }
}
