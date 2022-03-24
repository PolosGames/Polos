#pragma once

#ifndef POLOS_CONTEXT_VBO_H
#define POLOS_CONTEXT_VBO_H

#include "core/utils/alias.h"

namespace polos
{
    class vbo
    {
    public:
        void bind();
        void unbind();
    private:
        uint32 id;
    };
}

#endif /* POLOS_CONTEXT_VBO_H */
