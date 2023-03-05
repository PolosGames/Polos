#include "polos/polos_pch.h"

#include "file.h"

namespace polos
{
#ifdef POLOS_WIN
    static constexpr char seperator = '\\';
#else
    static constexpr char seperator = '/';
#endif

    File::File(std::string p_FullPath, FileMode p_Mode) noexcept
        : m_Mode{p_Mode}
    {
        std::replace(p_FullPath.begin(), p_FullPath.end(), '/', seperator);
        m_FileStream.open(p_FullPath.c_str(), std::ios::openmode(p_Mode));

        if (!m_FileStream.is_open())
        {
            LOG_ENGINE_ERROR("File {} could not be opened", p_FullPath);
            m_FileStream.close();
            return;
        }

        std::size_t full_name_loc = p_FullPath.find_last_of(seperator) + 1;
        std::string full_name     = p_FullPath.substr(full_name_loc, p_FullPath.size() - full_name_loc);
        std::size_t sep_loc = full_name.find_last_of('.') + 1;
        fileExtension  = full_name.substr(sep_loc, full_name.size() - sep_loc);
        fileName = full_name.substr(0, full_name.size() - fileExtension.size() - 1);
    }
    
    std::string File::ReadLine()
    {
        if(!( m_Mode & FileMode::k_Read))
        {
            LOG_ENGINE_ERROR("Could not read line because the file is not in read mode.");
            return ""; // TODO: Make the return type std::optional
        }

        if (m_FileStream.eof())
        {
            LOG_ENGINE_WARN("Could not read line because the file hit EOF.");
            return "";
        }
    
        std::string line;
        std::getline(m_FileStream, line);
        
        return line;
    }

    bool File::ReadLineIntoString(std::string& p_Line)
    {
        if (!(m_Mode & FileMode::k_Read))
        {
            LOG_ENGINE_ERROR("Could not read line because the file is not in read mode.");
            return false;
        }

        if (m_FileStream.eof())
        {
            LOG_ENGINE_WARN("Could not read line because the file hit EOF.");
            return false;
        }

        std::getline(m_FileStream, p_Line);
        return true;
    }
    
    std::string File::ReadStr()
    {
        std::stringstream ss;
        ss << m_FileStream.rdbuf();
        return ss.str();
    }
    
    DArray<byte> File::ReadByte()
    {
        ASSERT(m_Mode & (FileMode::k_Read | FileMode::k_Binary));
        // https://stackoverflow.com/questions/15138353/how-to-read-a-binary-file-into-a-vector-of-unsigned-chars

        return DArray<byte>(
            std::istreambuf_iterator<char>(m_FileStream),
            std::istreambuf_iterator<char>()
        );
    }
} // namespace polos
