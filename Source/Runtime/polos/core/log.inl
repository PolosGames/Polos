namespace polos
{
    template<typename ...Args>
    inline void Log::critical(uint8 type, spdlog::format_string_t<Args...> fmt, Args && ...args)
    {
        m_Logger[type]->critical(fmt, std::forward<Args>(args)...);
    }
    
    template<typename ...Args>
    inline void Log::error(uint8 type, spdlog::format_string_t<Args...> fmt, Args && ...args)
    {
        m_Logger[type]->error(fmt, std::forward<Args>(args)...);
    }
    
    template<typename ...Args>
    inline void Log::warn(uint8 type, spdlog::format_string_t<Args...> fmt, Args && ...args)
    {
        m_Logger[type]->warn(fmt, std::forward<Args>(args)...);
    }
    
    template<typename ...Args>
    inline void Log::info(uint8 type, spdlog::format_string_t<Args...> fmt, Args && ...args)
    {
        m_Logger[type]->info(fmt, std::forward<Args>(args)...);
    }
    
    template<typename ...Args>
    inline void Log::trace(uint8 type, spdlog::format_string_t<Args...> fmt, Args && ...args)
    {
        m_Logger[type]->trace(fmt, std::forward<Args>(args)...);
    }
}