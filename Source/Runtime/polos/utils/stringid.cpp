
#include "stringid.h"

namespace polos
{
    HashMap<StringId, std::string> g_StringTable;

    StringId get_string_id(std::string const& p_Str)
    {
        StringId sid = hash_function(p_Str.c_str());
        if (!g_StringTable.contains(sid))
        {
            g_StringTable.insert(std::move(std::make_pair(sid, p_Str)));
        }

        return sid;
    }

    std::string_view get_string_from_id(StringId p_Sid)
    {
        if (g_StringTable.contains(p_Sid))
        {
            return g_StringTable.at(p_Sid);
        }
        return "";
    }
}