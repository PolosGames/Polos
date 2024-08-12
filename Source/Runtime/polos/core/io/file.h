#pragma once

#include "polos/utils/alias.h"

namespace polos
{
    enum FileMode : uint32
    {
        k_Binary = std::fstream::binary,
        k_Read   = std::fstream::in,
        k_Write  = std::fstream::out,
    };

    class File
    {
    public:
        explicit File(std::string p_FullPath, FileMode p_Mode) noexcept;

        std::string  ReadStr();
        DArray<byte> ReadByte();
        std::string  ReadLine();
        bool         ReadLineIntoString(std::string& p_Line);
    public:
        std::string fileName;
        std::string fileExtension;
    private:
        std::fstream m_FileStream;
        uint32       m_Mode;
    };
} // namespace polos
