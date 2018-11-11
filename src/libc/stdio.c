#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
// #include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <kernel/drivers/tty.h>

int putchar(int ch) {
	tty_putchar(ch);
	
	return ch;
}

int puts(const char* string) {
	tty_puts(string);

	return 1;
}

static bool print(const char* data, size_t length) {
	tty_putstring(data, length);
	return true;
}

// int old_printf(const char* restrict format, ...) {
// 	va_list parameters;
// 	va_start(parameters, format);

// 	int written = 0;

// 	while (*format != '\0') {
// 		size_t maxrem = INT_MAX - written;

// 		if (format[0] != '%' || format[1] == '%') {
// 			if (format[0] == '%') {
// 				format++;
// 			}
			
// 			size_t amount = 1;
			
// 			while (format[amount] && format[amount] != '%') {
// 				amount++;
// 			}
			
// 			if (maxrem < amount) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
			
// 			if (!print(format, amount)) {
// 				return -1;
// 			}
			
// 			format += amount;
// 			written += amount;
			
// 			continue;
// 		}

// 		const char* format_begun_at = format++;

// 		if (*format == 'c') {
// 			format++;
// 			char c = (char) va_arg(parameters, int /* char promotes to int */);
// 			if (!maxrem) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
			
// 			if (!print(&c, 1)) {
// 				return -1;
// 			}
			
// 			written++;
// 		} else if (*format == 's') {
// 			format++;
// 			const char* str = va_arg(parameters, const char*);
// 			size_t len = strlen(str);
			
// 			if (maxrem < len) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
			
// 			if (!print(str, len)) {
// 				return -1;
// 			}
			
// 			written += len;
// 		} else if (*format == 'd') {
// 			format++;
// 			int data = va_arg(parameters, int);

// 			if(data < 0) {
// 				putchar('-');
				
// 				data = -data;
// 			}
			
// 			// Max length = 10
// 			char out[10];			
			
// 			uint _data = data;
			
// 			size_t charsCount = 0;
			
// 			do {
// 				out[charsCount++] = (_data % 10) + '0';
// 				_data /= 10;
				
// 			} while(_data);
			
// 			for(int i = charsCount - 1; i >= 0; --i) {
// 				putchar(out[i]);
// 			}
			
// 			if (maxrem < charsCount) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
	
// 			written += charsCount;
// 		} else if (*format == 'u') {
// 			format++;
// 			uint32_t data = va_arg(parameters, uint32_t);
			
// 			// Max length = 10
// 			char out[10];			
			
// 			int _data = data;
			
// 			size_t charsCount = 0;
			
// 			do {
// 				out[charsCount++] = (_data % 10) + '0';
// 				_data /= 10;
				
// 			} while(_data);
			
// 			for(int i = charsCount - 1; i >= 0; --i) {
// 				putchar(out[i]);
// 			}
			
// 			if (maxrem < charsCount) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
	
// 			written += charsCount;
// 		} else if (*format == 'h') {
// 			// Hex
			
// 			format++;
// 			uint32_t data = va_arg(parameters, uint32_t);			
			
// 			// Max length = 8
// 			char out[8];			
			
// 			int _data = data;
			
// 			size_t charsCount = 0;
			
// 			do {
// 				char c = (_data % 0x10) + '0';
				
// 				if(c > '9') {
// 					c -= '9' + 1;
// 					c += 'a';
// 				}
				
// 				out[charsCount++] = c;
				

// 				_data >>= 4;
				
// 			} while(_data);
			
// 			for(int i = charsCount - 1; i >= 0; --i) {
// 				putchar(out[i]);
// 			}
			
// 			if (maxrem < charsCount) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
	
// 			written += charsCount;
// 		} else {
// 			format = format_begun_at;
// 			size_t len = strlen(format);
			
// 			if (maxrem < len) {
// 				// TODO: Set errno to EOVERFLOW.
// 				return -1;
// 			}
			
// 			if (!print(format, len)) {
// 				return -1;
// 			}
			
// 			written += len;
// 			format += len;
// 		}
// 	}

// 	va_end(parameters);
// 	return written;
// }

