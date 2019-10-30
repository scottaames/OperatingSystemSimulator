// header files
#include "MetaDataAccess.h"

/*
Function Name:  getOpCodes
Algorithm:      opens file, acquires op code data, returns pointer
                to head of linked list
Precondition:   for correct operation, file is avilable, is formatted correctly,
                and has all correctly formed op codes
Postcondition:  in correct operation, returns pointer to head of
                op code linked list
Exceptions:     correctly and appropriately (without program failure)
                responds to and reports file access failure,
                incorrectly formatted lead or end descriptors,
                incorrectly formatted prompt, incorrect op code letter,
                incorrect op code name, op code value out of range,
                and incomplete file conditions
Notes: none
 */
int getOpCodes( char *fileName, OpCodeType **opCodeDataHead )
{
    // initialize function/variables

        // intialize read only constant
        const char READ_ONLY_FLAG[] = "r";

        // initialize start and end counts for balanced app operations
        int startCount = 0, endCount = 0;

        // intialize local head pointer to null
        OpCodeType *localHeadPtr = NULL;

        // intialize variables
        int accessResult;
        char dataBuffer[ MAX_STR_LEN ];
        OpCodeType *newNodePtr;
        FILE *fileAccessPtr;

    // intialize op code data pointer in case of return error
    *opCodeDataHead = NULL;

    // open file for reading
        // function: fopen
    fileAccessPtr = fopen( fileName, READ_ONLY_FLAG );

    // check for file open failure
    if( fileAccessPtr == NULL )
    {
        // return file access error
        return MD_FILE_ACCESS_ERR;
    }

    // check first line for correct leader
        // function: getLineTo, compareString
    if( getLineTo( fileAccessPtr, MAX_STR_LEN, COLON,
                        dataBuffer, IGNORE_LEADING_WS ) != NO_ERR
    || compareString( dataBuffer, "Start Program Meta-Data Code" ) != STR_EQ )
    {
        // close file
            // func: fclose
        fclose( fileAccessPtr );

        // return corrupt descriptor error
        return MD_CORRUPT_DESCRIPTOR_ERR;
    }

    // allocate memory for the temporary data structure
        // function: malloc
    newNodePtr = ( OpCodeType * ) malloc( sizeof( OpCodeType ) );

    // get the first op command
        // functoin: getOpCommand
    accessResult = getOpCommand( fileAccessPtr, newNodePtr );

    // get start and end counts for later comparison
        // func: updateStartCount, updateEndCount
    startCount = updateStartCount( startCount, newNodePtr->opName );
    endCount = updateEndCount( endCount, newNodePtr->opName );

    // check for failure of first complete op command
    if( accessResult != COMPLETE_OPCMD_FOUND_MSG )
    {
        // close file
            // func: fclose
        fclose( fileAccessPtr );

        // clear data from the structure list
            // func: clearMetaDataList
        *opCodeDataHead = clearMetaDataList( localHeadPtr );

        // free temp structure memory
            // func: free
        free( newNodePtr );

        // return result of operation
        return accessResult;
    }
    // loop across all remaining op commands
    //  (while complete op commands are found)

    while( accessResult == COMPLETE_OPCMD_FOUND_MSG )
    {
        // add the new op command to the linked list
            // func: addNode
        localHeadPtr = addNode( localHeadPtr, newNodePtr );

        // get a new op command
            // func: getOpCommand
        accessResult = getOpCommand( fileAccessPtr, newNodePtr );

        // update start and end counts for later comparison
            // func: updateStartCount, updateEndCount
        startCount = updateStartCount( startCount, newNodePtr->opName );
        endCount = updateEndCount( endCount, newNodePtr->opName );
    }

    // after loop completion, check for last op command found
    if( accessResult == LAST_OPCMD_FOUND_MSG )
    {
        // check for start and end op code counts equal
        if( startCount == endCount )
        {
            // add the last node to the linked list
                // func: addNode
            localHeadPtr = addNode( localHeadPtr, newNodePtr );

            // set access result to no error for later operation
            accessResult = NO_ERR;

            // check last line for incorrect end descriptor
                // func: getLineTo, compareString
            if( getLineTo( fileAccessPtr, MAX_STR_LEN, PERIOD,
                            dataBuffer, IGNORE_LEADING_WS ) != NO_ERR
                || compareString( dataBuffer, "End Program Meta-Data Code" )
                                                                    != STR_EQ )
            {
                // set access result to corrupted descriptor error
                accessResult = MD_CORRUPT_DESCRIPTOR_ERR;
            }
        }

        // otherwise, assumed start count not equal to end count
        else
        {
            // set access result to unbalance start/end error
            accessResult = UNBALANCED_START_END_ERR;
        }
    }

    // check for any errors found (not no error)
    if (accessResult != NO_ERR )
    {
        // clear the op command list
            // func: clearMetaDataList
        localHeadPtr = clearMetaDataList( localHeadPtr );
    }

    // close access file
        // func: fclose
    fclose( fileAccessPtr );

    // release temporary structure memory
        // fucntion: freee
    free( newNodePtr );

    // assign temporary local head pointer to parameter return pointer
    *opCodeDataHead = localHeadPtr;

    // return access result
    return accessResult;
}

