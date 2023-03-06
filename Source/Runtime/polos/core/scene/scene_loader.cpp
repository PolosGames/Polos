
#include "scene_loader.h"

namespace polos
{
    std::vector<std::shared_ptr<Scene>> LoadProject(std::string p_Path)
    {
        File project_file(p_Path, FileMode::k_Read);

        std::vector<std::shared_ptr<Scene>> loaded_scenes;

        std::string line_content;
        while (project_file.ReadLineIntoString(line_content))
        {
            File scene(line_content, FileMode::k_Read);

            std::string scene_content(std::move(scene.ReadStr()));
            LOG_ENGINE_INFO("Scene contents:\n{}", scene_content.c_str());
        }

        return loaded_scenes;
    }
}