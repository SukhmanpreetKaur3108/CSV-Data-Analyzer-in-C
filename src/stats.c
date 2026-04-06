/*
 * stats.c — Statistical computation functions
 * Computes mean, median, min, and max for each numeric column.
 * Uses qsort() from stdlib.h for median calculation.
 *
 * Course: C for Everyone (Part 1 & 2) — UC Santa Cruz, Coursera
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stats.h"
#include "utils.h"

/* ------------------------------------------------------------------ */
/* Comparator for qsort — sorts doubles in ascending order            */
/* ------------------------------------------------------------------ */
static int compare_doubles(const void *a, const void *b) {
    double x = *(const double *)a;
    double y = *(const double *)b;
    if (x < y) return -1;
    if (x > y) return  1;
    return 0;
}

/* ------------------------------------------------------------------ */
/* Helper: collect all valid (non-NaN) values from one column         */
/* Returns a heap-allocated array; caller must free() it.             */
/* *out_count is set to the number of valid values found.             */
/* ------------------------------------------------------------------ */
static double *get_column_values(CSVData *csv, int col, int *out_count) {
    double *vals = (double *)safe_malloc(csv->row_count * sizeof(double));
    int count = 0;

    for (int i = 0; i < csv->row_count; i++) {
        if (!isnan(csv->data[i][col])) {
            vals[count++] = csv->data[i][col];
        }
    }

    *out_count = count;
    return vals;
}

/* ------------------------------------------------------------------ */
/* Core statistical functions (operate on a plain double array)       */
/* ------------------------------------------------------------------ */

static double calc_mean(const double *vals, int count) {
    if (count == 0) return (double)NAN;

    double sum = 0.0;
    for (int i = 0; i < count; i++)
        sum += vals[i];

    return sum / count;
}

static double calc_median(const double *vals, int count) {
    if (count == 0) return (double)NAN;

    /*
     * We need to sort the values, but we don't want to modify
     * the original array — so we copy it first.
     */
    double *sorted = (double *)safe_malloc(count * sizeof(double));
    memcpy(sorted, vals, count * sizeof(double));

    qsort(sorted, count, sizeof(double), compare_doubles);

    double median;
    if (count % 2 == 1) {
        median = sorted[count / 2];
    } else {
        /* Even count: average the two middle elements */
        median = (sorted[count / 2 - 1] + sorted[count / 2]) / 2.0;
    }

    free(sorted);
    return median;
}

static double calc_min(const double *vals, int count) {
    if (count == 0) return (double)NAN;

    double min_val = vals[0];
    for (int i = 1; i < count; i++) {
        if (vals[i] < min_val)
            min_val = vals[i];
    }
    return min_val;
}

static double calc_max(const double *vals, int count) {
    if (count == 0) return (double)NAN;

    double max_val = vals[0];
    for (int i = 1; i < count; i++) {
        if (vals[i] > max_val)
            max_val = vals[i];
    }
    return max_val;
}

/*
 * find_row_of_value — scans the original 2D data array (column col)
 * and returns the row index whose value equals 'target'.
 * Used to trace which student produced a min or max value.
 * Returns -1 if not found (shouldn't happen in normal use).
 */
static int find_row_of_value(CSVData *csv, int col, double target) {
    for (int i = 0; i < csv->row_count; i++) {
        if (!isnan(csv->data[i][col]) && csv->data[i][col] == target)
            return i;
    }
    return -1;
}

/*
 * copy_label — looks up the row label for a given row index
 * and returns a heap-allocated copy (caller must free it).
 * Returns NULL if no label column exists.
 */
static char *copy_label(CSVData *csv, int row_idx) {
    if (csv->label_col_idx == -1 || csv->row_labels == NULL)
        return NULL;
    if (row_idx < 0 || row_idx >= csv->row_count)
        return NULL;
    if (csv->row_labels[row_idx] == NULL)
        return NULL;

    char *copy = (char *)safe_malloc(strlen(csv->row_labels[row_idx]) + 1);
    strcpy(copy, csv->row_labels[row_idx]);
    return copy;
}

/* ------------------------------------------------------------------ */
/* Helper: returns 1 if the column has at least one numeric value     */
/* ------------------------------------------------------------------ */
static int col_has_numeric(CSVData *csv, int col) {
    for (int i = 0; i < csv->row_count; i++) {
        if (!isnan(csv->data[i][col])) return 1;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* compute_stats — public API                                          */
/* Allocates and returns an array of ColumnStats (one per numeric     */
/* column). Sets *out_count to the array length.                      */
/* Caller must free_stats() the returned array.                       */
/* ------------------------------------------------------------------ */
ColumnStats *compute_stats(CSVData *csv, int *out_count) {
    *out_count = 0;

    if (csv == NULL || csv->row_count == 0) {
        fprintf(stderr, "Error: No data to compute statistics on\n");
        return NULL;
    }

    /* Count how many columns are actually numeric */
    int num_numeric = 0;
    for (int j = 0; j < csv->col_count; j++) {
        if (col_has_numeric(csv, j)) num_numeric++;
    }

    if (num_numeric == 0) {
        fprintf(stderr, "Warning: No numeric columns found — nothing to compute\n");
        return NULL;
    }

    ColumnStats *stats = (ColumnStats *)safe_malloc(num_numeric * sizeof(ColumnStats));
    int idx = 0;

    for (int j = 0; j < csv->col_count; j++) {
        if (!col_has_numeric(csv, j)) continue;  /* skip text-only columns */

        int    valid_count;
        double *vals = get_column_values(csv, j, &valid_count);

        /* Copy the column name */
        stats[idx].col_name = (char *)safe_malloc(strlen(csv->headers[j]) + 1);
        strcpy(stats[idx].col_name, csv->headers[j]);

        stats[idx].valid_count = valid_count;
        stats[idx].mean        = calc_mean(vals, valid_count);
        stats[idx].median      = calc_median(vals, valid_count);
        stats[idx].min         = calc_min(vals, valid_count);
        stats[idx].max         = calc_max(vals, valid_count);

        /* Find which student (row) produced the min and max values */
        int min_row = find_row_of_value(csv, j, stats[idx].min);
        int max_row = find_row_of_value(csv, j, stats[idx].max);
        stats[idx].min_by = copy_label(csv, min_row);   /* NULL if no label col */
        stats[idx].max_by = copy_label(csv, max_row);

        free(vals);
        idx++;
    }

    *out_count = num_numeric;
    return stats;
}

/*
 * free_stats — release all memory owned by a ColumnStats array.
 * Must be called after compute_stats() when results are no longer needed.
 */
void free_stats(ColumnStats *stats, int count) {
    if (stats == NULL) return;

    for (int i = 0; i < count; i++) {
        free(stats[i].col_name);
        free(stats[i].min_by);   /* safe even if NULL */
        free(stats[i].max_by);
    }
    free(stats);
}
