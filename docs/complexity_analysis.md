# Complexity Analysis

This document is a Markdown conversion of the Project One runtime and memory tradeoff analysis and pseudocode.

## Pseudocode

### Vector

```text
STRUCT Course
String courseNumber
String courseTitle
Vector of String prerequisites
END STRUCT
Assumed standard string utilities exist globally:
Trim, Split(line, ","), ToUpper, RemoveBOM(firstTokenIfPresent)
FUNCTION NormalizeId(raw : String) RETURNS String
RETURN ToUpper(Trim(raw))
END FUNCTION
FUNCTION ParseLineToCourse(line : String, lineNumber : Integer) RETURNS (Boolean ok, Course c, String err)
Course c
c.prerequisites <- empty Vector of String
err <- ""
END FUNCTION
FUNCTION FindCourse_Vector(list : Vector of Course, id : String) RETURNS Course OR NULL
String target <- NormalizeId(id)
END FUNCTION
FUNCTION PrintSingleCourse_Vector(list : Vector of Course, id : String)
Course c <- FindCourse_Vector(list, id)
IF c is NULL THEN
PRINT "Course not found: " + id
RETURN
END IF
END FUNCTION
FUNCTION PrintAllCoursesSorted_Vector(list : Vector of Course)
NOTE: sorts list in place by courseNumber
END FUNCTION
```

### Hash Table

```text
STRUCT HashTable
Integer bucketCount
List of List of Course buckets
END STRUCT
FUNCTION InitHashTable(REFERENCE table : HashTable, bucketCount : Integer)
table.bucketCount <- bucketCount
table.buckets <- list of bucketCount empty lists of Course
END FUNCTION
FUNCTION Hash(key : String) RETURNS Integer
Integer hashValue <- 0
END FUNCTION
FUNCTION IndexForKey(key : String, bucketCount : Integer) RETURNS Integer
RETURN Hash(key) MOD bucketCount
END FUNCTION
FUNCTION InsertCourse_Hash(REFERENCE table : HashTable, c : Course)
Integer idx <- IndexForKey(c.courseNumber, table.bucketCount)
END FUNCTION
FUNCTION FindCourse_Hash(table : HashTable, id : String) RETURNS Course OR NULL
String target <- NormalizeId(id)
Integer idx <- IndexForKey(target, table.bucketCount)
END FUNCTION
FUNCTION LoadCourses_Hash(fileName : String, REFERENCE table : HashTable) RETURNS Boolean
InitHashTable(table, 50)
END FUNCTION
FUNCTION ValidatePrerequisites_Hash(table : HashTable)
Integer errorCount <- 0
END FUNCTION
FUNCTION PrintAllCoursesSorted_Hash(table : HashTable)
Vector of String keys <- empty Vector of String
END FUNCTION
FUNCTION PrintSingleCourse_Hash(table : HashTable, id : String)
Course c <- FindCourse_Hash(table, id)
IF c IS NULL THEN
PRINT "Course not found: " + id
RETURN
END IF
END FUNCTION
```

### Binary Search Tree

```text
STRUCT Node
Course course
Node left
Node right
END STRUCT
STRUCT BinarySearchTree
Node root
END STRUCT
STRUCT PrereqRef
String parentId
String prereqId
END STRUCT
FUNCTION DestroyTree(node : Node)
IF node == null THEN
RETURN
END IF
END FUNCTION
FUNCTION InsertNode(node : Node, c : Course) RETURNS Node
IF node == null THEN
Node n
n.course <- c
n.left <- null
n.right <- null
RETURN n
END IF
END FUNCTION
FUNCTION FindCourseNode(node : Node, id : String) RETURNS Node
String target <- NormalizeId(id)
Node current <- node
END FUNCTION
FUNCTION LoadCourses_BST(fileName : String, REFERENCE bst : BinarySearchTree) RETURNS Boolean
OPEN inputFile fileName
IF inputFile not open THEN
PRINT "Error: cannot open file " + fileName
RETURN FALSE
END IF
END FUNCTION
FUNCTION ValidatePrerequisites_BST(bst : BinarySearchTree, prereqRefs : Vector of PrereqRef)
Integer errorCount <- 0
END FUNCTION
FUNCTION PrintInOrder(node : Node)
IF node == null THEN
RETURN
END IF
END FUNCTION
FUNCTION PrintSingleCourse_BST(bst : BinarySearchTree, id : String)
Node n <- FindCourseNode(bst.root, id)
IF n == null THEN
PRINT "Course not found: " + id
RETURN
END IF
END FUNCTION
```

