#pragma once

/**
 * The signed integer type of the result of subtracting two pointers.
 */
typedef signed long ptrdiff_t;

/**
 * The unsigned integer type of the result of the `sizeof` operator.
 */
typedef unsigned long size_t;

/**
 * An object type whose alignment is as great as is supported by the
 * implementation in all contexts.
 */
typedef struct { long long __ll; long double __ld; } max_align_t;

/**
 * An integer type whose range of values can represent distinct codes for
 * all members of the largest extended character set specified among the
 * supported locales.
 */
typedef int wchar_t;

/**
 * Null pointer constant.
 */
#define NULL ((void*)0)

#define true 1
#define false 0

/**
 * The offset in bytes to the structure member from the beginning of its
 * structure.
 */
#define offsetof(type, member) __builtin_offsetof(type, member)

/**
 * For no warnings on unused variable.
 */
#define UNUSED(x) (void)(x)

typedef unsigned char uint8_t;
typedef signed char int8_t;

typedef unsigned short uint16_t;
typedef signed short int16_t;

typedef unsigned long int uint32_t;
typedef signed long int int32_t;

typedef unsigned long long uint64_t;
typedef signed long long int64_t;

typedef unsigned int uint;

typedef uint8_t bool;

typedef uint32_t intptr_t;