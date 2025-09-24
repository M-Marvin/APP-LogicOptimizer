# Logic Optimiser/Solver #

<img width="1052" height="938" alt="Bild_2025-09-24_203917364" src="https://github.com/user-attachments/assets/42f88859-b4ce-4c44-8710-442027ff79d2" />

This is an simple small executable which takes an truth table from an text file (as tabulator seperated table) and
can find the optimal equations to implement the function described by the table.

Both outputs and inputs can either have the states TRUE FALSE or DONT CARE.

## Input File Format ##
The tables for the program only have two requirements:
- A valid table where each column is sperated by an tabulator and each row sperated by an line feed (LF or CR-LF), no additonal empty lines
- Each cell either has 1/TRUE for logical true, 0/FALSE for logical false or any other value (including an empty string) for don't care.

## Command Line Parameters ##
The programm is invoked with the folowing flags:
-tt [...] the path to the truth table file
-i [...] the number of inputs in the truth table
-o [...] the number of outputs in the truth table
-v verbose mode

Only one, -i or -o have to be specified, if both are specified they have to match with the number of columns in the table.
In verbose mode, additonal graphical representations of the intermediate steps are printed to the console, this might slow down the program significantly.