// /* On x86, division of one 64-bit integer by another cannot be
//    done with a single instruction or a short sequence.  Thus, GCC
//    implements 64-bit division and remainder operations through
//    function calls.  These functions are normally obtained from
//    libgcc, which is automatically included by GCC in any link
//    that it does.

//    Some x86-64 machines, however, have a compiler and utilities
//    that can generate 32-bit x86 code without having any of the
//    necessary libraries, including libgcc.  Thus, we can make
//    Pintos work on these machines by simply implementing our own
//    64-bit division routines, which are the only routines from
//    libgcc that Pintos requires.

//    Completeness is another reason to include these routines.  If
//    Pintos is completely self-contained, then that makes it that
//    much less mysterious. */

// /* Uses x86 DIVL instruction to divide 64-bit N by 32-bit D to
//    yield a 32-bit quotient.  Returns the quotient.
//    Traps with a divide error (#DE) if the quotient does not fit
//    in 32 bits. */
// static inline uint32_t
// divl (uint64_t n, uint32_t d)
// {
//   uint32_t n1 = n >> 32;
//   uint32_t n0 = n;
//   uint32_t q, r;

//   asm ("divl %4"
//        : "=d" (r), "=a" (q)
//        : "0" (n1), "1" (n0), "rm" (d));

//   return q;
// }

// /* Returns the number of leading zero bits in X,
//    which must be nonzero. */
// static int
// nlz (uint32_t x) 
// {
//   /* This technique is portable, but there are better ways to do
//      it on particular systems.  With sufficiently new enough GCC,
//      you can use __builtin_clz() to take advantage of GCC's
//      knowledge of how to do it.  Or you can use the x86 BSR
//      instruction directly. */
//   int n = 0;
//   if (x <= 0x0000FFFF)
//     {
//       n += 16;
//       x <<= 16; 
//     }
//   if (x <= 0x00FFFFFF)
//     {
//       n += 8;
//       x <<= 8; 
//     }
//   if (x <= 0x0FFFFFFF)
//     {
//       n += 4;
//       x <<= 4;
//     }
//   if (x <= 0x3FFFFFFF)
//     {
//       n += 2;
//       x <<= 2; 
//     }
//   if (x <= 0x7FFFFFFF)
//     n++;
//   return n;
// }

// /* Divides unsigned 64-bit N by unsigned 64-bit D and returns the
//    quotient. */
// static uint64_t
// udiv64 (uint64_t n, uint64_t d)
// {
//   if ((d >> 32) == 0) 
//     {
//       /* Proof of correctness:

//          Let n, d, b, n1, and n0 be defined as in this function.
//          Let [x] be the "floor" of x.  Let T = b[n1/d].  Assume d
//          nonzero.  Then:
//              [n/d] = [n/d] - T + T
//                    = [n/d - T] + T                         by (1) below
//                    = [(b*n1 + n0)/d - T] + T               by definition of n
//                    = [(b*n1 + n0)/d - dT/d] + T
//                    = [(b(n1 - d[n1/d]) + n0)/d] + T
//                    = [(b[n1 % d] + n0)/d] + T,             by definition of %
//          which is the expression calculated below.

//          (1) Note that for any real x, integer i: [x] + i = [x + i].

//          To prevent divl() from trapping, [(b[n1 % d] + n0)/d] must
//          be less than b.  Assume that [n1 % d] and n0 take their
//          respective maximum values of d - 1 and b - 1:
//                  [(b(d - 1) + (b - 1))/d] < b
//              <=> [(bd - 1)/d] < b
//              <=> [b - 1/d] < b
//          which is a tautology.

//          Therefore, this code is correct and will not trap. */
//       uint64_t b = 1ULL << 32;
//       uint32_t n1 = n >> 32;
//       uint32_t n0 = n; 
//       uint32_t d0 = d;

//       return divl (b * (n1 % d0) + n0, d0) + b * (n1 / d0); 
//     }
//   else 
//     {
//       /* Based on the algorithm and proof available from
//          http://www.hackersdelight.org/revisions.pdf. */
//       if (n < d)
//         return 0;
//       else 
//         {
//           uint32_t d1 = d >> 32;
//           int s = nlz (d1);
//           uint64_t q = divl (n >> 1, (d << s) >> 32) >> (31 - s);
//           return n - (q - 1) * d < d ? q - 1 : q; 
//         }
//     }
// }

