# CS 300 - Data Structures and Algorithms

## Portfolio artifact

This repository contains two artifacts from CS 300:

1. Project One - analysis of runtime and memory tradeoffs across data structures (see docs/complexity.md).
2. Project Two - working C++ program that loads a course catalog, supports lookup by course ID, and prints the full course list in alphanumeric order (see ProjectTwo.cpp).

## Module Eight Journal - reflection

### What problem was I solving?

I implemented a course advising tool that reads courses and prerequisites from a CSV file, stores them in memory, and supports two primary outputs: printing a sorted course list and printing details for a single course (including prerequisites).

### How did I approach the problem, and why do data structures matter?

I started by listing the required operations: load data, look up a course by ID, and produce sorted output. I selected data structures based on those operations rather than convenience. For fast lookup by course ID, a hash table provides average case constant time access. For sorted output, I extract items into a vector and sort when the user requests a full listing, which keeps the overall design simple and efficient for interactive use.

### How did I overcome roadblocks?

The main friction points were CSV parsing and defensive input handling. I addressed parsing issues by trimming tokens, handling empty prerequisite fields, and validating that each line produced a usable course record. I improved robustness by validating menu choices, checking file open errors, and keeping program flow predictable when inputs are invalid.

### How did this work expand my approach to designing software?

This project reinforced an operation driven design habit: define requirements, map operations to data structures, then implement the simplest flow that meets the constraints. It also strengthened my habit of justifying design choices with runtime and memory characteristics, not guesswork.

### How has this evolved the way I write maintainable, readable, and adaptable programs?

I now place more emphasis on separation of concerns (parsing, storage, presentation), naming that matches intent, and guardrails around inputs and failures. The result is code that is easier to reason about, easier to extend (for example, adding more menu actions), and less fragile when the input data is imperfect.

## Build and run

This project can be built with CMake.

### Requirements

1. CMake 3.20 or newer
2. A C++17 compatible compiler

### Commands

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/course_planner
```

## Data files

1. data/ABCU_Advising_Program_Input.csv - course catalog used for the assignment (if included in your repo).
2. data/sample_courses.csv - sanitized sample dataset suitable for public viewing and quick tests (if included in your repo).

## Program use

1. Start the program.
2. Load a CSV file when prompted.
3. Use the menu to print the sorted course list or print a single course and its prerequisites.

## Design notes

1. Lookup structure: hash table with chaining for average case O(1) access by course ID.
2. Sorted output: gather courses into a vector and sort on demand.

