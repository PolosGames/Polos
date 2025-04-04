//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef THIRDPARTY_QUILL_QUILL_SHARED_H
#define THIRDPARTY_QUILL_QUILL_SHARED_H

#include <quill/core/Attributes.h>
#include <quill/Logger.h>

#include <string>

namespace polos::logging
{
QUILL_EXPORT void setup_quill();
QUILL_EXPORT quill::Logger* get_logger(std::string const& name);
} // namespace polos::logging

#endif //THIRDPARTY_QUILL_QUILL_SHARED_H
