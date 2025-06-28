//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_UTILS_INCLUDE_POLOS_UTILS_MACRO_UTILITIES_HPP_
#define POLOS_UTILS_INCLUDE_POLOS_UTILS_MACRO_UTILITIES_HPP_

/// Concatenates the passed parameters together.
#define PL_CONCAT_IMPL(x, y) x##y
#define PL_CONCAT(x, y)      PL_CONCAT_IMPL(x, y)

#define PL_ANON_NAME(name) PL_CONCAT(name, __LINE__)

#endif//POLOS_UTILS_INCLUDE_POLOS_UTILS_MACRO_UTILITIES_HPP_
