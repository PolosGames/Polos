///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_PASS_LAMBDA_PASS_HPP
#define POLOS_RENDERING_RENDER_PASS_LAMBDA_PASS_HPP

#include "polos/rendering/i_render_pass.hpp"

#include <functional>
#include <string>

namespace polos::rendering
{

class LambdaPass : public IRenderPass
{
public:
    using SetupFunc   = std::function<void(RenderPassResolver&)>;
    using ExecuteFunc = std::function<void(VkCommandBuffer, RenderGraphRegistry const&)>;

    explicit LambdaPass(SetupFunc t_setup_f, ExecuteFunc t_execute_f, std::string_view t_name);
    ~LambdaPass() override;

    auto               Setup(RenderPassResolver& t_builder) -> void override;
    auto               Execute(VkCommandBuffer t_cmd_buf, const RenderGraphRegistry& t_registry) -> void override;
    [[nodiscard]] auto Name() const -> char const* override;
private:
    SetupFunc   m_setup_func;
    ExecuteFunc m_execute_func;
    std::string m_name;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_PASS_LAMBDA_PASS_HPP
