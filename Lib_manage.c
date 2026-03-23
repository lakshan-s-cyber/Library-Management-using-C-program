#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_BOOKS 100
#define MAX_STUDENTS 50
#define FINE_PER_DAY 10.0 // Fine amount
#define BORROW_DAYS 0.001 // Books are due in 15 days
#define PWD 129911


//--- Data storage ----

struct Student { // Student details
    int id;
    char name[50];
    int is_active; // 1 if valid, 0 if deleted
};


struct Book {     // Book details to store and retrieve
    int id;
    char title[50];
    char author[50];
    int pages;
    float price;
    int is_borrowed;     // 0 = Available, 1 = Borrowed
    int borrowed_by_uid; // ID of student who has the book
    time_t due_date;     // Timestamp for when it is due
};

// --- Global Structure Variables ---

struct Book library[MAX_BOOKS];
struct Student students[MAX_STUDENTS];
int book_count = 0;
int student_count = 0;

// --- Helper Functions ---



// Get current date as string for display
void printDate(time_t date) {
    struct tm *tm_info = localtime(&date);
    char buffer[26];
    strftime(buffer, 26, "%Y-%m-%d %H:%M", tm_info);
    printf("%s", buffer);
}

// Find book index by ID
int findBookIndex(int id) {
    for (int i = 0; i < book_count; i++) {
        if (library[i].id == id) return i;
    }
    return -1;
}

// Find student index by ID
int findStudentIndex(int id) {
    for (int i = 0; i < student_count; i++) {
        if (students[i].id == id) return i;
    }
    return -1;
}

// --- Core Features ---

void addBook() {
    if (book_count >= MAX_BOOKS) {
        printf("\nLibrary is full!\n");
        return;
    }

    library[book_count].id = book_count + 1001; // ID starting 1001

    printf("\nEnter Book Title: ");
    scanf(" %[^\n]s", library[book_count].title); // Reads strings with whitespaces
    printf("Enter Author Name: ");
    scanf(" %[^\n]s", library[book_count].author);
    printf("Enter Pages: ");
    scanf("%d", &library[book_count].pages);
    printf("Enter Price: ");
    scanf("%f", &library[book_count].price);

    library[book_count].is_borrowed = 0;
    library[book_count].borrowed_by_uid = -1; // Marks no student v=borrowed it

    printf("Book Added Successfully! Book ID is: %d\n", library[book_count].id);
    book_count++;
}

void registerStudent() {
    if (student_count >= MAX_STUDENTS) {
        printf("\nCannot register more students.\n");
        return;
    }

    students[student_count].id = student_count + 1; // ID starting 1
    printf("\nEnter Student Name: ");
    scanf(" %[^\n]s", students[student_count].name);
    students[student_count].is_active = 1;

    printf("Student Registered! Assigned Student ID: %d\n", students[student_count].id);
    student_count++;
}

void listBooks() {
    printf("\n--- Library Catalog ---\n");
    printf("%-10s %-20s %-20s %-10s\n", "ID", "Title", "Author", "Status");  //Correctly spaced so left justify by factors of 10
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < book_count; i++) {
        printf("%-10d %-20s %-20s %-10s\n",
            library[i].id,
            library[i].title,
            library[i].author,
            library[i].is_borrowed ? "BORROWED" : "AVAILABLE"); //is_borrowed is 0 by default
    }
}

void borrowBook() {
    int b_id, s_id;
    printf("\n--- Borrow a Book ---\n");

    // 1. Authorize Student
    printf("Enter Student ID: ");
    scanf("%d", &s_id);
    int s_idx = findStudentIndex(s_id);
    if (s_idx == -1) {
        printf("Error: Student ID not found. Please register first.\n");
        return;
    }

    // 2. Find Book
    printf("Enter Book ID to borrow: ");
    scanf("%d", &b_id);
    int b_idx = findBookIndex(b_id);

    if (b_idx == -1) {
        printf("Error: Book not found.\n");
        return;
    }

    if (library[b_idx].is_borrowed) {
        printf("Error: Book is already borrowed.\n");
        return;
    }

    // 3. Process Transaction by changing status of borrowing
    library[b_idx].is_borrowed = 1;
    library[b_idx].borrowed_by_uid = s_id;

    // Calculate Due Date (Current time + 15 days in seconds(time.h constant)
    time_t now = time(NULL);        //NULL gets current time , time_t is fixed within time.h
    library[b_idx].due_date = now + (BORROW_DAYS * 24 * 3600);

    printf("Success! Book issued to %s.\n", students[s_idx].name);
    printf("Due Date: ");
    printDate(library[b_idx].due_date);
    printf("\n");
}

