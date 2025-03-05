#pragma once

#include "Program.hpp"
#include <map>
#include <fstream>

namespace GLSL
{
    class Project
    {
    public:
        Project(const std::filesystem::path &directory, const std::initializer_list<SourceSpec> &sources);

    public:
        void Preprocess();

        void Build(Program &program, const std::initializer_list<std::filesystem::path> &files);

    private:
        std::filesystem::path                   m_Directory;
        std::map<std::filesystem::path, Source> m_Sources;
    };
}
