// Pre-compiler directive
#ifndef STRING_UTILS_H
#define STRING_UTILS_H


// header files
#include <stdio.h>		// file ops
#include <stdlib.h> 	// dynamic memory operations

// create global constants - across files
typedef enum { False, True } Boolean;
typedef enum { NO_ERR,
			   INCOMPLETE_FILE_ERR,
			   INPUT_BUFFER_OVERRUN_ERR } StringManipCode;


extern const int STD_STR_LEN;
extern const int MAX_STR_LEN;
extern const int SUBSTRING_NOT_FOUND;
extern const int STR_EQ;
extern const char NULL_CHAR;
extern const char SPACE;
extern const char COLON;
extern const char PERIOD;
extern const char SEMICOLON;
extern const char LEFT_PAREN;
extern const char RIGHT_PAREN;
extern const Boolean IGNORE_LEADING_WS;
extern const Boolean ACCEPT_LEADING_WS;

// function prototypes
int getStringLength( char *testStr );
void copyString( char *destination, char *source );
void concatenateString( char *destination, char *source );
int compareString( char *oneStr, char *otherStr );
void getSubString( char *destStr, char *sourceStr,
									int startIndex, int endIndex );
int findSubString( char *testStr, char *searchSubStr );
void setStrToLowerCase( char *destStr, char *sourceStr );
char setCharToLowerCase( char testChar );
int getLineTo( FILE *filePtr, int bufferSize, char stopChar,
                           char *buffer, Boolean omitLeadingWhiteSpace );
Boolean isEndOfFile( FILE *filePtr );
void clearString( char *toClear );


#endif  // STRING_UTILS_H