void returnBook() {
    int b_id;
    printf("\n--- Return a Book ---\n");
    printf("Enter Book ID: ");
    scanf("%d", &b_id);

    int b_idx = findBookIndex(b_id);
    if (b_idx == -1 || !library[b_idx].is_borrowed) {
        printf("Error: Invalid Book ID or Book was not borrowed.\n");
        return;
    }

    // Check for Fines
    time_t now = time(NULL);
    double diff_seconds = difftime(now, library[b_idx].due_date); // check difference between current time and due date time (seconds unit)

    if (diff_seconds > 0) {
        // Calculate days late (rounding up)
        int days_late = (int)(diff_seconds / (24 * 3600)) + 1; //Fine by days (float removed by type casting int)
        float fine = days_late * FINE_PER_DAY;
        printf("ALERT: Book is returned late by %d day(s).\n", days_late);
        printf("Total Fine to pay: %.2f\n", fine);
    } else {
        printf("Book returned on time. No fine.\n");
    }

    // Reset Book Status of borrowing to ) and uid of student is freed
    library[b_idx].is_borrowed = 0;
    library[b_idx].borrowed_by_uid = -1;
    printf("Book marked as returned.\n");
}


// List all active studnets who were registered within the system

void listStudents() {
    printf("\n--- Registered Students ---\n");
    printf("%-10s %-20s %-10s\n", "ID", "Name", "Status");
    printf("--------------------------------------------\n");
    for (int i = 0; i < student_count; i++) {
        if (students[i].is_active) {
            printf("%-10d %-20s %-10s\n",
                students[i].id,
                students[i].name,
                "Active");
        }
    }
}

// show available books with extra information for admin
void show_avail_books(){

    printf("\n%-5s %-20s %-15s %-10s %-10s %-10s %-5s\n","ID","Title","Author","Pages","Price","Status","Student_id");
    printf("-----------------------------------------------------------------------------------");

    for(int b_idx=0;b_idx< MAX_BOOKS;b_idx++){

        if(library[b_idx].id==0) continue;
        const char *s = library[b_idx].is_borrowed ? "Borrowed" : "Available";
        int status = library[b_idx].borrowed_by_uid;

        if(status!=-1)
            printf("\n%-5d %-20s %-15s %-10d %-10.2f %-10s %-5d\n %",library[b_idx].id,library[b_idx].title,
                library[b_idx].author,library[b_idx].pages,library[b_idx].price,s,library[b_idx].borrowed_by_uid);
        else
            printf("\n%-5d %-20s %-15s %-10d %-10.2f %-10s %-5s\n %",library[b_idx].id,library[b_idx].title,
                library[b_idx].author,library[b_idx].pages,library[b_idx].price,s,"None");
    }
}






int main() {
    int input = 0;

    printf("%zu",sizeof(time_t));

    // Pre-register a dummy student
    students[0].id = 1;
    strcpy(students[0].name, "Lakzan");
    students[0].is_active = 1;
    student_count = 1;

    // classify into admin account and student account , admin account pwd FIXED : Comment by Lakshan

    int ch;
    while(ch!=3){
    printf("\n\n******** WELCOME TO E-LIBRARY ********\n");
    printf("1. Admin\n");
    printf("2. Student\n");
    printf("3. Exit\n");
    printf("Enter personnel: ");
    scanf("%d", &ch);

    if (ch == 1) {
        int admin_pwd;
        printf("Enter Admin password: \n");
        scanf("%d", &admin_pwd);              // <-- FIXED

        if (admin_pwd != PWD) {
            printf("Invalid credentials\n");
            return 0;
        }

        while (input != 5) {
            printf("\n\n******** WELCOME TO E-LIBRARY, ADMIN ********\n");
            printf("1. Add Book\n");
            printf("2. List All Books\n");
            printf("3. Register New Student\n");
            printf("4. List All Students\n");
            printf("5. Exit\n");
            printf("Select Option: ");
            scanf("%d", &input);

            switch (input) {
                case 1: addBook(); break;
                case 2: show_avail_books(); break;
                case 3: registerStudent(); break;
                case 4: listStudents(); break;
                case 5: printf("Exiting...\n"); break;
                default: printf("Invalid Option!\n");
            }
        }
    } else if (ch == 2) {
        input = 0;
        while (input != 4) {
            printf("\n\n******** WELCOME TO E-LIBRARY, STUDENT ********\n");
            printf("1. Show Available Books\n");
            printf("2. Borrow Book (Auth Required)\n");
            printf("3. Return Book & Check Fines\n");
            printf("4. Exit\n");
            printf("Select Option: ");
            scanf("%d", &input);

            switch (input) {
                case 1: listBooks();break;
                case 2: borrowBook(); break;
                case 3: returnBook(); break;
                case 4: printf("Exiting...\n"); break;
                default: printf("Invalid Option!\n");
            }
        }
    }
    else if( ch==3) printf("Exiting....\n");

    else {
        printf("Invalid personnel selection.\n");
    }
    }

    return 0;
}

