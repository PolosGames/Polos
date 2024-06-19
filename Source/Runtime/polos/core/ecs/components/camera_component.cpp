
#include "camera_component.h"

namespace polos::ecs
{
    template<>
    void SerializeComponent(std::vector<byte>& p_OutVector, camera_component* p_Component)
    {
        byte* bytes = reinterpret_cast<byte*>(p_Component);
        std::copy(bytes, std::next(bytes, sizeof(camera_component)), std::back_inserter(p_OutVector));
    }

    template<>
    camera_component DeserializeComponent(std::vector<byte>& p_InVector, ptrdiff_t p_Index)
    {
        camera_component comp;
        byte*            bytes = reinterpret_cast<byte*>(&comp);

        auto comp_start_iterator = std::next(p_InVector.begin(), p_Index);
        std::copy(comp_start_iterator, std::next(comp_start_iterator, sizeof(camera_component)), bytes);

        return comp;
    }
}// namespace polos::ecs