#pragma once

namespace polos
{
    struct GUID
    {
        uuids::uuid guid;
    };

    bool operator==(GUID const& p_Lhs, GUID const& p_Rhs);

    GUID GenerateGUID();

}// namespace polos
