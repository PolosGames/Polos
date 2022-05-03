#pragma once
#ifndef POLOS_CORE_UTILS_TYPEUTIL_H_
#define POLOS_CORE_UTILS_TYPEUTIL_H_

#include <typeinfo>

#include "stringid.h"
#include "containers/containers.h"

namespace polos
{
    template<typename T>
    StringId TypeHash()
    {
        std::string name = typeid(T).name();
        static int64 th = get_string_id(name.c_str());

        return th;
    }
}


#endif /* POLOS_CORE_UTILS_TYPEUTIL_H_ */