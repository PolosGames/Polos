//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef PLATFORM_INCLUDE_POLOS_PLATFORM_PLATFORM_HPP_
#define PLATFORM_INCLUDE_POLOS_PLATFORM_PLATFORM_HPP_

/// Concatenates the passed parameters together.
#define PL_CONCAT_IMPL(x, y) x ## y
#define PL_CONCAT(x, y)  PL_CONCAT_IMPL(x, y)

#define PL_ANON_NAME(name) PL_CONCAT(name, __LINE__)

#endif //PLATFORM_INCLUDE_POLOS_PLATFORM_PLATFORM_HPP_
