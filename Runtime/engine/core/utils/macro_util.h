#pragma once

#ifndef POLOS_UTILS_MACRO_UTIL_H
#define POLOS_UTILS_MACRO_UTIL_H

#define _PL_EXPAND(x) x
#define PL_EXPAND(x) _PL_EXPAND(x)

/// Concatenates the passed parameters together.
#define _PL_CONCAT(x, y) x ## y
#define PL_CONCAT(x, y)  _PL_CONCAT(x, y)

/// Makes the passed parameter string
#define _PL_STRINGIFY(x) #x
#define PL_STRINGIFY(x)  _PL_STRINGIFY(x)

/// Creates a unique name based on a line
#define PL_ANON_NAME(name) PL_CONCAT(name, __LINE__)
#define PL

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

#define PL_DELETE_COPY_MOVE_CTOR(Type)			\
	Type(const Type&) =	delete;				\
	Type(Type&&) = delete;					\
	Type& operator=(const Type&) = delete;	\
	Type& operator=(Type&&) = delete;		\



#endif /* POLOS_UTILS_MACRO_UTIL_H */