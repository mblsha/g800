#include "utils.h"

#include <string.h>

int stricmp(const char *s1, const char *s2) {
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

int memicmp(const char *s1, const char *s2, int len) {
  return strncasecmp(s1, s2, len);
}

int strnicmp(const char *s1, const char *s2, int len) {
  return strncasecmp(s1, s2, len);
}
