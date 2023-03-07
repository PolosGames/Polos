#pragma once

#include "polos/core/events/event.h"
#include "polos/utils/alias.h"

namespace polos
{
    struct engine_stop final : public Event<engine_stop>
    {
    };
}
