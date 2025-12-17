// Header file for the RC circuit analysis application

#ifndef FUNCS_H
#define FUNCS_H

// defining PI
#ifndef PI
#define PI 3.14159265358979323846
#endif

// Stores the parameters of the RC circuit entered by the user
typedef struct {
    double R;
    double C;
    double Vin;
    int is_set; // Flag indicating whether parameters are valid
} RCParams;

// These functions are implemented in funcs.c and are called from main.c based on the user's menu selection
void menu_item_1(void);
void menu_item_2(void);
void menu_item_3(void);
void menu_item_4(void);
void menu_item_5(void);


#endif