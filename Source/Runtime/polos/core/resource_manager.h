#pragma once

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
