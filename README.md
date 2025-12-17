# ELEC2645 Unit 2 – RC Circuit Analysis CLI (C++)

## Overview
This project is a command-line C++ application that analyses the behaviour of a first-order RC circuit.  
It allows the user to enter R, C, and input voltage values, then perform common RC calculations and simulations using a simple menu interface.
This project demonstrates the application of circuit theory, numerical simulation, and modular C++ programming within an embedded systems style environment.

Main features:
- Enter / edit RC parameters (R, C, Vin)
- Calculate time constant: τ = R·C
- Calculate cutoff frequency: fc = 1 / (2πRC)
- Simulate capacitor charging voltage Vc(t)
- Simulate capacitor discharging voltage Vc(t)
- Export voltage–time simulation data to a CSV file for plotting in Excel

The program is designed with a modular structure:
- `main.c` handles menu display, input validation, and dispatching menu options.
- `funcs.c` contains the RC maths, parameter storage, and menu “handler” functions.
- `funcs.h` declares shared structures/constants and function prototypes.

---

## Files
- `main.c` – menu system / user interaction
- `funcs.c` – RC calculations, input helpers, CSV export
- `funcs.h` – shared header (struct + function prototypes)
- `Makefile` – build commands
- `test.sh` – provided compilation test script (autograder-style check)

---

## Compile and Run

### Compile the program using gcc:
```
gcc main.c funcs.c -o main.out -lm
```
### After successful compilation, run the executable:
```
./main.out