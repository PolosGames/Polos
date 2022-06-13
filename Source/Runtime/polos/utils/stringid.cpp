#include "polos/polos_pch.h"

#include "stringid.h"

namespace polos
{
    HashMap<StringId, std::string> g_StringTable{};

    StringId get_string_id(std::string const& str)
    {
        StringId sid = hash_function(str.c_str());
        if (!g_StringTable.contains(sid))
        {
            g_StringTable.insert(std::move(std::make_pair(sid, str)));
        }

        return sid;
    }

    std::string_view get_string_from_id(StringId sid)
    {
        if (g_StringTable.contains(sid))
        {
            return g_StringTable.at(sid);
        }
        return "";
    }
}