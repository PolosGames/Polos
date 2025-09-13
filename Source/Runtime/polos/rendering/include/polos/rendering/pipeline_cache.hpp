//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_PIPELINE_CACHE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_PIPELINE_CACHE_HPP_

#include "polos/rendering/graphics_pipeline_info.hpp"

#include <unordered_map>

namespace polos::rendering
{

class PipelineCache
{
public:
    PipelineCache();
    ~PipelineCache();

    PipelineCache(PipelineCache const&)            = delete;
    PipelineCache(PipelineCache&&)                 = delete;
    PipelineCache& operator=(PipelineCache const&) = delete;
    PipelineCache& operator=(PipelineCache&&)      = delete;

    auto GetPipeline(graphics_pipeline_info const& t_pipeline_info) -> VkPipeline;

    /// @brief Either creates the pipeline from the given information or loads it from disk with VkPipelineCache
    /// @param t_pipeline_info The information to create the pipeline with.
    auto LoadOrCreatePipeline(graphics_pipeline_info const& t_pipeline_info) -> VkPipeline;
private:
    std::unordered_map<std::size_t, VkPipeline> m_cache;
    VkPipelineCache                             m_creation_cache;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_PIPELINE_CACHE_HPP_
