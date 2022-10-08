#include "polos/polos_pch.h"

#include "polos/utils/guid.h"

namespace polos
{
    std::random_device GUID::s_RngDevice;
    std::mt19937_64 GUID::s_RngEngine{s_RngDevice()};
    std::uniform_int_distribution<int64> GUID::s_UniDist;

    GUID::GUID()
        : m_GUID(s_UniDist(s_RngEngine))
    {
    }

    GUID::GUID(int64 guid)
        : m_GUID(guid)
    {
    }

    GUID::operator int64() const
    {
        return m_GUID; 
    }

    bool operator==(GUID const& lhs, GUID const& rhs)
    {
        return lhs.m_GUID == rhs.m_GUID;
    }
}// namespace polos