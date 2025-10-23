//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_LAMBDA_PASS_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_LAMBDA_PASS_HPP_

#include "polos/rendering/i_render_pass.hpp"

#include <functional>

namespace polos::rendering
{

class LambdaPass : public IRenderPass
{
public:
    using SetupFunc   = std::function<void(RenderPassResolver&)>;
    using ExecuteFunc = std::function<void(VkCommandBuffer, const RenderGraphRegistry&)>;

    explicit LambdaPass(SetupFunc t_setup_f, ExecuteFunc t_execute_f);
    ~LambdaPass() override = default;

    void Setup(RenderPassResolver& t_builder) override;
    void Execute(VkCommandBuffer t_cmd_buf, const RenderGraphRegistry& t_registry) override;
private:
    SetupFunc   m_setup_func;
    ExecuteFunc m_execute_func;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_LAMBDA_PASS_HPP_
