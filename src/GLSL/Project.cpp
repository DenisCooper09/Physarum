#include "GLSL/Project.hpp"

namespace GLSL
{
    Project::Project(const std::filesystem::path &directory, const std::initializer_list<SourceSpec> &sources)
    {
        m_Directory = directory;

        for (const auto &source: sources)
        {
            std::ifstream file(directory / source.filename);
            std::string   content(std::istreambuf_iterator<char>(file), {});

            m_Sources[source.filename] = {content, source.type};
        }
    }

    void Project::Preprocess()
    {
        const std::string include_preprocessor = "#include \"";

        for (auto &[filename, source]: m_Sources)
        {
            size_t start;
            while ((start = source.content.find(include_preprocessor)) != std::string::npos)
            {
                start += include_preprocessor.length();

                size_t      end          = source.content.find('"', start);
                std::string include_file = source.content.substr(start, end - start);

                start -= include_preprocessor.length();

                if (m_Sources[include_file].type != ShaderType::Header)
                {
                    std::cerr << "[ERROR]: Can't include a non-header shader.\n";
                    continue;
                }

                source.content.replace(start, (end - start) + 1, m_Sources[include_file].content);
            }
        }
    }

    void Project::Build(Program &program, const std::initializer_list<std::filesystem::path> &files)
    {
        for (const auto &file: files)
            program.Attach(m_Sources[file]);

        program.Link();
    }
}