// /* Divides unsigned 64-bit N by unsigned 64-bit D and returns the
//    remainder. */
// static uint32_t
// umod64 (uint64_t n, uint64_t d)
// {
//   return n - d * udiv64 (n, d);
// }

// /* Divides signed 64-bit N by signed 64-bit D and returns the
//    quotient. */
// static int64_t
// sdiv64 (int64_t n, int64_t d)
// {
//   uint64_t n_abs = n >= 0 ? (uint64_t) n : -(uint64_t) n;
//   uint64_t d_abs = d >= 0 ? (uint64_t) d : -(uint64_t) d;
//   uint64_t q_abs = udiv64 (n_abs, d_abs);
//   return (n < 0) == (d < 0) ? (int64_t) q_abs : -(int64_t) q_abs;
// }

// /* Divides signed 64-bit N by signed 64-bit D and returns the
//    remainder. */
// static int32_t
// smod64 (int64_t n, int64_t d)
// {
//   return n - d * sdiv64 (n, d);
// }
// 
// /* These are the routines that GCC calls. */

// long long __divdi3 (long long n, long long d);
// long long __moddi3 (long long n, long long d);
// unsigned long long __udivdi3 (unsigned long long n, unsigned long long d);
// unsigned long long __umoddi3 (unsigned long long n, unsigned long long d);

// /* Signed 64-bit division. */
// long long
// __divdi3 (long long n, long long d) 
// {
//   return sdiv64 (n, d);
// }

// /* Signed 64-bit remainder. */
// long long
// __moddi3 (long long n, long long d) 
// {
//   return smod64 (n, d);
// }

// /* Unsigned 64-bit division. */
// unsigned long long
// __udivdi3 (unsigned long long n, unsigned long long d) 
// {
//   return udiv64 (n, d);
// }

// /* Unsigned 64-bit remainder. */
// unsigned long long
// __umoddi3 (unsigned long long n, unsigned long long d) 
// {
//   return umod64 (n, d);
// }

// typedef unsigned long long UDWtype;
// typedef unsigned long UWtype;

// UDWtype
// __udivmoddi4 (UDWtype n, UDWtype d, UDWtype *rp)
// {
//   UDWtype q = 0, r = n, y = d;
//   UWtype lz1, lz2, i, k;

//   /* Implements align divisor shift dividend method. This algorithm
//      aligns the divisor under the dividend and then perform number of
//      test-subtract iterations which shift the dividend left. Number of
//      iterations is k + 1 where k is the number of bit positions the
//      divisor must be shifted left  to align it under the dividend.
//      quotient bits can be saved in the rightmost positions of the dividend
//      as it shifts left on each test-subtract iteration. */

//   if (y <= r)
//     {
//       lz1 = __builtin_clzll (d);
//       lz2 = __builtin_clzll (n);

//       k = lz1 - lz2;
//       y = (y << k);

//       /* Dividend can exceed 2 ^ (width − 1) − 1 but still be less than the
// 	 aligned divisor. Normal iteration can drops the high order bit
// 	 of the dividend. Therefore, first test-subtract iteration is a
// 	 special case, saving its quotient bit in a separate location and
// 	 not shifting the dividend. */
//       if (r >= y)
// 	{
// 	  r = r - y;
// 	  q =  (1ULL << k);
// 	}

//       if (k > 0)
// 	{
// 	  y = y >> 1;

// 	  /* k additional iterations where k regular test subtract shift
// 	    dividend iterations are done.  */
// 	  i = k;
// 	  do
// 	    {
// 	      if (r >= y)
// 		r = ((r - y) << 1) + 1;
// 	      else
// 		r =  (r << 1);
// 	      i = i - 1;
// 	    } while (i != 0);

// 	  /* First quotient bit is combined with the quotient bits resulting
// 	     from the k regular iterations.  */
// 	  q = q + r;
// 	  r = r >> k;
// 	  q = q - (r << k);
// 	}
//     }

//   if (rp)
//     *rp = r;
//   return q;
// }