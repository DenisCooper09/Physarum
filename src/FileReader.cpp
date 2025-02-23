#include "FileReader.h"

void ReadFile(const char *path, char **contents)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file)
    {
        std::cerr << "Failed to open file.\n";
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    *contents = new char[size + 1];

    if (!file.read(*contents, size))
    {
        std::cerr << "Failed to read file.\n";
        delete[] *contents;
    }

    *(*contents + size) = '\0';
}
