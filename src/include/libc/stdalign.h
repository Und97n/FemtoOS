#pragma once

#if __STDC_VERSION__ < 201112L
#define _Alignas
#define _Alignof
#endif

#define alignas _Alignas

#define alignof _Alignof

#define __alignas_is_defined 1

#define __alignof_is_defined 1