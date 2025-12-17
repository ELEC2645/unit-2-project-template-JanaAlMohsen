// ELEC2645 Unit 2 Project Template
// Command Line Application Menu Handling Code

#include <stdio.h> // Input/output functions
#include <stdlib.h> // General utilities (exit, strtol)
#include <string.h> // String handling 
#include <ctype.h> // Character classification (isdigit)
#include <math.h> // Math functions/constants
#include "funcs.h" // User defied functions for menu items


// Functions prototypes 
static void main_menu(void);            // Runs in the main loop 
static void print_main_menu(void);      // Output the main menu description 
static int  get_user_input(void);       // Get a valid integer menu choice 
static void select_menu_item(int input);// Run code based on user's choice 
static void go_back_to_main(void);      // Wait for 'b'/'B' to continue 
static int  is_integer(const char *s);  // Validate integer string 



int main(void)
{
    // This will run forever until we call exit(0) in select_menu_item()
    for(;;) {
        main_menu();
    }
    // This line is never reached but included for completeness
    return 0;
}

// Displays the menu, retrieves user input, and processes the selected menu option
static void main_menu(void)
{
    print_main_menu();
    {
        int input = get_user_input();
        select_menu_item(input);
    }
}

// Prompts the user for a menu selection and validates the input 
static int get_user_input(void)
{
    enum { MENU_ITEMS = 6 };   // 1..5 = items, 6 = Exit 
    char buf[128];
    int valid_input = 0;
    int value = 0;

    do {
        printf("\nSelect item: ");
        if (!fgets(buf, sizeof(buf), stdin)) { // Reads input safely as a string
            puts("\nInput error. Exiting."); // EOF or error; bail out gracefully
            exit(1);
        }

        // strip trailing newline
        buf[strcspn(buf, "\r\n")] = '\0';

        // Validate input as integer 
        if (!is_integer(buf)) {
            printf("Enter an integer!\n");
            valid_input = 0;
        } else {
            value = (int)strtol(buf, NULL, 10);

            // Check if integer is within menu bounds
            if (value >= 1 && value <= MENU_ITEMS) {
                valid_input = 1;
            } else {
                printf("Invalid menu item!\n");
                valid_input = 0;
            }
        }
    } while (!valid_input);

    return value;
}


// Excecutes the appropriate call action based on user menu choice
// Calls functions defined in funcs.c
static void select_menu_item(int input)
{
    switch (input) {
        case 1:
            menu_item_1();
            go_back_to_main();
            break;
        case 2:
            menu_item_2();
            go_back_to_main();
            break;
        case 3:
            menu_item_3();
            go_back_to_main();
            break;
        case 4:
            menu_item_4();
            go_back_to_main();
            break;
        case 5:
            menu_item_5();
            go_back_to_main();
            break;
        default:
            printf("Bye bye!\n");
            exit(0);
    }
}

// Displays the main menu options to the user in a clear and well formatted way
static void print_main_menu(void)
{
    printf("\n-------------------- Main menu ----------------------\n");
    printf("\n"
           "\t\t\t\t\t\t\n"
           "\t1. Enter/Edit RC parameters\n"
           "\t2. Time Constant (tau) & Cutoff Frequency\n"
           "\t3. Charging Voltage at Time (Vc(t))\n"
           "\t4. Discharging Voltage at Time (Vc(t))\n"
           "\t5. Export CSV (time vs voltage)\n"
           "\t6. Exit\n"
           "\t\t\t\t\t\t\n");
    printf("------------------------------------------------------\n");
}

// pauses excecution after a menu action is called until the user enters 'B' or 'b' 
static void go_back_to_main(void)
{
    char buf[64];
    do {
        printf("\nEnter 'b' or 'B' to go back to main menu: ");
        if (!fgets(buf, sizeof(buf), stdin)) {
            puts("\nInput error. Exiting.");
            exit(1);
        }
        buf[strcspn(buf, "\r\n")] = '\0'; /* strip newline */
    } while (!(buf[0] == 'b' || buf[0] == 'B') || buf[1] != '\0');
}

// Return 1 if s is an optional [+/-] followed by one-or-more digits, else 0
static int is_integer(const char *s)
{
    if (!s || !*s) return 0;

    // optional sign 
    if (*s == '+' || *s == '-') s++;

    // Must have at least one digit 
    if (!isdigit((unsigned char)*s)) return 0;

    // All remaining characters must be digits
    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}