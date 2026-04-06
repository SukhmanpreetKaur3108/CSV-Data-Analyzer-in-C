#ifndef STATS_H
#define STATS_H

/*
 * stats.h — Statistical computation declarations
 * Defines ColumnStats struct and computation function prototypes
 */

#include "csv_parser.h"

/* Holds computed statistics for a single numeric column */
typedef struct {
    char  *col_name;    /* Column name (heap-allocated copy)          */
    double mean;        /* Arithmetic mean                            */
    double median;      /* Median value (sorted middle)               */
    double min;         /* Minimum value in column                    */
    double max;         /* Maximum value in column                    */
    int    valid_count; /* Number of numeric (non-NaN) values         */
    char  *min_by;      /* Label (e.g. student name) with min value   */
    char  *max_by;      /* Label (e.g. student name) with max value   */
} ColumnStats;

ColumnStats *compute_stats(CSVData *csv, int *out_count);
void         free_stats(ColumnStats *stats, int count);

#endif /* STATS_H */
