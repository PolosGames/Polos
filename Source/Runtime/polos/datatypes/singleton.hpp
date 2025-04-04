//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef DATATYPES_SINGLETON_H
#define DATATYPES_SINGLETON_H

#include <cstdint>

namespace polos::datatypes
{

template<typename Type>
class Singleton
{
public:
    virtual ~Singleton() = default;

    template<typename... Args>
    static Type* Instance(Args&&... t_args);

    static bool Destroy(); // TODO: Return a std::expected here
private:
    struct data
    {
        union
        {
            Type         instance;
            std::uint8_t placeholder{0U};
        };
    };

    static bool s_is_constructed;
};

} // namespace polos::datatypes

#endif //DATATYPES_SINGLETON_H