/*
Function Name:  getOpCommand
Algorithm:      acquires one op command, verifies all parts of it,
                returns as parameter
Precondition:   file is open and file cursor is at beginning of an op code
Postcondition:  in correct operation, finds, tests, and returns op command as
                parameter, and returns status as integer
                - either complete op command found,
                or last op command found
Exceptions:     correctly and appropriately (without program failure) responds
                to and reports files access failure, incorrectly formatted op
                command letter, incorrectly formatted op command name, and/or
                incorrect or out of range of command value
Notes: none
 */
int getOpCommand( FILE *filePtr, OpCodeType *inData )
{
    // initialize func/vars

        // intiialize local constants -
        // max op name and op value lengths (10 & 9)
        const int MAX_OP_NAME_LENGTH = 10;  // actual max name length
        const int MAX_OP_VALUE_LENGTH = 9;  // actual max value length

        // intialize integer buffer value to zero
        long int  intBuffer = 0;

        // intialize source and destination indices to zero
        int sourceIndex = 0, destIndex = 0;

        // intialize otehr variables
        int accessResult;
        char strBuffer[ STD_STR_LEN ];

    //get whole op commadn as a string
        // func: getLineTo
    accessResult = getLineTo( filePtr, STD_STR_LEN, SEMICOLON,
                                        strBuffer, IGNORE_LEADING_WS );

    // check for successful access
    if( accessResult == NO_ERR )
    {
        // assign op command letter to struct component
        inData->opLtr = strBuffer[ sourceIndex ];
    }

    // otherwise, assume unsuccessful access
    else
    {
        // set pointer to data structure to null
        inData = NULL;

        // return op command access failure
        return OPCMD_ACCESS_ERR;
    }
    // verify op command letter
    switch( inData->opLtr )
    {
        // check for all correct cases
        case 'S':
        case 'A':
        case 'P':
        case 'M':
        case 'I':
        case 'O':
            break;

        // otherwise, assume not a correct case
        default:
            // set op command pointer to null
            inData = NULL;

            // return op command letter error
            return CORRUPT_OPCMD_LETTER_ERR;
    }

    // loop until left paren found
    while( sourceIndex < STD_STR_LEN && strBuffer[ sourceIndex ] != LEFT_PAREN )
    {
        // increment source index
        sourceIndex++;
    }

    // skip left paren element, increment source index
    sourceIndex++;

    // set op command text
    // loop until right parent found
    while( sourceIndex < STD_STR_LEN
            && destIndex < MAX_OP_NAME_LENGTH
                && strBuffer[ sourceIndex ] != RIGHT_PAREN )
    {
        // acquire letter
        inData->opName[ destIndex ] = strBuffer[ sourceIndex ];

        // increment source and destination indices
        destIndex++; sourceIndex++;

        // set end/null character to current end of string
        inData->opName[ destIndex ] = NULL_CHAR;
    }

    // check for incorrect op string
        // func: checkOpString
    if( checkOpString( inData->opName ) == False )
    {
        // set struct to null
        inData = NULL;

        // return corrupt op command found
        return CORRUPT_OPCMD_NAME_ERR;
    }

    // skip right paren element - increment source index, reset dest index
    sourceIndex++;
    destIndex = 0;

    // get integer value
    // loop while digits are found
        // function: isDigit
    while( sourceIndex < STD_STR_LEN
            && destIndex < MAX_OP_VALUE_LENGTH
                && isDigit( strBuffer [ sourceIndex ] ) == True )
    {
        // multiply current buffer by ten
         intBuffer *= 10;

        // add next integer value, converted from char to int
         intBuffer += (int)( strBuffer[ sourceIndex ] - '0' );

        // increment indices
         destIndex++; sourceIndex++;
    }
    // check for loop overrun failure, check specified lengths
    if( sourceIndex == STD_STR_LEN || destIndex == MAX_OP_VALUE_LENGTH )
    {
       // set struct to null
        inData = NULL;

        // return corrupt op command value found
        return CORRUPT_OPCMD_VALUE_ERR;
    }
    // set value to data structure component
    inData->opValue = intBuffer;

    // check for last op command "S(end)0"
        // func: compareString
    if( inData->opLtr == 'S'
            && compareString( inData->opName, "end" ) == STR_EQ )
    {
        // return last op command found message
        return LAST_OPCMD_FOUND_MSG;
    }
    // return complete op command found message
    return COMPLETE_OPCMD_FOUND_MSG;
}

