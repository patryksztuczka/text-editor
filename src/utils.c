#include <stdlib.h>
#include <string.h>

char *substr(const char *s, size_t start, size_t len) {
  char *p = malloc(len + 1);
  if (!p)
    return NULL;
  memcpy(p, s + start, len);
  p[len] = '\0';
  return p;
}
