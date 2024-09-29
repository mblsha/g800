#ifndef UTILS_H_
#define UTILS_H_

#include <stddef.h>
#include <stdint.h>

int memicmp(const char *s1, const char *s2, int len);
int stricmp(const char *s1, const char *s2);
int strnicmp(const char *s1, const char *s2, int len);

#endif // UTILS_H_
