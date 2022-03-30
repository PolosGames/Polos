#include "polos_pch.h"

#include "file.h"

namespace polos
{
#ifdef POLOS_WIN
    static constexpr char seperator = '\\';
#else
    static constexpr char seperator = '/';
#endif

    File::File(std::string_view full_path, FileMode mode) noexcept
        : m_Mode(mode)
    {
        m_FileStream.open(full_path.data(), std::ios::openmode(mode));
        ASSERTSTR(m_FileStream.is_open(), "File could not be opened.");
        
        std::string_view full_name = full_path.substr(full_path.find_last_of(seperator) + 1);
        std::size_t sep_loc = full_name.find_last_of('.');
        file_ext  = full_name.substr(sep_loc);
        file_name = full_name.substr(0, full_name.size() - file_ext.size());
    }
    
    std::string File::ReadLine()
    {
        if(!( m_Mode & FileMode::kRead)) return "Could not read line";
    
        std::string line;
        std::getline(m_FileStream, line);
        
        return line;
    }
    
    std::string File::Read()
    {
        std::stringstream ss;
        ss << m_FileStream.rdbuf();
        return ss.str();
    }
} // namespace polos
