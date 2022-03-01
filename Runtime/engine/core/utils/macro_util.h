#pragma once

#ifndef POLOS_UTILS_UTIL_H
#define POLOS_UTILS_UTIL_H

#define PL_EXPAND(X) x

/// Concatenates the passed parameters together.
#define CONCAT_IMPL_(x, y) x ## y
#define PL_CONCAT(x, y)    CONCAT_IMPL_(x, y)

/// Makes the passed parameter string
#define PL_STRINGIFY(x) #x

/// Creates a unique name based on a line
#define PL_ANON_NAME(name) PL_CONCAT(name, __LINE__)

/// Calls function x with parameter y
#define PL_CALL(x, y) x(y)

#define PL_RULE_OF_FIVE(Type)				\
	Type() = default;						\
	Type(const Type&) =	default;			\
	Type(Type&&) = default;					\
	Type& operator=(const Type&) = default;	\
	Type& operator=(Type&&) = default;		\
	~Type() = default;						\

#define PL_RULE_OF_FIVE_NO_DTOR(Type)		\
	Type() = default;						\
	Type(const Type&) =	default;			\
	Type(Type&&) = default;					\
	Type& operator=(const Type&) = default;	\
	Type& operator=(Type&&) = default;		\

#define DELETE_COPY_MOVE_CTOR(Type)			\
	Type(const Type&) =	delete;				\
	Type(Type&&) = delete;					\
	Type& operator=(const Type&) = delete;	\
	Type& operator=(Type&&) = delete;		\



#endif /* POLOS_UTILS_UTIL_H */