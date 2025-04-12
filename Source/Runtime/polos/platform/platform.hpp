//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

/// Concatenates the passed parameters together.
#define PL_CONCAT_IMPL(x, y) x ## y
#define PL_CONCAT(x, y)  PL_CONCAT_IMPL(x, y)

#define PL_ANON_NAME(name) PL_CONCAT(name, __LINE__)

#endif //PLATFORM_PLATFORM_H
