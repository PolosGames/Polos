#pragma once
#ifndef POLOS_CORE_UTILS_TYPEUTIL_H_
#define POLOS_CORE_UTILS_TYPEUTIL_H_

#include <typeinfo>

#include "stringid.h"
#include "polos/containers/containers.h"

namespace polos
{
    template<typename T>
    StringId TypeHash()
    {
        std::string const name = typeid(T).name();
        StringId const    id   = get_string_id(name);

        return id;
    }
}


#endif /* POLOS_CORE_UTILS_TYPEUTIL_H_ */