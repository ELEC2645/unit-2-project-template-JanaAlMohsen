// funcs.c implements the functionality behind each menu choice

#include <unistd.h> // getcwd
#include <limits.h> // PATH_MAX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "funcs.h"

// Stores current parameters for the whole app
static RCParams g_params = {0};

// Small input helpers
// They provide safe user input handling for doubles
// The menu system in main.c reads integer choices, but funcs.c reads floating point values  

// Reads a full line from stdin and attempts to convert it to a double using strtod and rejects invalid inputs
static int read_double_line(const char *prompt, double *out) { // prompt = message shown to user, out = pointer where the parsed double is stored
    char buf[128];
    char *endptr;

    printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) return 0;

    // Strip newline
    buf[strcspn(buf, "\r\n")] = '\0';

    // Convert text to double
    *out = strtod(buf, &endptr);

// If no conversion happened or leftover junk (besides spaces)
    if (endptr == buf) return 0;
    while (*endptr == ' ' || *endptr == '\t') endptr++;
    if (*endptr != '\0') return 0;

    return 1;
}

// Checks whether g_params has been set 
static int have_valid_params(void) {
    return (g_params.is_set && g_params.R > 0.0 && g_params.C > 0.0);
}

// Print the currently stored RC parameters in a consistent format
static void print_current_params(void) {
    printf("Current parameters: R=%.6g ohms, C=%.6g F, Vin=%.6g V\n",
           g_params.R, g_params.C, g_params.Vin);
}

// RC maths - they implement the equations used by the RC analysis tools

// Calculate the RC time constant tau
static double compute_tau(double R, double C) {
    return R * C;
}

// Calculates the capacitor voltage during charging 
static double vc_charging(double Vin, double t, double tau) {
    if (tau <= 0.0) return NAN;
    return Vin * (1.0 - exp(-t / tau));
}

// Calculates the capacitor voltage during discharging 
static double vc_discharging(double V0, double t, double tau) {
    if (tau <= 0.0) return NAN;
    return V0 * exp(-t / tau);
}

// Calculates the cutoff frequency of an RC low pass filter
static double cutoff_frequency(double R, double C) {
    if (R <= 0.0 || C <= 0.0) return NAN;
    return 1.0 / (2.0 * PI * R * C);
}

// Allows user to enter or update RC circuit parameters
void menu_item_1(void) {
    printf("\n>> Enter / Edit RC Parameters\n");

    double R, C, Vin;

    if (!read_double_line("Enter R (ohms): ", &R)) { puts("Invalid R."); return; }
    if (!read_double_line("Enter C (farads): ", &C)) { puts("Invalid C."); return; }
    if (!read_double_line("Enter Vin (volts): ", &Vin)) { puts("Invalid Vin."); return; }

    // Basic physical validation
    if (R <= 0.0 || C <= 0.0) {
        puts("R and C must be > 0.");
        return;
    }

    // Save parameters for other menu functions
    g_params.R = R;
    g_params.C = C;
    g_params.Vin = Vin;
    g_params.is_set = 1;

    puts("Parameters saved.");
    printf("R=%.6g ohms, C=%.6g F, Vin=%.6g V\n", g_params.R, g_params.C, g_params.Vin);
}

// Computes and prints time constant (tau) and cutoff frequency
// Requires parameters to have been set in menu_item_1()
void menu_item_2(void) {
    printf("\n>> Compute Time Constant and Cutoff Frequency\n");

    if (!have_valid_params()) {
        puts("Set valid R and C first using Menu 1.");
        return;
    }

    print_current_params();

    double tau = compute_tau(g_params.R, g_params.C);
    double fc  = cutoff_frequency(g_params.R, g_params.C);

    printf("tau = R*C = %.6g s\n", tau);
    printf("fc = 1/(2*pi*R*C) = %.6g Hz\n", fc);
}

// Simulates capacitor charging by calculating Vc(t) at a user specified time t 
// Uses Vin from stored parameters
void menu_item_3(void) {
    printf("\n>> Charging Simulation (Vc at time t)\n");

    if (!have_valid_params()) {
        puts("Set valid R and C first using Menu 1.");
        return;
    }

    print_current_params();


    double t;
    if (!read_double_line("Enter time t (seconds): ", &t)) { puts("Invalid t."); return; }
    if (t < 0.0) { puts("t must be >= 0."); return; }

    double tau = compute_tau(g_params.R, g_params.C);
    double vc  = vc_charging(g_params.Vin, t, tau);

    // handle invalid calculations
    if (isnan(vc)) {
        puts("Calculation invalid (check parameters).");
        return;
    }

    printf("Vc(t) = %.6g V (charging, Vin=%.6g)\n", vc, g_params.Vin);
}

