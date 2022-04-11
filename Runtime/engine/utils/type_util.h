#pragma once
#ifndef POLOS_CORE_UTILS_TYPEUTIL_H_
#define POLOS_CORE_UTILS_TYPEUTIL_H_

#include <typeinfo>

#include "stringid.h"
#include "containers/containers.h"

namespace polos
{
    template<typename T>
    const std::string& TypeName()
    {
        static std::string name = typeid(T).name();
        
//        if(g_TypeNames.contains())
//
//        // find the namespace colon
//        size_t name_pos = name.find_last_of(':');
//        if (name_pos != std::string::npos)
//        {
//            name = name.substr(name_pos + 1);
//        }
//
//        // remove compiler generated types for pointers
//        name_pos = name.find(" * __ptr64");
//        if (name_pos != std::string::npos)
//        {
//            name = name.substr(0, name_pos);
//            name += "*";
//        }
//
//        return name.c_str();
        return name;
    }

    template<typename T>
    StringId TypeHash()
    {
        static std::string name = TypeName<T>();
        static int64 th = get_string_id(name);

        return th;
    }
}


#endif /* POLOS_CORE_UTILS_TYPEUTIL_H_ */