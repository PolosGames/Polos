#include "transform_component.h"

namespace polos::ecs
{
    template<>
    void SerializeComponent(std::vector<byte>& p_OutVector, transform_component* p_Component)
    {
        byte* bytes = reinterpret_cast<byte*>(p_Component);
        std::copy(bytes, std::next(bytes, sizeof(transform_component)), std::back_inserter(p_OutVector));
    }

    template<>
    transform_component DeserializeComponent(std::vector<byte>& p_InVector, std::size_t p_Index)
    {
        transform_component comp;
        byte*               bytes               = reinterpret_cast<byte*>(&comp);
        auto                comp_start_iterator = std::next(p_InVector.begin(), p_Index);

        std::copy(comp_start_iterator, std::next(comp_start_iterator, sizeof(transform_component)), bytes);

        return comp;
    }
}// namespace polos::ecs