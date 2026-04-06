/*
 * main.c — CSV Data Analyzer entry point
 * Parses CLI arguments, orchestrates the analysis pipeline:
 *   parse CSV  →  compute stats  →  display  →  (optional) export
 *
 * Usage:
 *   csv_analyzer <input.csv>
 *   csv_analyzer <input.csv> -o <output.txt>
 *
 * Course: C for Everyone (Part 1 & 2) — UC Santa Cruz, Coursera
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csv_parser.h"
#include "stats.h"
#include "display.h"

#define VERSION "1.0.0"

/* ------------------------------------------------------------------ */
/* print_usage — show how to invoke the program                       */
/* ------------------------------------------------------------------ */
static void print_usage(const char *prog_name) {
    printf("\n  CSV Data Analyzer v%s\n", VERSION);
    printf("  --------------------------------\n");
    printf("  Usage:  %s <input.csv> [-o output.txt]\n\n", prog_name);
    printf("  <input.csv>        Path to the CSV file to analyze\n");
    printf("  -o <output.txt>    (Optional) Export summary to a text file\n");
    printf("  -h / --help        Show this help message\n\n");
    printf("  Examples:\n");
    printf("    %s data/sample.csv\n", prog_name);
    printf("    %s data/sample.csv -o results/summary.txt\n\n", prog_name);
}

/* ------------------------------------------------------------------ */
/* main — program entry point                                          */
/* ------------------------------------------------------------------ */
int main(int argc, char *argv[]) {

    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file  = NULL;
    const char *output_file = NULL;

    /* Parse command-line arguments */
    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;

        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -o flag requires a filename argument\n");
                return EXIT_FAILURE;
            }
            output_file = argv[++i];

        } else {
            /* Treat any unrecognised argument as the input file */
            if (input_file != NULL) {
                fprintf(stderr, "Error: Multiple input files specified\n");
                return EXIT_FAILURE;
            }
            input_file = argv[i];
        }
    }

    if (input_file == NULL) {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    /* ---- Step 1: Parse the CSV file ---- */
    printf("\n  Reading: %s\n", input_file);

    CSVData *csv = csv_open_and_parse(input_file);
    if (csv == NULL) {
        return EXIT_FAILURE;
    }

    printf("  Parsed:  %d rows  x  %d columns\n", csv->row_count, csv->col_count);

    /* ---- Step 2: Compute statistics ---- */
    int stats_count = 0;
    ColumnStats *stats = compute_stats(csv, &stats_count);

    if (stats == NULL) {
        csv_free(csv);
        return EXIT_FAILURE;
    }

    /* ---- Step 3: Display on console ---- */
    print_summary(stats, stats_count);

    /* ---- Step 4: Export to file if -o was given ---- */
    if (output_file != NULL) {
        if (export_summary(stats, stats_count, output_file) != 0) {
            /* Export failed — print error but don't crash */
            fprintf(stderr, "Warning: Export to '%s' failed\n", output_file);
        }
    }

    /* ---- Cleanup — free all heap memory ---- */
    free_stats(stats, stats_count);
    csv_free(csv);

    return EXIT_SUCCESS;
}
