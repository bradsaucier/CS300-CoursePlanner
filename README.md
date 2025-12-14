[![CI Build Status](https://github.com/bradsaucier/CS300-CoursePlanner/actions/workflows/ci.yml/badge.svg)](https://github.com/bradsaucier/CS300-CoursePlanner/actions/workflows/ci.yml)
[![License MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Language C++17](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](#technical-specifications)

# CS 300 Portfolio - Course Planner (Data Structures and Algorithms)

## Mission Overview

Summary (BLUF): This repository contains my CS 300 portfolio artifacts, including a C++ course advising tool that loads a course catalog from CSV, prints a sorted course list, and displays a single course with prerequisites.

"First There" is how I approach building systems from scratch: establish the environment, validate inputs early, select the right data structure, and deliver software that someone else can run without surprises.

## Table of Contents

- [Portfolio Artifacts](#portfolio-artifacts)
- [After Action Report](#after-action-report)
- [Technical Specifications](#technical-specifications)
- [Design Intelligence](#design-intelligence)
- [Service Record](#service-record)
- [Acknowledgment and Academic Integrity](#acknowledgment-and-academic-integrity)

## Portfolio Artifacts

This repo contains two CS 300 projects:

1. Project One - Complexity Analysis
   - File: `complexity_analysis.md`
   - Purpose: runtime and memory tradeoff analysis across vector, hash table, and binary search tree approaches, with a recommendation grounded in Big O and operational constraints.

2. Project Two - Course Planner Implementation
   - File: `src/ProjectTwo.cpp`
   - Purpose: loads a CSV course catalog into a hash table, prints a sorted course list, and prints a single course with prerequisites.

## After Action Report

### What problem was I solving

Mission objective: Build a simple advising assistance tool that reads course data from a CSV file, stores it in memory, and supports two outputs on demand:

- Print a complete course list in alphanumeric order by course number
- Print a specific course, including its prerequisite IDs

The work had to be correct, predictable to operate, and efficient enough to scale beyond the small samples used in class.

### How I approached the problem and why data structures matter

Course of action: Define the required operations first, then select the data structure that best matches those operations.

The dominant operation in this program is lookup by course number (for both direct queries and prerequisite validation). A hash table supports average O(1) lookup, which keeps latency low during interactive use. Ordered output is handled when requested by extracting course IDs and sorting them, paying O(n log n) only for the menu option that requires ordering.

This course reinforced that data structures are not academic trivia. They are the mechanism that controls performance, complexity, and long-term maintainability.

### How I overcame roadblocks

Challenges (friction points) showed up in two places: input quality and defensive control flow.

- CSV parsing: whitespace, empty fields, and minor formatting drift can break key matching.
- Data integrity: prerequisites can reference courses that do not exist in the catalog.
- User input handling: invalid menu selections and file open failures must not crash the program.

I addressed these issues by treating validation as a required control, not optional polish:

- Normalize course IDs (trim and uppercase) to prevent mismatches caused by casing or whitespace.
- Strip UTF-8 BOM artifacts so the first key is not polluted by invisible bytes.
- Reject malformed lines early and continue safely.
- Validate prerequisites after load (post-load referential integrity check) so forward references do not produce false errors.
- Read menu input by line and parse clean integers only, so invalid input cannot poison the input stream.

### How this work expanded my approach to software design

Project One forced me to justify choices using measured tradeoffs instead of intuition. Project Two made those choices real by requiring a working system with visible consequences.

That combination expanded how I design software: I now anchor structure selection, validation strategy, and user-facing behavior in explicit requirements and complexity bounds. Debugging also became more systematic by iterating through an Observe-Orient-Decide-Act cycle: observe failure modes, orient on root cause, decide the smallest safe change, act and re-test.

### How this evolved the way I write maintainable, readable, adaptable code

This project pushed me toward code that can be maintained by someone other than the author:

- Separation of concerns: parsing, storage, validation, and presentation are distinct functions with clear responsibilities.
- Naming that matches intent: functions read like their purpose (load, validate, print list, print single).
- Guardrails first: file checks, normalization, and input parsing are treated as baseline requirements.
- Adaptability by design: a menu loop plus discrete functions makes it straightforward to extend functionality without rewriting core logic.

## Technical Specifications

### Deployment protocol

Prerequisites (verify before execution):

- CMake 3.20 or newer
- A C++17 compatible compiler (GCC, Clang, or MSVC)
- A CSV input file formatted as: `courseNumber,courseTitle[,prereq1,prereq2,...]`

Build sequence:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Run sequence (Linux or macOS):

```bash
./build/course_planner
```

Run sequence (Windows PowerShell):

```powershell
.\build\course_planner.exe
```

### Example CSV

```csv
CSCI100,Introduction to Computer Science
CSCI200,Data Structures,CSCI100
CSCI300,Algorithms,CSCI200
MATH201,Discrete Mathematics
```

### Example session output

```text
1. Load Data Structure
2. Print Course List
3. Print Course
9. Exit

Enter choice: 1
Enter file name: courses.csv
Data load complete.

Enter choice: 2
CSCI100, Introduction to Computer Science
CSCI200, Data Structures
CSCI300, Algorithms
MATH201, Discrete Mathematics

Enter choice: 3
Enter course number: CSCI300
CSCI300, Algorithms
Prerequisites: CSCI200
```

### CI validation

GitHub Actions executes a configure and build pass on pushes and pull requests using CMake Release mode. This keeps the build pipeline continuously validated.

## Design Intelligence

### Architecture summary

- Primary storage: hash table with separate chaining
  - Rationale: repeated course ID lookups are the dominant operation (interactive queries and prerequisite validation), and average O(1) lookup minimizes latency.
- Ordered output: extract all courses to a vector and sort by course number on demand
  - Rationale: the structure optimized for lookup is not ordered. Sorting at print time isolates the O(n log n) cost to the specific action that requires ordering.
- Data integrity: post-load prerequisite validation
  - Rationale: validation must occur after load to avoid false errors on forward references. Integrity warnings are reported without blocking program use.

For deeper performance intelligence and tradeoff detail across vector, hash table, and BST approaches, see `complexity_analysis.md`.

## Service Record

Bradley Saucier is a retired Senior Master Sergeant (SMSgt), U.S. Air Force (USAF). I am a Computer Science B.S. candidate at Southern New Hampshire University (STEM Project Management concentration) with a B.A. from Columbia University and an Associate of Applied Science (AAS) from the Community College of the Air Force (CCAF).

My professional edge comes from bridging operational discipline and engineering execution: precision, systems thinking, resource management, and calm leadership under constraints. In software terms, that translates to clear requirements, defensible architecture choices, and code that is validated, readable, and built to be operated by someone other than the author.

## Acknowledgment and Academic Integrity

This repository contains coursework artifacts produced for SNHU CS-300 (Data Structures and Algorithms) and repackaged here for portfolio review. Project One documents the design rationale and data structure tradeoffs. Project Two is the implemented C++ course planner that loads course data from CSV, supports lookup by course ID, and prints a sorted course list.

### Academic Integrity and Citation Policy

All work in this repository is my own and was completed in accordance with the Southern New Hampshire University Academic Integrity Policy. If you reference or reuse any portion of this work, cite the repository and clearly distinguish original content from derived content.
