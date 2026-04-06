/*
 * csv_parser.c — CSV file reading and parsing
 * Opens a CSV file, reads the header row, then reads all data rows
 * into a dynamically allocated 2D array of doubles.
 *
 * Non-numeric cells are stored as NaN so stats.c can skip them.
 *
 * Course: C for Everyone (Part 1 & 2) — UC Santa Cruz, Coursera
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "csv_parser.h"
#include "utils.h"

/*
 * split_csv_line — tokenizes a comma-separated line using strtok.
 * Stores trimmed pointers into the tokens array.
 * NOTE: modifies 'line' in-place (strtok inserts '\0' at delimiters).
 * Returns the number of tokens found.
 *
 * Limitation: does not handle quoted fields containing commas.
 * That's a known simplification for this version.
 */
static int split_csv_line(char *line, char **tokens, int max_tokens) {
    int count = 0;
    char *token = strtok(line, ",");

    while (token != NULL && count < max_tokens) {
        tokens[count++] = trim_whitespace(token);
        token = strtok(NULL, ",");
    }

    return count;
}

/*
 * csv_open_and_parse — main parser function.
 * Reads the file at 'filepath', parses header + data rows,
 * returns a heap-allocated CSVData struct (caller must csv_free it).
 * Returns NULL on any critical error.
 */
CSVData *csv_open_and_parse(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return NULL;
    }

    CSVData *csv = (CSVData *)safe_malloc(sizeof(CSVData));
    csv->headers        = NULL;
    csv->data           = NULL;
    csv->row_count      = 0;
    csv->col_count      = 0;
    csv->row_labels     = NULL;
    csv->label_col_idx  = -1;   /* -1 means "not found yet" */
    csv->label_col_name = NULL;

    char  line[MAX_LINE_LEN];
    char *tokens[MAX_COLS];

    /* ---- Parse header row ---- */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fprintf(stderr, "Error: File is empty or unreadable\n");
        fclose(fp);
        free(csv);
        return NULL;
    }

    /* Strip carriage return / newline */
    line[strcspn(line, "\r\n")] = '\0';

    int col_count = split_csv_line(line, tokens, MAX_COLS);
    if (col_count == 0) {
        fprintf(stderr, "Error: Header row is empty — no columns detected\n");
        fclose(fp);
        free(csv);
        return NULL;
    }

    csv->col_count = col_count;
    csv->headers   = (char **)safe_malloc(col_count * sizeof(char *));

    for (int i = 0; i < col_count; i++) {
        csv->headers[i] = (char *)safe_malloc(strlen(tokens[i]) + 1);
        strcpy(csv->headers[i], tokens[i]);
    }

    /* ---- Parse data rows ---- */
    /*
     * We start with a capacity of 128 rows and double it whenever we run out.
     * This is the standard "doubling" strategy taught in the DSA section —
     * amortized O(1) per insertion.
     */
    int capacity = 128;
    csv->data       = (double **)safe_malloc(capacity * sizeof(double *));
    csv->row_labels = (char   **)safe_malloc(capacity * sizeof(char *));

    int row = 0;
    char line_copy[MAX_LINE_LEN];  /* strtok destroys the string, so we work on a copy */

    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\r\n")] = '\0';

        /* Skip blank lines */
        char temp[MAX_LINE_LEN];
        strcpy(temp, line);
        if (strlen(trim_whitespace(temp)) == 0) continue;

        /* Grow both arrays together if needed */
        if (row >= capacity) {
            capacity *= 2;
            csv->data       = (double **)safe_realloc(csv->data,       capacity * sizeof(double *));
            csv->row_labels = (char   **)safe_realloc(csv->row_labels, capacity * sizeof(char *));
        }

        csv->data[row]       = (double *)safe_malloc(col_count * sizeof(double));
        csv->row_labels[row] = NULL;   /* default — filled in below if label col found */

        /* Parse this row's values */
        strcpy(line_copy, line);
        int n_tokens = split_csv_line(line_copy, tokens, MAX_COLS);

        for (int j = 0; j < col_count; j++) {
            if (j < n_tokens && is_numeric(tokens[j])) {
                csv->data[row][j] = atof(tokens[j]);
            } else {
                /*
                 * Mark non-numeric / missing cells as NaN.
                 * stats.c uses isnan() to skip these during computation.
                 */
                csv->data[row][j] = (double)NAN;

                /*
                 * Label column detection:
                 * The FIRST column that contains a non-numeric, non-empty
                 * value is treated as the "label column" (e.g. student names).
                 * Once identified (label_col_idx >= 0) we stick with it.
                 */
                if (j < n_tokens && strlen(tokens[j]) > 0) {
                    if (csv->label_col_idx == -1) {
                        /* First time seeing a text column — lock it in */
                        csv->label_col_idx  = j;
                        csv->label_col_name = (char *)safe_malloc(strlen(csv->headers[j]) + 1);
                        strcpy(csv->label_col_name, csv->headers[j]);
                    }
                    if (csv->label_col_idx == j) {
                        /* Save this row's label string */
                        csv->row_labels[row] = (char *)safe_malloc(strlen(tokens[j]) + 1);
                        strcpy(csv->row_labels[row], tokens[j]);
                    }
                }
            }
        }

        row++;
    }

    csv->row_count = row;
    fclose(fp);

    if (row == 0) {
        fprintf(stderr, "Warning: CSV has a header row but no data rows\n");
    }

    return csv;
}

/*
 * csv_free — releases all heap memory owned by a CSVData struct.
 * Must be called once for every successful csv_open_and_parse().
 */
void csv_free(CSVData *csv) {
    if (csv == NULL) return;

    /* Free each header string */
    for (int i = 0; i < csv->col_count; i++) {
        free(csv->headers[i]);
    }
    free(csv->headers);

    /* Free each data row, then the array of row-pointers */
    for (int i = 0; i < csv->row_count; i++) {
        free(csv->data[i]);
    }
    free(csv->data);

    /* Free row labels (each individual string, then the array itself) */
    if (csv->row_labels != NULL) {
        for (int i = 0; i < csv->row_count; i++) {
            free(csv->row_labels[i]);   /* safe even if NULL */
        }
        free(csv->row_labels);
    }
    free(csv->label_col_name);  /* safe even if NULL */

    free(csv);
}
