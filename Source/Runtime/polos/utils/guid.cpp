#include "guid.h"

namespace polos
{
    static std::random_device s_rd;
    static std::mt19937_64 s_generator(s_rd());
    static std::uniform_int_distribution<uint64_t> s_uniformDistrib;

    bool operator==(GUID const& p_Lhs, GUID const& p_Rhs)
    {
        return p_Lhs.guid == p_Rhs.guid;
    }

    GUID GenerateGUID()
    {
        return GUID{s_uniformDistrib(s_generator)};
    }
}// namespace polos