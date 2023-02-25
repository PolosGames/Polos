#pragma once

#include <random>

namespace polos
{

    class GUID
    {
    public:
        GUID();
        GUID(int64 p_Guid);
        GUID(GUID const&)            = default;
        GUID(GUID&&)                 = default;
        GUID& operator=(GUID const&) = default;
        GUID& operator=(GUID&&)      = default;

        operator int64() const;
        friend bool operator==(GUID const& p_Lhs, GUID const& p_Rhs);

    private:
        int64 m_GUID;

        static std::random_device s_RngDevice;
        static std::mt19937_64    s_RngEngine;
        static std::uniform_int_distribution<int64> s_UniDist;
    };

}// namespace polos

namespace std
{
    template<typename T>
    struct hash;

    template<>
    struct hash<polos::GUID> {
        std::size_t operator()(polos::GUID const& p_Guid) const
        {
            return static_cast<std::int64_t>(p_Guid);
        }
    };

}// namespace std
