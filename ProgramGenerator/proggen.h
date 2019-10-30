// Header file for proggen.c

// Precompiler directive, to eliminate multiple compiles of h file
#ifndef PROGGEN_H
#define PROGGEN_H

// global constants

const int STD_STR_LEN = 45;
const int MAX_LINE_LENGTH = 55;
const int INPUT_CHANCE = 50;
const int OUTPUT_CHANCE = 33;
const int OPTIME_MAX = 15;
const int OPTIME_MIN = 5;
const char NULL_CHAR = '\0';
const char ENDLINE_CHAR[] = "\n";

typedef enum { False, True } Boolean;

// function prototypes

void getNewOp( int percent, char opStr[] );
Boolean getOdds( int oddPercent );
int getRandBetween( int low, int high );
int checkLineLength( char str[], int startLen, FILE *fileOutPtr );
int intToString( int value, char valStr[], int index );

#endif // PROGGEN_H


