// header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "proggen.h"

// main program
int main()
   {
    // initialize program

       // initialize variables
       int procPercentage, numOps, numPrograms;
       int progCtr, opCtr, lineLength = 0;
       char fileName[ STD_STR_LEN ];
       FILE *outFilePtr;
       char opString[ STD_STR_LEN ];
       char tempString[ STD_STR_LEN ];

       // initialize random generator
          // function: srand
       srand( time( NULL ) );

       // show title
          // function: printf
       printf( "Program Meta-Data Creation Program\n\n" );

    // get file name for meta-data file
       // function: printf, scanf
    printf( "Enter file name to use: " );
    scanf( "%s", fileName );

    // get weight of processing
       // function: printf, scanf
    printf( "Enter percentage of processing: " );
    scanf( "%d", &procPercentage );

    // get number of actions
       // function: printf, scanf
    printf( "Enter number of operations per program: " );
    scanf( "%d", &numOps );

    // get number of programs to generate
       // function: printf, scanf
    printf( "Enter number of programs: " );
    scanf( "%d", &numPrograms );

    // open file
       // function: fopen
    outFilePtr = fopen( fileName, "w" );

    // output file description header
       // function: fputs
    fputs( "Start Program Meta-Data Code:\n", outFilePtr );

    // set temporary string with string literal
       // function: strcpy
    strcpy( tempString, "S(start)0; " );

    // output Operating System start
       // function: fputs
    fputs( tempString, outFilePtr );

    // update length, output end of line as needed
       // function: checkLineLength
    lineLength = checkLineLength( tempString, lineLength, outFilePtr );

    // loop across number of programs
    for( progCtr = 0; progCtr < numPrograms; progCtr++ )
       {
        // set temporary string with string literal
           // function: strcpy
        strcpy( tempString, "A(start)0; " );
    
        // update length, output end of line as needed
           // function: checkLineLength
        lineLength = checkLineLength( tempString, lineLength, outFilePtr );

        // show begin of program
           // function: fputs
        fputs( tempString, outFilePtr );

        // loop across number of operations
        for( opCtr = 0; opCtr < numOps; opCtr++ )
           {
            // get new op
            // function: getNewOp
            getNewOp( procPercentage, opString );

            // update length, output end of line as needed
               // function: checkLineLength
            lineLength = checkLineLength( opString, lineLength, outFilePtr );

            // output new op
               // function: fputs
            fputs( opString, outFilePtr );
           }
        // end loop across number of operations

        // set temporary string with string literal
           // function: strcpy
        strcpy( tempString, "A(end)0; " );

        // update length, output end of line as needed
           // function: checkLineLength
        lineLength = checkLineLength( tempString, lineLength, outFilePtr );

        // add end of program
           // function: fputs
        fputs( tempString, outFilePtr );
       }
    // end number of programs loop

    // set temporary string with string literal
       // function: strcpy
    strcpy( tempString, "S(end)0;" );

    // update length, output end of line as needed
       // function: checkLineLength
    lineLength = checkLineLength( tempString, lineLength, outFilePtr );

    // output end of operating system
       // function: fputs
    fputs( tempString, outFilePtr );

    // output extra endline        
       // function: fputs
    fputs( ENDLINE_CHAR, outFilePtr );

    // output file description header
       // function: fputs
    fputs( "End Program Meta-Data Code.\n\n", outFilePtr );

    // close file
       // function: fclose
    fclose( outFilePtr );

    // shut down program

       // return success
       return 0; 
   }

// supporting function implementations

int getRandBetween( int low, int high )
   {
    // initialize range
    int range = high - low + 1;

    // set random result and return
       // function: rand
    return rand() % range + low;    
   }

Boolean getOdds( int oddPercent )
   {
    // initialize odds
       // function: rand
    int randVal = rand() % 100 + 1;

    // check for odds less than/equal to specification
    if( randVal <= oddPercent )
       {
        // return true
        return True;
       }

    // otherwise, if odds not met, return false
    return False;
   }

