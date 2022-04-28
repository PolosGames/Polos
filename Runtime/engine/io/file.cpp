#include "polos_pch.h"

#include "file.h"

namespace polos
{
#ifdef POLOS_WIN
    static constexpr char seperator = '\\';
#else
    static constexpr char seperator = '/';
#endif

    File::File(std::string&& full_path, FileMode mode) noexcept
        : m_Mode(mode)
    {
        std::replace(full_path.begin(), full_path.end(), '/', seperator);
        m_FileStream.open(full_path.c_str(), std::ios::openmode(mode));
        
        if(!m_FileStream.is_open())
        {
            LOG_ENGINE_ERROR("File {} could not be opened", full_path);
            m_FileStream.close();
            return;
        }
        
        std::size_t full_name_loc = full_path.find_last_of(seperator) + 1;
        std::string full_name = full_path.substr(full_name_loc, full_path.size() - full_name_loc);
        std::size_t sep_loc = full_name.find_last_of('.') + 1;
        file_ext  = full_name.substr(sep_loc, full_name.size() - sep_loc);
        file_name = full_name.substr(0, full_name.size() - file_ext.size() - 1);
    }
    
    std::string File::ReadLine()
    {
        if(!( m_Mode & FileMode::kRead)) return "Could not read line";
    
        std::string line;
        std::getline(m_FileStream, line);
        
        return line;
    }
    
    std::string File::ReadStr()
    {
        std::stringstream ss;
        ss << m_FileStream.rdbuf();
        return ss.str();
    }
    
    DArray<byte> File::ReadByte()
    {
        if (!(m_Mode & (FileMode::kRead | FileMode::kBinary))) return DArray<byte>{};

        DArray<byte> byte_arr {
            std::istreambuf_iterator<char>(m_FileStream), {}
        };

        return byte_arr;
    }
} // namespace polos
