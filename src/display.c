/*
 * display.c — Console output formatting and file export
 * Prints results as an aligned table; optionally writes to a .txt file.
 *
 * Course: C for Everyone (Part 1 & 2) — UC Santa Cruz, Coursera
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "display.h"

/* Column widths for the console table */
#define NAME_WIDTH  20
#define STAT_WIDTH  14

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

/* Print a horizontal separator row matching the table layout */
static void print_separator(void) {
    printf("+----------------------");     /* name column */
    for (int i = 0; i < 4; i++)            /* 4 stat columns */
        printf("+----------------");
    printf("+\n");
}

/* Print a single stat cell — shows N/A if value is NaN */
static void print_stat_cell(double val) {
    if (isnan(val)) {
        printf("| %*s ", STAT_WIDTH, "N/A");
    } else {
        printf("| %*.4f ", STAT_WIDTH, val);
    }
}

/* ------------------------------------------------------------------ */
/* print_summary — display results on the console as a formatted table*/
/* ------------------------------------------------------------------ */
void print_summary(ColumnStats *stats, int count) {
    if (stats == NULL || count == 0) {
        printf("No numeric columns to display.\n");
        return;
    }

    printf("\n");
    printf("  ============================================================"
           "====================\n");
    printf("   CSV DATA ANALYZER  —  Statistical Summary\n");
    printf("  ============================================================"
           "====================\n\n");

    print_separator();
    printf("| %-*s | %*s | %*s | %*s | %*s |\n",
           NAME_WIDTH, "Column",
           STAT_WIDTH, "Mean",
           STAT_WIDTH, "Median",
           STAT_WIDTH, "Min",
           STAT_WIDTH, "Max");
    print_separator();

    for (int i = 0; i < count; i++) {
        /* Truncate very long column names so the table stays aligned */
        char display_name[NAME_WIDTH + 1];
        strncpy(display_name, stats[i].col_name, NAME_WIDTH);
        display_name[NAME_WIDTH] = '\0';

        printf("| %-*s ", NAME_WIDTH, display_name);
        print_stat_cell(stats[i].mean);
        print_stat_cell(stats[i].median);
        print_stat_cell(stats[i].min);
        print_stat_cell(stats[i].max);
        printf("|\n");
    }

    print_separator();

    /* Print per-column valid row counts below the table */
    printf("\n  Valid rows used per column:\n  ");
    for (int i = 0; i < count; i++) {
        printf("  %s: %d", stats[i].col_name, stats[i].valid_count);
        if (i < count - 1) printf("  |");
    }
    printf("\n");

    /*
     * Student highlights — only shown if label data exists (min_by != NULL).
     * Shows WHO scored the lowest and highest in each subject.
     */
    if (stats[0].min_by != NULL) {
        printf("\n  %-20s  %-8s  %-18s  %-8s  %-18s\n",
               "Column", "Min", "Lowest scored by", "Max", "Highest scored by");
        printf("  %-20s  %-8s  %-18s  %-8s  %-18s\n",
               "--------------------", "--------",
               "------------------", "--------", "------------------");

        for (int i = 0; i < count; i++) {
            if (stats[i].min_by == NULL) continue;  /* skip cols with no label */
            printf("  %-20s  %-8.2f  %-18s  %-8.2f  %-18s\n",
                   stats[i].col_name,
                   stats[i].min,  stats[i].min_by,
                   stats[i].max,  stats[i].max_by);
        }
        printf("\n");
    } else {
        printf("\n");
    }
}

/* ------------------------------------------------------------------ */
/* export_summary — write the same results to a plain-text file       */
/* Returns 0 on success, -1 on file error.                            */
/* ------------------------------------------------------------------ */
int export_summary(ColumnStats *stats, int count, const char *output_path) {
    FILE *fp = fopen(output_path, "w");
    if (fp == NULL) {
        perror("Error: Could not open output file for writing");
        return -1;
    }

    fprintf(fp, "CSV Data Analyzer — Statistical Summary\n");
    fprintf(fp, "========================================\n\n");

    /* Header row */
    fprintf(fp, "%-20s  %12s  %12s  %12s  %12s  %8s\n",
            "Column", "Mean", "Median", "Min", "Max", "Count");
    fprintf(fp, "%-20s  %12s  %12s  %12s  %12s  %8s\n",
            "--------------------",
            "------------", "------------",
            "------------", "------------",
            "--------");

    for (int i = 0; i < count; i++) {
        if (isnan(stats[i].mean)) {
            fprintf(fp, "%-20s  %12s  %12s  %12s  %12s  %8d\n",
                    stats[i].col_name,
                    "N/A", "N/A", "N/A", "N/A",
                    stats[i].valid_count);
        } else {
            fprintf(fp, "%-20s  %12.4f  %12.4f  %12.4f  %12.4f  %8d\n",
                    stats[i].col_name,
                    stats[i].mean,
                    stats[i].median,
                    stats[i].min,
                    stats[i].max,
                    stats[i].valid_count);
        }
    }

    /* Student highlights section in the export file */
    if (count > 0 && stats[0].min_by != NULL) {
        fprintf(fp, "\nStudent Highlights (Min / Max per column)\n");
        fprintf(fp, "-----------------------------------------\n");
        fprintf(fp, "%-20s  %-8s  %-18s  %-8s  %-18s\n",
                "Column", "Min", "Lowest scored by", "Max", "Highest scored by");
        fprintf(fp, "%-20s  %-8s  %-18s  %-8s  %-18s\n",
                "--------------------", "--------",
                "------------------", "--------", "------------------");
        for (int i = 0; i < count; i++) {
            if (stats[i].min_by == NULL) continue;
            fprintf(fp, "%-20s  %-8.2f  %-18s  %-8.2f  %-18s\n",
                    stats[i].col_name,
                    stats[i].min, stats[i].min_by,
                    stats[i].max, stats[i].max_by);
        }
    }

    fclose(fp);
    printf("  Summary exported to: %s\n\n", output_path);
    return 0;
}
