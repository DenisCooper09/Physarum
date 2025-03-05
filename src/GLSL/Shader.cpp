#include "GLSL/Shader.hpp"

namespace GLSL
{
    Shader::Shader(ShaderType type) : Object()
    {
        m_Handle = glCreateShader(type);
    }

    Shader::~Shader()
    {
        glDeleteShader(m_Handle);
    }

    void Shader::Compile(const std::string &source) const
    {
        const char *src = source.c_str();
        glShaderSource(m_Handle, 1, &src, nullptr);

        glCompileShader(m_Handle);

        GLint compiled;
        glGetShaderiv(m_Handle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLchar message[1024];
            glGetShaderInfoLog(m_Handle, 1024, nullptr, message);
            std::cerr << "[ERROR] Compilation failed: " << message << '\n';
        }
    }
}
