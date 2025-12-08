//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/render_pass/lambda_pass.hpp"

#include "polos/rendering/render_graph_registry.hpp"
#include "polos/rendering/render_pass_resolver.hpp"

namespace polos::rendering
{

LambdaPass::LambdaPass(SetupFunc t_setup_f, ExecuteFunc t_execute_f)
    : m_setup_func{std::move(t_setup_f)},
      m_execute_func{std::move(t_execute_f)}
{}

LambdaPass::~LambdaPass() = default;

void LambdaPass::Setup(RenderPassResolver& t_builder)
{
    if (nullptr != m_setup_func)
    {
        std::invoke(m_setup_func, t_builder);
    }
}

void LambdaPass::Execute(VkCommandBuffer t_cmd_buf, const RenderGraphRegistry& t_registry)
{
    if (nullptr != m_execute_func)
    {
        std::invoke(m_execute_func, t_cmd_buf, t_registry);
    }
}

}// namespace polos::rendering