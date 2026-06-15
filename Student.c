#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME      "students.dat"
#define NUM_SUBJECTS  5
#define NAME_LEN      50
#define COURSE_LEN    30

const char *subjectNames[NUM_SUBJECTS] = {
    "Mathematics", "Physics", "Chemistry", "English", "Computer Science"
};

typedef struct {
    int   roll;
    char  name[NAME_LEN];
    char  course[COURSE_LEN];
    float marks[NUM_SUBJECTS];
    float total;
    float percentage;
    char  grade[4];
} Student;

/* ---------- Function Prototypes ---------- */
void insertStudent(void);
void displayStudents(void);
void searchStudent(void);
void updateStudent(void);
void deleteStudent(void);
void calculateResult(Student *s);
void getGrade(float percentage, char *grade);

/* ---------- Main Menu ---------- */
int main() {
    int choice;

    do {
        printf("\n===== STUDENT MANAGEMENT SYSTEM =====\n");
        printf("1. Insert Student Record\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student Record\n");
        printf("5. Delete Student Record\n");
        printf("6. Exit\n");
        printf("======================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');   /* clear invalid input */
            choice = -1;
        }

        switch (choice) {
            case 1: insertStudent();   break;
            case 2: displayStudents(); break;
            case 3: searchStudent();   break;
            case 4: updateStudent();   break;
            case 5: deleteStudent();   break;
            case 6: printf("Exiting... Goodbye!\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 6);

    return 0;
}


void calculateResult(Student *s) {
    int i;
    float sum = 0;

    /* Add up all 5 subject marks */
    for (i = 0; i < NUM_SUBJECTS; i++) sum += s->marks[i];

    s->total = sum;

    /* Max possible marks = NUM_SUBJECTS * 100.
       percentage = (marks obtained / max possible) * 100 */
    s->percentage = (sum / (NUM_SUBJECTS * 100)) * 100;

    /* Use percentage to decide the letter grade */
    getGrade(s->percentage, s->grade);
}

/* ---------- Assign grade based on percentage ----------
   grade is a char array (string) like "A+", "B", "F".
   We check from highest to lowest threshold and use the FIRST
   condition that matches, then copy that grade string into the
   caller's grade array using strcpy(). */

void getGrade(float percentage, char *grade) {
    if (percentage >= 90)      strcpy(grade, "A+");
    else if (percentage >= 80) strcpy(grade, "A");
    else if (percentage >= 70) strcpy(grade, "B+");
    else if (percentage >= 60) strcpy(grade, "B");
    else if (percentage >= 50) strcpy(grade, "C");
    else if (percentage >= 40) strcpy(grade, "D");
    else                        strcpy(grade, "F");
}

/* ---------- Insert a new student record ---------- */
void insertStudent(void) {
    Student s;
    int i;

    printf("\n--- Add New Student ---\n");
    printf("Enter Roll Number: ");
    scanf("%d", &s.roll);

    printf("Enter Name  : ");
    scanf(" %49[^\n]", s.name);

    printf("Enter Course: ");
    scanf(" %29[^\n]", s.course);

    printf("\nEnter marks for %d subjects (out of 100):\n", NUM_SUBJECTS);
    for (i = 0; i < NUM_SUBJECTS; i++) {
        printf("%s/t: ", subjectNames[i]);
        scanf("%f", &s.marks[i]);
    }

    calculateResult(&s);

    /* "ab" = append in binary mode -> adds this record to the
       end of the file without touching existing records. */
    FILE *fp = fopen(FILENAME, "ab");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);

    printf("\nStudent record added successfully!\n");
    printf("Total: %.2f | Percentage: %.2f%% | Grade: %s\n",
           s.total, s.percentage, s.grade);
}

/* ---------- Display all student records ---------- */
void displayStudents(void) {
    Student s;
    int found = 0;

    /* "rb" = read in binary mode */
    FILE *fp = fopen(FILENAME, "rb");

    if (fp == NULL) {
        printf("\nNo records found!\n");
        return;
    }

    printf("\n%s/t %s %s/t %s %s/t %s\n",
           "Roll", "Name", "Course", "Total", "Percent", "Grade");
    printf("--------------------------------------------------------------------\n");

    /* fread() reads one Student record at a time.
       It returns 1 on success and 0 when there's nothing left to read,
  so the loop automatically stops at the end of the file. */

    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        printf("%-6d %-20s %-15s %-8.2f %-10.2f %-6s\n",
               s.roll, s.name, s.course, s.total, s.percentage, s.grade);
        found = 1;
    }

    if (!found) printf("No student records available.\n");

    fclose(fp);
}

/* ---------- Search a student by roll number ---------- */

void searchStudent(void) {
    Student s;
    int roll, found = 0, i;

    printf("\nEnter Roll Number to search: ");
    scanf("%d", &roll);

    FILE *fp = fopen(FILENAME, "rb");
    if (fp == NULL) {
        printf("\nNo records found!\n");
        return;
    }

    /* Go through the file record by record until we find a match
       or reach the end of the file. */
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) {
            found = 1;
            printf("\n--- Student Found ---\n");
            printf("Roll No   : %d\n", s.roll);
            printf("Name      : %s\n", s.name);
            printf("Course    : %s\n", s.course);
            for (i = 0; i < NUM_SUBJECTS; i++)
                printf("%-18s: %.2f\n", subjectNames[i], s.marks[i]);
            printf("Total     : %.2f\n", s.total);
            printf("Percentage: %.2f%%\n", s.percentage);
            printf("Grade     : %s\n", s.grade);
            break; /* stop searching once found */
        }
    }

    if (!found) printf("\nStudent with Roll Number %d not found.\n", roll);

    fclose(fp);
}

