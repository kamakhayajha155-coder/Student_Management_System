# 🎓 Student Management System (C)
A console-based Student Management System built in C using file handling — supports inserting, displaying, searching, updating, and deleting student records with automatic grade calculation.

## Features
- **Insert** a new student record (roll number, name, course, marks for 5 subjects)
- **Display** all student records in a formatted table
- **Search** for a student by roll number
- **Update** an existing student's details and marks
- **Delete** a student record
- Automatic calculation of **total marks, percentage, and grade** (A+ to F)
- Data persists across runs using **binary file storage**

## Tech Stack
- **Language:** C
- **Concepts used:** structures, file handling (`fopen`, `fread`, `fwrite`, `fseek`), loops, conditionals

## How to Run
```bash
gcc student_management_basic.c -o student
./student