### Menu

```text
FUNCTION Menu(driverType : String)
Boolean loaded <- FALSE
Integer choice <- 0
END FUNCTION
```

## Runtime Analysis

### Scope

This analysis covers the load and create phase only. It includes opening the file, reading n lines, parsing, creating Course objects, inserting, and validating that all prerequisites exist. It excludes the menu loop and excludes Option 2 and Option 3 runtime. Option 2 sorting (when needed) adds O(n log n) per invocation but is not part of the load-and-create analysis. Assumes bounded line length; per-line parsing cost is treated as constant relative to n.

### Variables

n = number of courses
m = average prerequisites per course
h = BST height (worst case h = n, balanced case h = log n)

### BST Note

LoadCourses_BST includes duplicate detection via FindCourseNode before insertion. This adds an additional O(n * h) term, but the load phase remains O(n * h) by Big O simplification.

### Runtime Summary

| Structure | Load and create (average) | Load and create (worst case) | Notes |
|---|---|---|---|
| Vector | O(n^2 * m) | O(n^2 * m) | Validation performs nested linear searches |
| Hash table (separate chaining) | O(n * m) | O(n^2 * m) | Average assumes good distribution; worst case collision chain approaches n |
| BST (non-self-balancing) | O(n * m * h) | O(n^2 * m) | h is tree height; worst case h = n; balanced h = log n |

### Representative Cost Chart (load and create)

| Step | Cost per execution | Executions | Contribution |
|---|---:|---:|---|
| Open file | 1 | 1 | O(1) |
| WHILE condition | 1 | n + 1 | O(n) |
| ParseLineToCourse | O(1) (bounded line length) | n | O(n) |
| Insert (Vector) | 1 | n | O(n) |
| Insert (Hash) | O(1) avg, O(n) worst | n | O(n) avg, O(n^2) worst |
| Insert (BST) | O(h) | n | O(n * h) |
| Duplicate check (BST) | O(h) | n | O(n * h) |
| Prereq lookups total | varies by structure | n * m | see below |
| Lookup per prereq (Vector) | O(n) | n * m | O(n^2 * m) |
| Lookup per prereq (Hash) | O(1) avg, O(n) worst | n * m | O(n * m) avg, O(n^2 * m) worst |
| Lookup per prereq (BST) | O(h) | n * m | O(n * m * h) |

## Advantages, Disadvantages, and Recommendation

### Vector

Vector
Advantages
Simple implementation and minimal overhead
Fast sequential access
Disadvantages
Prerequisite validation uses nested linear searches: O(n^2 * m)
Single course lookup is O(n)
Option 2 requires sorting
Duplicates are permitted by design; FindCourse_Vector returns the first match

### Hash Table

Hash Table
Advantages
Average O(1) lookup makes validation efficient: O(n * m) average
Option 3 lookups are fast
Prerequisite titles are resolved via repeated lookups
Disadvantages
Unordered storage; Option 2 requires extracting keys and sorting: O(n log n)
Worst case insertion and lookup degrade if many keys hash to the same bucket (collision chain approaches n)
Duplicate keys overwrite prior entries

### Binary Search Tree

Binary Search Tree (Non-Self-Balancing)
Advantages
Option 2 is natural via in-order traversal (sorted output without extra sorting)
Disadvantages
Performance depends on input order; without self-balancing, worst case degrades to O(n) per insert and search
Prerequisite validation can degrade to O(n^2 * m) in worst case
Higher memory overhead due to node pointers
Duplicates are rejected with a warning
Temporary PrereqRef vector duplicates prerequisite references during load

### Recommendation

Recommendation
Use the hash table for implementation. The program performs repeated lookups during prerequisite validation and during Option 3 queries. A hash table supports these lookups in average O(1) time, keeping the most lookup-intensive operations efficient. The Option 2 requirement is satisfied by sorting extracted keys when needed, and that O(n log n) cost during printing is acceptable given the load and lookup benefits.
