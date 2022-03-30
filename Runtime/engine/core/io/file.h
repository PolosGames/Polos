#ifndef POLOS_CORE_IO_FILE_H_
#define POLOS_CORE_IO_FILE_H_

#include "utils/alias.h"

namespace polos
{
    enum FileMode : uint32
    {
        kBinary = std::ios::binary,
        kRead   = std::ios::in,
        kWrite  = std::ios::out,
    };

    class File
    {
    public:
        std::string file_name;
        std::string file_ext;
    public:
        explicit File(std::string_view full_path, FileMode mode) noexcept;
    
        std::string Read();
        std::string ReadLine();
    private:
        std::fstream m_FileStream;
        uint32       m_Mode;
        
    };
} // namespace polos

#endif //POLOS_CORE_IO_FILE_H_