/*
Function Name:  updateStartCount
Algorithm:      updates number of "start" op commands found in file
Precondition:   count >= 0, op string has "start" or other op name
Postcondition:  if op string has "start", input count + 1 is returned;
                otherwise, input count is returned unchanged
Exceptions:     none
Notes: none
 */
int updateStartCount( int count, char *opString )
{
    // check for "start" in op string
        // func: compareString
    if( compareString( opString, "start") == STR_EQ )
    {
        // return incremented start count
        return count + 1;
    }

    // return unchanged start count
    return count;
}

/*
Function Name:  updateEndCount
Algorithm:      updates number of "end" op commands found in file
Precondition:   count >= 0, op string has "end" or other op name
Postcondition:  if op string has "end", input count + 1 is returned;
                otherwise, input count is returned unchanged
Exceptions:     none
Notes: none
 */
int updateEndCount( int count, char *opString )
{
    // check for "end" in op string
        // func: compareString
    if( compareString( opString, "end") == STR_EQ )
    {
        // return incremented end count
        return count + 1;
    }

    // return unchanged end count
    return count;
}

/*
Function Name:  addNode
Algorithm:      adds op command structure with data to a linked list
Precondition:   linked list pointer assigned to null or to one op command link,
                struct pointer assigned to op command struct data
Postcondition:  assigns new structure node to beginning of linked list
                or end of established linked list
Exceptions:     none
Notes: assumes memory access/availability
 */
OpCodeType *addNode( OpCodeType *localPtr, OpCodeType *newNode )
{
    // check for local ptr assigned to null
    if( localPtr == NULL )
    {
        // access memory for new link/node
            // func: malloc
        localPtr = ( OpCodeType * ) malloc( sizeof( OpCodeType ) );
        // assign all three values to newly created node
        // assign next pointer to null
            // func: copyString
        localPtr->opLtr = newNode->opLtr;
        copyString( localPtr->opName, newNode->opName );
        localPtr->opValue = newNode->opValue;
        localPtr->next = NULL;

        // return current local pointer
        return localPtr;
    }
    // assume end of list not found yet
    // assign recursive function to current's next link
        // function: addNode
    localPtr->next = addNode( localPtr->next, newNode );

    // return current local pointer
    return localPtr;
}

/*
Function Name:  checkOpString
Algorithm:      checks tested op string against list of possibilities
Precondition:   tested op string is correct C-Style string with potential
                op command name in it
Postcondition:  in correct operation, verifies the test string with one of
                the potential op strings and returns true; otherwise,
                returns false
Exceptions:     none
Notes: none
 */
