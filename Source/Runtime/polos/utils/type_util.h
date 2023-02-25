#pragma once

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
