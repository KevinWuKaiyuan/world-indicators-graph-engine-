# ECE250 Project 5 — Relational Graph of Countries

## Overview
This project constructs a graph of countries where nodes represent countries and edges represent shared statistical relationships derived from time-series data. It extends earlier ECE250 projects involving binary search trees and hash tables and is designed to run under Linux with strict input/output formatting for automated testing.

## Core Concepts

### Graph Model
- **Node**: A country (identified by country code and name).
- **Edge**: Exists between two countries if they share at least one relationship.
- **Relationship tuple**:
  ```
  (Series_Code, Threshold, Relation)
  ```
  - `Series_Code`: World Bank time-series identifier  
  - `Threshold`: numeric value  
  - `Relation`: `greater`, `less`, or `equal`  

Equality comparisons use a tolerance of **1e-3**.  
`greater` and `less` are strict inequalities.

Each edge stores a **set of unique relationship tuples**.

## Supported Commands

### LOAD filename
Loads the dataset.
Output:
```
success
```

### INITIALIZE
Initializes a graph with all countries as nodes and no edges.
Output:
```
success
```

### UPDATE_EDGES Series_Code threshold relation
Updates graph edges based on the given relationship.
Output:
- `success` if at least one new relationship is added
- `failure` otherwise

### ADJACENT Country_Code
Lists all adjacent countries by name.
Output:
- Space-separated country names
- `none` if no adjacent countries
- `failure` if the country does not exist

### PATH Country_Code1 Country_Code2
Determines if a path exists between two countries.
Output:
- `true` or `false`

### RELATIONSHIPS Country_Code1 Country_Code2
Lists all relationship tuples on the edge between two countries.
Output:
```
(Series_Code Threshold Relation) ...
```
`none` if no edge exists.

### EXIT
Terminates the program. No output.

## Compilation
```bash
g++ -std=c++17 -Wall -Wextra -O2 *.cpp -o project5
```

## Execution
```bash
./project5 < input.txt
```

## Runtime Analysis
The project includes a written analysis of the worst-case runtime of the `ADJACENT` command, beginning with:
```
The runtime for my ADJACENT command is O(...)
```
and followed by a formal justification.

## Assumptions
- LOAD is always the first command
- Countries are not added after loading
- INITIALIZE is always called at least once
- Only one edge may exist between two countries

## Author
Kevin  
Electrical Engineering — University of Waterloo  
ECE250
