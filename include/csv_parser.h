#ifndef CSV_PARSER_H
#define CSV_PARSER_H

/*
 * csv_parser.h — CSV file reading and parsing
 * Defines CSVData struct and parser function declarations
 */

#define MAX_LINE_LEN  4096
#define MAX_COLS      256

/* Holds the entire parsed CSV dataset */
typedef struct {
    char   **headers;       /* Column names from header row              */
    double **data;          /* 2D array: data[row][col]                  */
    int      row_count;     /* Number of data rows (not header)          */
    int      col_count;     /* Number of columns                         */

    /* Label tracking — first text column (e.g. "Name") is auto-detected */
    char   **row_labels;    /* String value per row from the label column */
    int      label_col_idx; /* Index of the label column (-1 if none)    */
    char    *label_col_name;/* Name of the label column (e.g. "Name")    */
} CSVData;

CSVData *csv_open_and_parse(const char *filepath);
void     csv_free(CSVData *csv);

#endif /* CSV_PARSER_H */
