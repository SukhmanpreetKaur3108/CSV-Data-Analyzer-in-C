/*
 * utils.c — Utility functions
 * Memory allocation wrappers, string trimming, numeric validation
 *
 * Course: C for Everyone (Part 1 & 2) — UC Santa Cruz, Coursera
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

/*
 * safe_malloc — malloc that exits on failure instead of returning NULL.
 * In a real production system you'd want proper error propagation,
 * but for this project-level tool, crashing loudly is fine.
 */
void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Error: Memory allocation failed (%zu bytes requested)\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

/*
 * safe_realloc — realloc wrapper. Frees original pointer on failure
 * to avoid the classic "lost pointer on failed realloc" memory leak.
 */
void *safe_realloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        fprintf(stderr, "Error: Memory reallocation failed (%zu bytes requested)\n", size);
        free(ptr);  /* avoid leaking the old block */
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

/*
 * trim_whitespace — removes leading and trailing whitespace in-place.
 * Returns pointer to the first non-space character (may point mid-string).
 * The trailing null is moved earlier; no new allocation needed.
 */
char *trim_whitespace(char *str) {
    if (str == NULL) return NULL;

    /* Skip leading whitespace */
    while (isspace((unsigned char)*str))
        str++;

    if (*str == '\0') return str;  /* all spaces — empty string */

    /* Remove trailing whitespace */
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    *(end + 1) = '\0';

    return str;
}

/*
 * is_numeric — returns 1 if the string is a valid number, 0 otherwise.
 * Accepts: optional leading +/-, integer digits, one optional decimal point.
 * Rejects: empty strings, strings with alphabetic characters, double dots.
 */
int is_numeric(const char *str) {
    if (str == NULL || *str == '\0') return 0;

    const char *p = str;
    int has_digit = 0;
    int has_dot   = 0;

    /* Allow an optional leading sign */
    if (*p == '-' || *p == '+') p++;

    while (*p != '\0') {
        if (isdigit((unsigned char)*p)) {
            has_digit = 1;
        } else if (*p == '.' && !has_dot) {
            has_dot = 1;
        } else {
            return 0;  /* unexpected character — not numeric */
        }
        p++;
    }

    return has_digit;  /* must have at least one digit */
}