void getNewOp( int percent, char opStr[] )
   {
    // initialize recursive start index
    int recStartIndex = 0;

    // initialize a control value so the first operation is always input
    static Boolean firstOp = True;
    
    // initialize speedFactor to 1, for processor
        // speedFactor makes I/O devices relatively slower than the processor
        // - although not as relatively slow as they would really be
    int speedFactor = 1;

    // declare other variables
    int numCycles;
    char numStr[ STD_STR_LEN ];

    // check for chance of processing
       // function: getOdds
    if( getOdds( percent ) == True )
       {
        // place a processing action
           // function: strcpy
        strcpy( opStr, "P(run)" );
       }

    // otherwise, assume some kind of I/O
    else
       {
        // check for first I/O operations, or chance of input operation
             // function: getOdds
        if( firstOp || getOdds( INPUT_CHANCE ) == True ) // input process
           {
            // check for odds of hard drive
                // function: getOdds
            if( getOdds( INPUT_CHANCE ) == True ) // toss coin
               {
                // place a hard drive operation
                   // function: strcpy
                strcpy( opStr, "I(hard drive)" );

                // 3x slower than processor
                speedFactor = 3;
               }

            // otherwise, assume keyboard
            else
               {
                // place a keyboard operation
                   // function: strcpy
                strcpy( opStr, "I(keyboard)" );

                // 10x slower than processor
                speedFactor = 10;
               }

            // toggle first operation flag
            firstOp = False;
           }

        // otherwise, assume output operation
        else
           {
            // check for odds of hard drive
                // function: getOdds
            if( getOdds( OUTPUT_CHANCE ) == True ) // toss coin
               {
                // place a hard drive operation
                   // function: strcpy
                strcpy( opStr, "O(hard drive)" );

                // 3x slower than processor
                speedFactor = 3;
               }

            // check for odds of monitor
                // function: getOdds
            else if( getOdds( OUTPUT_CHANCE ) == True )
               {
                // place a monitor operation
                   // function: strcpy
                strcpy( opStr, "O(monitor)" );

                // 2x slower than processor
                speedFactor = 2;
               }

            // otherwise, assume printer operation
            else
               {
                // place a printer operation
                   // function: strcpy
                strcpy( opStr, "O(printer)" );

                // 5x slower than processor
                speedFactor = 5;
               }
           }
       }

    // find random number of I/O cycles
       // function: getRandBetween
    numCycles = getRandBetween( OPTIME_MIN, OPTIME_MAX ) * speedFactor;

    // set number as string
       // function: intToString
    intToString( numCycles, numStr, recStartIndex );

    // append string number to metadata item
       // function: strcat
    strcat( opStr, numStr );

    // append semicolon to metadata item
       // function: strcat
    strcat( opStr, "; " );
   }

int checkLineLength( char str[], int startLen, FILE *fileOutPtr )
   {
    // initialize length of string, as appended
    int newLength = startLen + strlen( str );

    // check length for longer than allowed
    if( newLength > MAX_LINE_LENGTH )
       {
        // send endline character
           // function: fputs
        fputs( ENDLINE_CHAR, fileOutPtr );

        // return reset line length
        return 0;
       }

    // return updated line length
    return newLength;
   }

int intToString( int value, char valStr[], int index )
   {
    // initialize function, variables
    char digit;

    // check for digits remaining
    if( value > 0 )
       {
        // convert digit to character
        digit = (char) ( value % 10 + '0' );

        // call recursive function to get next digt
           // function: intToString
        index = intToString( value / 10, valStr, index + 1 );
       }

    // otherwise, assume no digits remaining
    else
       {
        // end string with NULL_CHAR
        valStr[ index ] = NULL_CHAR;

        // return uncounted character
        return 0;
       }

    // add character to string
    valStr[ index ] = digit;

    // return updated string length
    return index + 1;
   }


