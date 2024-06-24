#pragma once

namespace polos
{
    struct GUID
    {
        uint64_t guid;
    };

    bool operator==(GUID const& p_Lhs, GUID const& p_Rhs);

    GUID GenerateGUID();

}// namespace polos
