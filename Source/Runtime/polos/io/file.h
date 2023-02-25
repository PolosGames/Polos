#pragma once

#include "polos/utils/alias.h"

namespace polos
{
    enum FileMode : uint32
    {
        kBinary = std::fstream::binary,
        kRead   = std::fstream::in,
        kWrite  = std::fstream::out,
    };

    class File
    {
    public:
        std::string file_name;
        std::string file_ext;
    public:
        explicit File(std::string full_path, FileMode mode) noexcept;
    
        std::string  ReadStr();
        DArray<byte> ReadByte();
        std::string  ReadLine();
    private:
        std::fstream m_FileStream;
        uint32       m_Mode;
        
    };
} // namespace polos
