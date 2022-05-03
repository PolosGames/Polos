#include "polos_pch.h"

#include "utils/stringid.h"

#include "resource_manager.h"

namespace polos
{
    ResourceManager* ResourceManager::m_Instance;

    static constexpr std::array<cstring, kMaxResType> g_ResourceTypeIds {
        "texture", // kTexture
        "model",   // kModel
        "audio",   // kAudio
    };

    ResourceHandle ResourceManager::LoadAsset(ResourceType type, cstring resource_file)
    {
        ResourceManager& mgr = *m_Instance;
        return 0;
    }
}// namespace polos