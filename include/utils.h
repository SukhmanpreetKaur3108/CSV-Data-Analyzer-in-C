#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

/*
 * utils.h — Utility function declarations
 * Memory helpers, string utilities, numeric checks
 */

void *safe_malloc(size_t size);
void *safe_realloc(void *ptr, size_t size);
char *trim_whitespace(char *str);
int   is_numeric(const char *str);

#endif /* UTILS_H */
