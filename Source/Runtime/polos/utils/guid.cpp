#include "guid.h"

namespace polos
{
    bool operator==(GUID const& p_Lhs, GUID const& p_Rhs)
    {
        return p_Lhs.guid == p_Rhs.guid;
    }

    GUID GenerateGUID()
    {
        return GUID{uuids::uuid_system_generator{}()};
    }
}// namespace polos