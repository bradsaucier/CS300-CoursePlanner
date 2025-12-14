# CS-300 Course Planner - Hash Table Prerequisite Engine

[![CI Build Status](https://github.com/bradsaucier/CS300-CoursePlanner/actions/workflows/ci.yml/badge.svg)](https://github.com/bradsaucier/CS300-CoursePlanner/actions/workflows/ci.yml)
[![C++17](https://img.shields.io/badge/Standard-C%2B%2B17-blue?logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)
[![Status](https://img.shields.io/badge/Status-Portfolio_Project-8A2BE2)](#)

Course Planner is a C++17 console application that loads a course catalog from CSV, stores it in a custom hash table (separate chaining), validates prerequisite references, and supports fast lookups plus sorted catalog output. This repository is a portfolio presentation of CS-300 coursework focused on data structures, defensive parsing, and complexity tradeoffs.

## Table of contents

1. [Portfolio highlights](#portfolio-highlights)
2. [Quick start](#quick-start)
3. [CSV format](#csv-format)
4. [Usage](#usage)
5. [System architecture](#system-architecture)
6. [Design rationale and complexity](#design-rationale-and-complexity)
7. [Project structure](#project-structure)
8. [Troubleshooting](#troubleshooting)
9. [Project reflection - Module Eight journal](#project-reflection---module-eight-journal)
10. [License](#license)
11. [Citation](#citation)
12. [About the author](#about-the-author)
13. [Academic integrity](#academic-integrity)

## Portfolio highlights

1. Custom data structure - hash table implemented from scratch with separate chaining to demonstrate collision handling and manual memory management.
2. Algorithmic efficiency - O(1) average-case course lookup and O(n) catalog validation pass using hash table membership checks.
3. Defensive ingestion - input normalization (trim and consistent casing) plus UTF-8 BOM stripping to prevent hidden encoding defects from corrupting the first course ID.
4. Console UX and state management - load-gated menu options prevent invalid operations before the catalog is initialized.
5. CI signal - GitHub Actions build validates the project compiles in a clean environment.

## Quick start

### Prerequisites

1. A C++17 compiler (GCC, Clang, or MSVC).
2. CMake 3.20+.

### Build and run (recommended approach)

Run from the repository root so the CSV can be referenced by filename when prompted.

Linux or macOS:

```bash
cmake -S . -B build
cmake --build build
./build/course_planner
```

Windows (MSVC multi-config builds place the binary under Debug or Release):

```powershell
cmake -S . -B build
cmake --build build --config Debug
.uild\Debug\course_planner.exe
```

When prompted for the input file, enter the CSV filename (example: `courses.csv`) if it is in the current working directory, or provide a relative or absolute path.

## CSV format

Each line is:

`courseNumber,courseTitle[,prereq1,prereq2,...]`

Example:

```text
CSCI100,Introduction to Computer Science
CSCI200,Data Structures,CSCI100
CSCI300,Algorithms,CSCI200
MATH201,Discrete Mathematics
```

Notes:

1. Course IDs are treated case-insensitively after normalization (example: `csci300` and `CSCI300` are equivalent).
2. Prerequisites are validated after load. If a prerequisite references a course ID that is not present in the catalog, the loader reports it as missing.

## Usage

The application presents a menu-driven console interface:

1. Load data structure (CSV).
2. Print course list (sorted by course ID).
3. Print course (single course view with prerequisites).
4. Exit.

Operational guidance:

1. Load must occur before options 2 or 3.
2. Option 2 performs sort-on-demand (extract keys, sort, then print).
3. Option 3 performs a direct hash table lookup for the requested course ID.

## System architecture

```mermaid
flowchart TD
  A[CSV filename from user] --> B[Open file stream]
  B --> C[Read line]
  C --> D[Sanitize and normalize fields]
  D --> E[Insert course into hash table]
  E --> F[Repeat until EOF]
  F --> G[Post-load prerequisite validation]
  G --> H{Main menu}
  H --> I[Print sorted course list]
  H --> J[Lookup and print single course]
  I --> K[Extract keys and sort]
  J --> L[O(1) average lookup]
```

## Design rationale and complexity

Design choice summary:

1. Workload is read-heavy after ingestion (many lookups and prints).
2. Hash table provides O(1) average-case lookup, which reduces validation and query cost compared to linear scans.
3. Sorted output is handled separately by extracting keys and sorting on demand.

Complexity overview (high level):

1. Load - O(n) inserts (average-case).
2. Validate prerequisites - O(n) membership checks (average-case).
3. Print sorted list - O(n log n) due to sorting extracted keys.
4. Print single course - O(1) average-case lookup.

Full writeup:

1. [Complexity analysis](./docs/complexity_analysis.md)

## Project structure

```text
.
├── .github/workflows/ci.yml
├── .gitignore
├── CITATION.cff
├── CMakeLists.txt
├── LICENSE
├── README.md
├── docs/
│   └── complexity_analysis.md
└── src/
    └── ProjectTwo.cpp
```

## Troubleshooting

1. Symptom: file fails to load (example message: "Could not open file").
   Cause: the CSV path does not exist relative to the current working directory.
   Fix: run the executable from the repository root and enter the CSV filename, or provide a correct relative or absolute path.

2. Symptom: "Format error line X" (or a skipped line).
   Cause: malformed CSV row (missing required fields or invalid formatting).
   Fix: ensure each line contains at least `courseNumber,courseTitle` and uses commas as delimiters.

3. Symptom: "Missing prerequisite: <ID>" after load.
   Cause: a prerequisite references a course ID not present in the catalog.
   Fix: add the missing course row to the CSV or correct the prerequisite ID.

4. Symptom: course lookup fails for an ID that appears in the CSV.
   Cause: hidden BOM on the first line or unexpected whitespace or case in the input.
   Fix: ensure the first course ID is clean. The loader strips UTF-8 BOM and normalizes input, but verify the CSV is not corrupted and that IDs are consistent.

## Project reflection - Module Eight journal

This section answers the CS-300 Module Eight journal prompts and provides context for future review.

What was the problem you were solving in the projects for this course?
The core problem was building an advising support tool that can load course data from a CSV file, store it in memory, and provide two user-facing outputs: a full course list sorted alphanumerically by course number, and a single course view that includes its prerequisite course IDs.

Project One required analyzing runtime and memory tradeoffs across candidate data structures (vector, hash table, and binary search tree) to justify an implementation choice. Project Two required turning that analysis into working, testable C++ code that behaves predictably under normal and bad inputs.

How did you approach the problem? Consider why data structures are important to understand.
I approached the work by starting with required operations and their expected frequency, then selecting a structure that optimizes the dominant operation rather than optimizing everything equally.

In this tool, the dominant operation is lookup by course number. That lookup is used for direct queries and for prerequisite validation. A hash table provides average O(1) lookup, which keeps interactive use responsive and makes the validation pass efficient. The requirement for sorted output is addressed separately by extracting keys and sorting on demand, paying O(n log n) only when the user requests the sorted list.

This reinforced why data structures matter: the structure choice directly affects performance, complexity of implementation, and how easy it is to extend the program later. Choosing the right structure up front reduces both runtime cost and code risk.

How did you overcome any roadblocks you encountered while going through the activities or project?
Most roadblocks came from input quality and control flow safety.

CSV parsing issues: whitespace drift, missing fields, and inconsistent formatting can break key matching.

Mitigation: normalize course IDs (trim and consistent casing), reject malformed lines early, and continue safely.
BOM and invisible characters: UTF-8 byte order mark artifacts can corrupt the first token in a file.

Mitigation: detect and strip BOM so the first course ID is correct.
Prerequisite integrity: prerequisites may reference courses that appear later in the file (forward references) or may reference courses that do not exist.

Mitigation: load all courses first, then run a post-load referential integrity check that reports warnings without crashing.
User input handling: invalid menu selections should not poison the input stream or crash the program.

Mitigation: read menu choices by line and parse integers cleanly; handle invalid input with a safe message and re-prompt.
These steps turned fragile failure modes into controlled behavior and kept the program predictable to operate.

How has your work on this project expanded your approach to designing software and developing programs?
This work strengthened my habit of designing from requirements and failure modes instead of coding directly toward the happy path.

Project One required me to justify decisions using complexity and operational constraints, not intuition. Project Two made those tradeoffs visible by forcing an implementation that had to handle real input issues, produce correct outputs, and remain understandable to someone reviewing the code later.

I also became more systematic in debugging and iteration: isolate the failure, reproduce it reliably, apply the smallest safe change, and re-test the full flow (load, validate, print list, print single course).

How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?
I focused on code that can be maintained by someone other than the author:

Separation of concerns: parsing, storage, validation, sorting, and output are handled in distinct functions.
Clear intent in naming: functions are named after what they do (load, validate, print list, print course).
Guardrails by default: file open checks, normalization, and input parsing are treated as baseline requirements, not optional polish.
Adaptability: the menu loop and discrete functions make it straightforward to add features (for example, additional search modes or reporting) without rewriting the data loading core.
Overall, this project pushed me toward readable structure, defensive programming, and design choices that remain valid as dataset size and requirements evolve.

## License

MIT License. See [LICENSE](./LICENSE).

## Citation

See [CITATION.cff](./CITATION.cff).

## About the author

Bradley Saucier

## Academic integrity

This repository contains coursework artifacts produced for SNHU CS-300 and repackaged for portfolio review. All work in this repository is my own. If you reference or reuse any portion of this work, cite the repository and clearly distinguish original content from derived content.
