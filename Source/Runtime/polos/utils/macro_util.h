#pragma once

#define PL_EXPAND_IMPL(x) x
#define PL_EXPAND(x) PL_EXPAND_IMPL(x)

/// Concatenates the passed parameters together.
#define PL_CONCAT_IMPL(x, y) x ## y
#define PL_CONCAT(x, y)  PL_CONCAT_IMPL(x, y)

/// Makes the passed parameter string
#define PL_STRINGIFY_IMPL(x) #x
#define PL_STRINGIFY(x)  PL_STRINGIFY_IMPL(x)

/// Creates a unique name based on a line
#define PL_ANON_NAME(name) PL_CONCAT(name, __LINE__)

/// Calls function x with parameter y
#define PL_CALL(x, y) x(y)

#define PL_RULE_OF_SIX(Type)                         \
    Type()                       noexcept = default; \
    ~Type()                      noexcept = default; \
    Type(const Type&)            noexcept = default; \
    Type(Type&&)                 noexcept = default; \
    Type& operator=(const Type&) noexcept = default; \
    Type& operator=(Type&&)      noexcept = default;

#define PL_RULE_OF_FIVE(Type)                        \
    Type()                       noexcept = default; \
    Type(const Type&)            noexcept = default; \
    Type(Type&&)                 noexcept = default; \
    Type& operator=(const Type&) noexcept = default; \
    Type& operator=(Type&&)      noexcept = default;


#define PL_VOID_CAST(Variable) static_cast<void>(Variable)
