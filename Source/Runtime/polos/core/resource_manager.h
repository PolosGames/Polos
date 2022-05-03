#pragma once

#ifndef POLOS_CORE_RESOURCEMANAGER_H_
#define POLOS_CORE_RESOURCEMANAGER_H_

namespace polos
{
    enum ResourceType
    {
        kTexture,
        kModel,
        kAudio,

        kMaxResType,
    };

    class ResourceManager
    {
    public:
        static ResourceHandle LoadAsset(ResourceType type, cstring resource_file);
    private:
        friend class Engine;
        static ResourceManager* m_Instance;
    };
}// namespace polos

#endif /* POLOS_CORE_RESOURCEMANAGER_H_ */