// Simulates capacitor discharging by calculating Vc(t) at a user specified time (t) 
// User provides initial voltage V0
void menu_item_4(void) {
    printf("\n>> Discharging Simulation (Vc at time t)\n");

    if (!have_valid_params()) {
        puts("Set valid R and C first using Menu 1.");
        return;
    }

    print_current_params();


    double V0, t;
    if (!read_double_line("Enter initial capacitor voltage V0 (V): ", &V0)) { puts("Invalid V0."); return; }
    if (!read_double_line("Enter time t (seconds): ", &t)) { puts("Invalid t."); return; }
    if (t < 0.0) { puts("t must be >= 0."); return; }

    double tau = compute_tau(g_params.R, g_params.C);
    double vc  = vc_discharging(V0, t, tau);

    // Handles invalid calculations
    if (isnan(vc)) {
        puts("Calculation invalid (check parameters).");
        return;
    }

    printf("Vc(t) = %.6g V (discharging from V0=%.6g)\n", vc, V0);
}

// 
void menu_item_5(void) {
    printf("\n>> Export CSV (time vs voltage)\n");

    if (!have_valid_params()) {
        puts("Set valid R and C first using Menu 1.");
        return;
    }

    print_current_params();


    // Choose simulation mode
    double mode;
    if (!read_double_line("Enter mode (1 = charging, 2 = discharging): ", &mode)) {
        puts("Invalid mode.");
        return;
    }
    if (!(mode == 1.0 || mode == 2.0)) {
        puts("Mode must be 1 or 2.");
        return;
    }

    // Time range and step
    double t0, t1, dt;
    if (!read_double_line("Start time t0 (s): ", &t0)) { puts("Invalid t0."); return; }
    if (!read_double_line("End time t1 (s): ", &t1))   { puts("Invalid t1."); return; }
    if (!read_double_line("Time step dt (s): ", &dt))  { puts("Invalid dt."); return; }

    if (t0 < 0.0 || t1 < 0.0 || dt <= 0.0 || t1 < t0) {
        puts("Check that t0>=0, t1>=0, dt>0, and t1>=t0.");
        return;
    }

    // Discharge requires an initial capacitor voltage V0 
    double V0 = 0.0;
    if (mode == 2.0) {
        if (!read_double_line("Initial capacitor voltage V0 (V): ", &V0)) {
            puts("Invalid V0.");
            return;
        }
    }

    // Filename 
    char filename[128];
    printf("Enter CSV filename (e.g., rc_output.csv): ");
    if (!fgets(filename, sizeof(filename), stdin)) {
        puts("Invalid filename.");
        return;
    }
    filename[strcspn(filename, "\r\n")] = '\0';
    if (filename[0] == '\0') {
        puts("Filename cannot be empty.");
        return;
    }

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Could not create CSV file");
        return;
    }

    // Write header
    fprintf(fp, "time,voltage\n");

    double tau = compute_tau(g_params.R, g_params.C);

    long samples = 0; // Variable to count how many data rows are written to the CSV

    // Generate and write data points 
    for (double t = t0; t <= t1 + 1e-12; t += dt) {
        double v;

        // Select the appropriate RC equation depending on the mode
        if (mode == 1.0) {
            v = vc_charging(g_params.Vin, t, tau);
        } else {
            v = vc_discharging(V0, t, tau);
        }

        // Write a single row to the CSV file in the format: time, voltage
        fprintf(fp, "%.10g,%.10g\n", t, v);

        samples++; // Increment the sample counter to record that one data row has been successfully written

    }

    fclose(fp); // Close the CSV file to ensure all buffered data is written

    puts("CSV exported successfully.");
    printf("Wrote %ld samples to %s\n", samples, filename); // Provide confirmation to the user that the export succeeded

    char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Saved in directory: %s\n", cwd);
        } else {
            puts("Saved in current working directory (path unavailable).");
        }



    puts("Tip: Open the CSV in Excel and plot voltage vs time.");
}
