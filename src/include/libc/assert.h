#pragma once

void __assertPrintUtil(char* string, int lineNumber);

#ifdef NDEBUG
#define assert(exp)
#else
#define assert(exp) {if(exp) __assertPrintUtil("Assertion failed: " #exp ", file " __FILE__ ", line %d", __LINE__);} 
#endif