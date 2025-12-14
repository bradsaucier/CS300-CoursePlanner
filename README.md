[![CI Build Status](https://github.com/bradsaucier/CS300-CoursePlanner/actions/workflows/ci.yml/badge.svg)](https://github.com/bradsaucier/CS300-CoursePlanner/actions/workflows/ci.yml)
[![License MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Language C++17](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](#build-and-run)

# CS 300 Portfolio - Course Planner (Data Structures and Algorithms)

## Mission overview

This repository contains portfolio artifacts for SNHU CS-300 (Data Structures and Algorithms):

- A course advising tool written in C++ that loads a course catalog from CSV
- A complexity analysis comparing vector, hash table, and binary search tree approaches

The application supports:
- Printing a complete course list in alphanumeric order by course number
- Printing a specific course, including its prerequisite IDs

## Table of contents

- [Included projects](#included-projects)
- [Build and run](#build-and-run)
- [CSV format](#csv-format)
- [Example session](#example-session)
- [System design](#system-design)
- [Project reflection - Module Eight journal](#project-reflection---module-eight-journal)
- [About the author](#about-the-author)
- [Academic integrity](#academic-integrity)

## Included projects

1. Project One - Complexity Analysis
   - File: `complexity_analysis.md`
   - Purpose: Runtime and memory tradeoff analysis across vector, hash table, and binary search tree approaches, with a recommendation grounded in Big O and operational constraints.

2. Project Two - Course Planner Implementation
   - File: `ProjectTwo.cpp` (place it under `src/` if you prefer a `src/ProjectTwo.cpp` layout)
   - Purpose: Loads a CSV course catalog into a hash table, prints a sorted course list, and prints a single course with prerequisites.

## Build and run

### Prerequisites

- CMake 3.20 or newer
- A C++17 compatible compiler (GCC, Clang, or MSVC)
- A CSV input file formatted as `courseNumber,courseTitle[,prereq1,prereq2,...]`

### Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Run

Linux or macOS:

```bash
./build/course_planner
```

Windows PowerShell:

```powershell
.\build\course_planner.exe
```

### CI

GitHub Actions executes a configure and build pass on pushes and pull requests using CMake Release mode.

## CSV format

Each line must follow:

```text
courseNumber,courseTitle[,prereq1,prereq2,...]
```

Example:

```csv
CSCI100,Introduction to Computer Science
CSCI200,Data Structures,CSCI100
CSCI300,Algorithms,CSCI200
MATH201,Discrete Mathematics
```

## Example session

```text
1. Load Data Structure.
2. Print Course List.
3. Print Course.
9. Exit
What would you like to do? 1
Enter the file name: courses.csv

1. Load Data Structure.
2. Print Course List.
3. Print Course.
9. Exit
What would you like to do? 2
Here is a sample schedule:
CSCI100, Introduction to Computer Science
CSCI200, Data Structures
CSCI300, Algorithms
MATH201, Discrete Mathematics

1. Load Data Structure.
2. Print Course List.
3. Print Course.
9. Exit
What would you like to do? 3
What course do you want to know about? CSCI300
CSCI300, Algorithms
Prerequisites: CSCI200
```

## System design

### Data structure selection

The dominant operation is lookup by course number (direct queries and prerequisite validation). A hash table supports average O(1) lookup, keeping interactive queries responsive.

Sorted output is handled on demand by extracting course records and sorting them, paying O(n log n) only for the menu option that requires ordering.

### Defensive input handling

The implementation is designed to be resilient to common CSV issues and user input errors:
- Course ID normalization to reduce mismatches from whitespace or casing
- UTF-8 BOM handling so the first key is not polluted by invisible bytes
- Malformed line rejection with safe continuation
- Post-load prerequisite validation to avoid false errors on forward references
- Line-based menu input parsing so invalid input does not poison the input stream

For deeper tradeoff detail across vector, hash table, and BST approaches, see `complexity_analysis.md`.

## Project reflection - Module Eight journal

This section answers the CS-300 Module Eight journal prompts and provides context for future review.

### What was the problem you were solving in the projects for this course?

The core problem was building an advising support tool that can load course data from a CSV file, store it in memory, and provide two user-facing outputs: a full course list sorted alphanumerically by course number, and a single course view that includes its prerequisite course IDs.

Project One required analyzing runtime and memory tradeoffs across candidate data structures (vector, hash table, and binary search tree) to justify an implementation choice. Project Two required turning that analysis into working, testable C++ code that behaves predictably under normal and bad inputs.

### How did you approach the problem? Consider why data structures are important to understand.

I approached the work by starting with required operations and their expected frequency, then selecting a structure that optimizes the dominant operation rather than optimizing everything equally.

In this tool, the dominant operation is lookup by course number. That lookup is used for direct queries and for prerequisite validation. A hash table provides average O(1) lookup, which keeps interactive use responsive and makes the validation pass efficient. The requirement for sorted output is addressed separately by extracting keys and sorting on demand, paying O(n log n) only when the user requests the sorted list.

This reinforced why data structures matter: the structure choice directly affects performance, complexity of implementation, and how easy it is to extend the program later. Choosing the right structure up front reduces both runtime cost and code risk.

### How did you overcome any roadblocks you encountered while going through the activities or project?

Most roadblocks came from input quality and control flow safety.

- CSV parsing issues: whitespace drift, missing fields, and inconsistent formatting can break key matching.
  - Mitigation: normalize course IDs (trim and consistent casing), reject malformed lines early, and continue safely.

- BOM and invisible characters: UTF-8 byte order mark artifacts can corrupt the first token in a file.
  - Mitigation: detect and strip BOM so the first course ID is correct.

- Prerequisite integrity: prerequisites may reference courses that appear later in the file (forward references) or may reference courses that do not exist.
  - Mitigation: load all courses first, then run a post-load referential integrity check that reports warnings without crashing.

- User input handling: invalid menu selections should not poison the input stream or crash the program.
  - Mitigation: read menu choices by line and parse integers cleanly; handle invalid input with a safe message and re-prompt.

These steps turned fragile failure modes into controlled behavior and kept the program predictable to operate.

### How has your work on this project expanded your approach to designing software and developing programs?

This work strengthened my habit of designing from requirements and failure modes instead of coding directly toward the happy path.

Project One required me to justify decisions using complexity and operational constraints, not intuition. Project Two made those tradeoffs visible by forcing an implementation that had to handle real input issues, produce correct outputs, and remain understandable to someone reviewing the code later.

I also became more systematic in debugging and iteration: isolate the failure, reproduce it reliably, apply the smallest safe change, and re-test the full flow (load, validate, print list, print single course).

### How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?

I focused on code that can be maintained by someone other than the author:

- Separation of concerns: parsing, storage, validation, sorting, and output are handled in distinct functions.
- Clear intent in naming: functions are named after what they do (load, validate, print list, print course).
- Guardrails by default: file open checks, normalization, and input parsing are treated as baseline requirements, not optional polish.
- Adaptability: the menu loop and discrete functions make it straightforward to add features (for example, additional search modes or reporting) without rewriting the data loading core.

Overall, this project pushed me toward readable structure, defensive programming, and design choices that remain valid as dataset size and requirements evolve.

## About the author

Bradley Saucier  
Retired Senior Master Sergeant (USAF) - B.S. Computer Science candidate at Southern New Hampshire University (STEM Project Management concentration) - B.A. Columbia University - AAS CCAF

## Academic integrity

This repository contains coursework artifacts produced for SNHU CS-300 and repackaged for portfolio review. All work in this repository is my own. If you reference or reuse any portion of this work, cite the repository and clearly distinguish original content from derived content.
repository and clearly distinguish original content from derived content.
