#pragma once

#include "polos/core/scene/scene.h"
#include "polos/io/file.h"

namespace polos
{
    std::vector<std::shared_ptr<Scene>> LoadProject(std::string p_Path);
} // namespace polos
