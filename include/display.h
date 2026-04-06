#ifndef DISPLAY_H
#define DISPLAY_H

/*
 * display.h — Output formatting and export declarations
 * Console table printing and file export prototypes
 */

#include "stats.h"

void print_summary(ColumnStats *stats, int count);
int  export_summary(ColumnStats *stats, int count, const char *output_path);

#endif /* DISPLAY_H */
