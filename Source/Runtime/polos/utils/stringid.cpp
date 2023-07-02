#include "stringid.h"

#include "stringid.h"

namespace polos
{
    namespace
    {
        std::array<StringId, 256>     g_StringIdTable;
        std::array<std::string,  256> g_StringTable;
        std::size_t                   g_Counter{};
    }

    uint32 StrHash32(cstring p_Str)
    {
        uint32 m{37};
        uint32 h{};
        uint8* p{};

        h = 0;
        for (p = (uint8*)p_Str; *p != '\0'; p++)
            h = m * h + *p;

        return h + (h >> 5);
    }

    StringId GetStringId(std::string const& p_Str)
    {
        StringId    sid  = StrHash64(p_Str.c_str());
        std::size_t i{};
        auto        it   = std::ranges::find_if(g_StringIdTable, [sid, &i](auto const& p_Sid) { i++; return sid == p_Sid; });
        if (it == g_StringIdTable.end())
        {
            g_StringIdTable[g_Counter] = sid;
            g_StringTable[g_Counter]   = p_Str;
            g_Counter++;
        }

        return sid;
    }

    std::string_view GetStringFromId(StringId p_Sid)
    {
        std::size_t i{};
        auto        it = std::ranges::find_if(g_StringIdTable, [sid = p_Sid, &i](auto const& p_Sid) { i++; return p_Sid == sid; });
        if (it != g_StringIdTable.end())
        {
            return g_StringTable.at(i);
        }

        LOG_ENGINE_WARN("No string with the id {} found! Returning empty string_view.", p_Sid);

        return std::string_view{};
    }
}