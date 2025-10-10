//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_LAMBDA_PASS_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_LAMBDA_PASS_HPP_

#include "polos/rendering/i_render_pass.hpp"
#include "polos/rendering/render_pass_builder.hpp"
#include "polos/rendering/render_pass_registry.hpp"

#include <functional>

namespace polos::rendering
{

class LambdaPass : public IRenderPass
{
public:
    using SetupFunc   = std::function<void(RenderPassBuilder&)>;
    using ExecuteFunc = std::function<void(VkCommandBuffer, const RenderPassRegistry&)>;

    explicit LambdaPass(SetupFunc t_setup_f, ExecuteFunc t_execute_f);
    ~LambdaPass() override = default;

    void Setup(RenderPassBuilder& t_builder) override;
    void Execute(VkCommandBuffer t_cmd_buf, const RenderPassRegistry& t_registry) override;
private:
    SetupFunc   m_setup_func;
    ExecuteFunc m_execute_func;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDER_PASS_LAMBDA_PASS_HPP_
