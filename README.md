# CSV Data Analyzer in C

A command-line tool that reads a CSV file and computes statistical summaries
(mean, median, min, max) for every numeric column. Also identifies which row
(e.g. student name) produced the lowest and highest value per column.

Built as a mini project after completing the **C for Everyone** specialization
(Part 1 & Part 2) by UC Santa Cruz on Coursera — July 2024.

---

## Features

- Parses any CSV file via command-line argument
- Auto-detects and skips text columns (e.g. Name)
- Computes mean, median, min, max, and valid row count per numeric column
- Shows which student / entry scored the lowest and highest in each column
- Optional export to a `.txt` summary file
- Zero external dependencies — pure C11 standard library

---

## Project Structure

```
csv-data-analyzer/
├── src/
│   ├── main.c          # Entry point, CLI argument handling
│   ├── csv_parser.c    # File reading, CSV parsing, label detection
│   ├── stats.c         # Mean, median, min, max computation
│   ├── display.c       # Console output and file export
│   └── utils.c         # Memory helpers, string utilities
├── include/
│   ├── csv_parser.h
│   ├── stats.h
│   ├── display.h
│   └── utils.h
├── data/
│   └── sample.csv      # Sample student marks dataset (15 rows)
├── docs/
│   ├── CSV_Analyzer_Explainer.docx   # Plain-English explainer (non-technical)
│   └── make_explainer.py             # Script that generates the explainer doc
├── CSV_Data_Analyzer_Project_Report.docx
├── Makefile
└── README.md
```

---

## Requirements

| Tool | Version | Purpose |
|------|---------|---------|
| GCC (MinGW-w64) | 13.x+ | C11 compiler |
| VS Code | Latest | Editor |
| Make (optional) | 4.x+ | Build automation |

**Windows setup:**
1. Install MSYS2 from https://www.msys2.org
2. Run: `pacman -S mingw-w64-ucrt-x86_64-gcc`
3. Add `C:\msys64\ucrt64\bin` to your system PATH

---

## Build & Run

**Using Make:**
```bash
make
./build/csv_analyzer data/sample.csv
```

**Without Make (manual compile):**
```bash
gcc -Wall -Wextra -std=c11 -g -Iinclude src/main.c src/csv_parser.c src/stats.c src/display.c src/utils.c -o build/csv_analyzer -lm
./build/csv_analyzer data/sample.csv
```

**Export results to a file:**
```bash
./build/csv_analyzer data/sample.csv -o build/summary.txt
```

---

## Sample Output

```
  Reading: data/sample.csv
  Parsed:  15 rows  x  7 columns

  ================================================================
   CSV DATA ANALYZER  —  Statistical Summary
  ================================================================

+----------------------+----------------+----------------+----------------+----------------+
| Column               |           Mean |         Median |            Min |            Max |
+----------------------+----------------+----------------+----------------+----------------+
| Maths                |        79.0333 |        83.0000 |        55.0000 |        95.0000 |
| Physics              |        75.3333 |        76.0000 |        55.0000 |        94.5000 |
| Chemistry            |        78.6333 |        80.5000 |        57.5000 |        93.5000 |
| Biology              |        78.6667 |        80.5000 |        58.5000 |        94.0000 |
| Attendance           |        86.8000 |        88.0000 |        70.0000 |        98.0000 |
+----------------------+----------------+----------------+----------------+----------------+

  Column                Min       Lowest scored by    Max       Highest scored by
  --------------------  --------  ------------------  --------  ------------------
  Maths                 55.00     Hank                95.00     Charlie
  Physics               55.00     Diana               94.50     Grace
  Chemistry             57.50     Hank                93.50     Karen
  Biology               58.50     Diana               94.00     Olivia
  Attendance            70.00     Hank                98.00     Charlie
```

---

## Concepts Used

- Dynamic memory allocation (`malloc`, `realloc`, `free`)
- Pointer arithmetic and 2D pointer arrays
- File I/O (`fopen`, `fgets`, `fclose`)
- String parsing with `strtok`
- Sorting with `qsort` (for median)
- Modular programming with header files and separate compilation units

---

## Course Reference

| Course | Platform | Completed |
|--------|----------|-----------|
| C for Everyone, Part 1: Programming Fundamentals | UC Santa Cruz — Coursera | July 2024 |
| C for Everyone, Part 2: Structured Programming   | UC Santa Cruz — Coursera | July 2024 |

---

## Future Enhancements

- Standard deviation and variance
- Support for TSV and pipe-separated files
- Column filtering via CLI flags
- JSON / CSV export formats
