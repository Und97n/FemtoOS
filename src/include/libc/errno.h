#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define EDOM   1
#define EILSEQ 2
#define ERANGE 3

extern int errno;

#ifdef __cplusplus
} /* extern "C" */
#endif