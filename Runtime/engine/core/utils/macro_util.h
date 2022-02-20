#pragma once

#ifndef POLOS_UTILS_UTIL_H
#define POLOS_UTILS_UTIL_H

#define PL_EXPAND(X) x

/// Concatanates the passed parameters together.
#define PL_CONCAT(x, y) x ## y

/// Makes the passed parameter string
#define PL_STRINGIFY(x) #x

/// Creates a unique name based on a line
#define PL_RANDNAME(name) PL_CONCAT(name, __LINE__)

/// Calls function x with parameter y
#define PL_CALL(x, y) x(y)

#endif /* POLOS_UTILS_UTIL_H */