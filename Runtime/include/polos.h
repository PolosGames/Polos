#pragma once
#ifndef POLOS_POLOS_H
#define POLOS_POLOS_H

/*
 * Do not include this file anywhere inside the engine's core.
 * This header is only meant for applications that include this
 */
#include "polos_pch.h"

#include "core/application.h"
#include "core/camera.h"
#include "core/event_bus.h"
#include "core/i_window.h"
#include "core/log.h"

#include "containers/containers.h"
#include "containers/delegate.h"

#include "debug/plassert.h"
#include "debug/profiling.h"

#include "engine/engine.h"

#include "events/events.h"

#include "gui/gui.h"

#include "io/file.h"
#include "io/filesystem.h"

#include "memory/linear_allocator.h"
#include "memory/pool_allocator.h"
#include "memory/stack_allocator.h"
#include "memory/mem_utils.h"

#include "platform/platform_detection.h"

#include "renderer/renderer.h"
#include "renderer/context/graphics_context.h"
#include "renderer/context/shader.h"
#include "renderer/context/vertex.h"
#include "renderer/context/vao.h"
#include "renderer/context/shader_lib.h"

#include "time/timer.h"
#include "time/scope_timer.h"

#include "utils/alias.h"
#include "utils/concepts.h"
#include "utils/feature.h"
#include "utils/macro_util.h"
#include "utils/meta.h"
#include "utils/stringid.h"
#include "utils/type_util.h"

#include "core/entry_point.h"

#endif /* POLOS_POLOS_H */
