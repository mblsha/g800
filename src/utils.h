#ifndef UTILS_H_
#define UTILS_H_

#include <stddef.h>
#include <stdint.h>

template <typename T>
int stricmp(T *s1, T *s2) {
  size_t len1 = strlen(s1);
  size_t len2 = strlen(s2);

  size_t min_len = (len1 < len2) ? len1 : len2;

  int result = strncasecmp(s1, s2, min_len);

  if (result == 0) {
    if (len1 < len2) {
      return -1;
    } else if (len1 > len2) {
      return 1;
    }
  }

  return result;
}

template <typename T>
int memicmp(T *s1, T *s2, int len) {
  return strncasecmp(s1, s2, len);
}

template <typename T>
int strnicmp(T *s1, T *s2, int len) {
  return strncasecmp(s1, s2, len);
}

#endif // UTILS_H_
