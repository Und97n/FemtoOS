#include <ctype.h>

int isalnum(const int c) {
  return isalpha(c) || isdigit(c);
}

int isalpha(const int c) {
  return isupper(c) || islower(c); /* in the "C" locale */
}

int isascii(const int c) {
  return c >= 0 && c <= 0x7F; /* 7-bit ASCII */
}

int isblank(const int c) {
  return c == ' ' || c == '\t'; /* in the "C" locale */
}

int iscntrl(const int c) {
  return (unsigned int)c < 0x20 || c == 0x7F; /* in the "C" locale */
}

int isdigit(const int c) {
  return c >= '0' && c <= '9';
}

int isgraph(const int c) {
  return c >= 0x21 && c <= 0x7E; /* in the "C" locale */
}

int islower(const int c) {
  return c >= 'a' && c <= 'z'; /* in the "C" locale */
}

int isprint(const int c) {
  return c >= 0x20 && c <= 0x7E; /* in the "C" locale */
}

int ispunct(const int c) {
  return isprint(c) && !isspace(c) && !isalnum(c); /* in the "C" locale */
}

int isspace(const int c) {
  switch (c) { /* in the "C" locale: */
    case ' ':  /* space */
    case '\f': /* form feed */
    case '\n': /* new-line */
    case '\r': /* carriage return */
    case '\t': /* horizontal tab */
    case '\v': /* vertical tab */
      return 1;
    default:
      return 0;
  }
}

int isupper(const int c) {
  return c >= 'A' && c <= 'Z'; /* in the "C" locale */
}

int isxdigit(const int c) {
  return (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

int tolower(const int c) {
  return isupper(c) ? 'a' + (c - 'A') : c; /* in the "C" locale */
}

int toupper(const int c) {
  return islower(c) ? 'A' + (c - 'a') : c; /* in the "C" locale */
}