Boolean checkOpString( char *testStr )
{
    // check for all possible op names
        //func: compareString
    if( compareString( testStr, "access" )      == STR_EQ
     || compareString( testStr, "allocate" )    == STR_EQ
     || compareString( testStr, "end" )         == STR_EQ
     || compareString( testStr, "hard drive" )  == STR_EQ
     || compareString( testStr, "keyboard" )    == STR_EQ
     || compareString( testStr, "printer" )     == STR_EQ
     || compareString( testStr, "monitor" )     == STR_EQ
     || compareString( testStr, "run" )         == STR_EQ
     || compareString( testStr, "start" )       == STR_EQ )
    {
        // return found - true
        return True;
    }

    // return failure - false
    return False;
}

/*
Function Name:  isDigit
Algorithm:      checks for character digit, returns result
Precondition:   tests value is character
Postcondition:  if test value is a value '0' < value < '9', returns true,
                otherwise returns false
Exceptions:     none
Notes: none
 */
Boolean isDigit( char testChar )
{
    // check for test character between characters '0' - '9' inclusive
    if( testChar >= '0' && testChar <= '9' )
    {
        // return true
        return True;
    }

    // otherwise, assume character is not digit, return false
    return False;
}

/*
Function Name:  displayMetaData
Algorithm:      iterates through op code linked list,
                displays op code data individually
Precondition:   linked list, with or without data
                (should not be called if no data)
Postcondition:  displays all op codes in list
Exceptions:     none
Notes: none
 */
void displayMetaData( OpCodeType *localPtr )
{
    // display title, with underline
        // func: printf
    printf( "\nMeta-Data File Display\n" );
    printf( "======================\n\n" );
    // loop to end of linked list
    while( localPtr != NULL )
    {
        // print op code letter
            // func: printf
        printf( "Op code letter: %c\n", localPtr->opLtr );

        // print op code name
            // func: printf
        printf( "Op code name: %s\n", localPtr->opName );

        // print op code value
            // func: printf
        printf( "Op code value: %d\n\n", localPtr->opValue);

        // assign local pointer to next node
        localPtr = localPtr->next;
    }
}

/*
Function Name:  displayMetaDataError
Algorithm:      uses enum/cosntant values as indices to select display string,
                then displays error message with selected string
Precondition:   integer input code has one of the enumerated code values
Postcondition:  error message is displayed with the correct local error item
Exceptions:     none
Notes: none
 */
void displayMetaDataError( int code )
{
    // create string error list, 10 items, max 35 letters
    // includes 3 errors from StringManipError
    char errorList[ 10 ][ 35 ] =
                    { "No Error",
                      "Incomplete File Error",
                      "Input Buffer Overrun",
                      "MD File Access Error",
                      "MD COrrupt Descriptor Error",
                      "Op Command Access Error",
                      "Corrupt Op Command Letter Error",
                      "Corrupt Op Command Name Error",
                      "Corrupt Op Command Value Error",
                      "Unbalanced Start-End Code Error" };

    // display error to monitor with selected error string
        // func: printf
    printf( "\nFATAL ERROR: %s, Program aborted\n", errorList[ code ] );
}

/*
Function Name:  clearMetaDataList
Algorithm:      recursively iterates through op code linked list,
                returns memory to OS from the bottom of the list upward
Precondition:   linked list, with or without data
Postcondition:  all node memory, if any, is returned to OS,
                reutrn pointer (head) is set to null
Exceptions:     none
Notes: none
 */
OpCodeType *clearMetaDataList( OpCodeType *localPtr )
{
    // check for local pointer not set to null (list not empty)
    if( localPtr != NULL )
    {
        // check for lcoal pointer's next node not null
        if( localPtr->next != NULL )
        {
            // call recursive function with next pointer
                // func: clearMetaDataList
            clearMetaDataList( localPtr->next );
        }

        // after recursive call, release memory to OS
            // func: free
        free( localPtr );
    }

    // return null to calling function
    return NULL;
}






