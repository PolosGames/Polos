#pragma once
#ifndef POLOS_CORE_UTILS_TYPEUTIL_H_
#define POLOS_CORE_UTILS_TYPEUTIL_H_

#include <typeinfo>

namespace polos
{
    template<typename T>
    char const* TypeName()
    {
        static std::string name = typeid(T).name();

        // find the namespace colon
        size_t name_pos = name.find_last_of(':');
        if (name_pos != std::string::npos)
        {
            name = name.substr(name_pos + 1);
        }

        // remove compiler generated types for pointers
        name_pos = name.find(" * __ptr64");
        if (name_pos != std::string::npos)
        {
            name = name.substr(0, name_pos);
            name += "*";
        }

        return name.c_str();
    }

    int64 Hashit(char const* const str);
    int64 Hashit(std::string const& str);
    int64 Hashit(std::string&& str);

    template<typename T>
    string_id TypeHash()
    {
        static char const* name = TypeName<T>();
        static int64 th = Hashit(name);

        return th;
    }

    inline string_id operator""_sid(char const* str, std::size_t _) { return Hashit(str); }
}


#endif /* POLOS_CORE_UTILS_TYPEUTIL_H_ */