/* ---------- Update an existing student record ----------
   "rb+" opens the file for BOTH reading and writing in binary
   mode, without erasing its contents (unlike "wb"). This lets us read a record, then write a new one in its exact place. */

void updateStudent(void) {
    Student s;
    int roll, found = 0, i;

    printf("\nEnter Roll Number to update: ");
    scanf("%d", &roll);

    FILE *fp = fopen(FILENAME, "rb+");
    if (fp == NULL) {
        printf("\nNo records found!\n");
        return;
    }

    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) {
            found = 1;
            printf("\nCurrent details:\n");
            printf("Name  : %s\n", s.name);
            printf("Course: %s\n", s.course);
            for (i = 0; i < NUM_SUBJECTS; i++)
                printf("%s/t: %.2f\n", subjectNames[i], s.marks[i]);

            printf("\nEnter new Name  : ");
            scanf(" %49[^\n]", s.name);

            printf("Enter new Course: ");
            scanf(" %29[^\n]", s.course);

            printf("\nEnter new marks:\n");
            for (i = 0; i < NUM_SUBJECTS; i++) {
                printf("%-20s: ", subjectNames[i]);
                scanf("%f", &s.marks[i]);
            }

            calculateResult(&s);

            /* --- The "rewind and overwrite" trick ---
               After fread() succeeds, the file pointer has already
               moved this record to the start of the next one.
               To overwrite the SAME record with our edited copy, we
               move the pointer BACK by exactly one record's size,
               then fwrite() the updated struct in its place.
               SEEK_CUR means "relative to current position", and a
               negative offset means "move backwards". */


            fseek(fp, (long)sizeof(Student), SEEK_CUR);
            fwrite(&s, sizeof(Student), 1, fp);

            printf("\nStudent record updated successfully!\n");
            printf("New Total: %.2f | Percentage: %.2f%% | Grade: %s\n",
                   s.total, s.percentage, s.grade);
            break;
        }
    }

    if (!found) printf("\nStudent with Roll Number %d not found.\n", roll);

    fclose(fp);
}

/* ---------- Delete a student record ----------
   C has no direct "delete this record from the middle of a file"
   operation, so the standard, beginner-friendly trick is:
     1. Open the original file for reading.
     2. Create a brand-new temporary file for writing.
     3. Copy every record EXCEPT the one to delete into temp.dat.
     4. Delete the original file and rename temp.dat to take its place.
   The net effect is that the matching record simply disappears. */

void deleteStudent(void) {
    Student s;
    int roll, found = 0;

    printf("\nEnter Roll Number to delete: ");
    scanf("%d", &roll);

    FILE *fp   = fopen(FILENAME, "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (fp == NULL || temp == NULL) {
        printf("\nError opening files!\n");
        if (fp)   fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) {
            found = 1;
            continue; 
/*we cannot write this record to temp.dat -> it's "deleted" */

        }
        fwrite(&s, sizeof(Student), 1, temp); /* keep every other record */
    }

    fclose(fp);
    fclose(temp);

    /* Replace the old file with the filtered one */

    remove(FILENAME);         
    /* delete original students.dat  */

    rename("temp.dat", FILENAME); 
/* temp.dat becomes students.dat */

    if (found) printf("\nStudent record deleted successfully!\n");
    else        printf("\nStudent with Roll Number %d not found.\n", roll);
}