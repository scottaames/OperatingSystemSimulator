// header files
#include <stdio.h>
#include "ConfigAccess.h"
#include "MetaDataAccess.h"

/*
Function Name:  main
Algorithm:      driver function to test metadata and config file upload
                operation together
Precondition:   none
Postcondition:  returns zero (0) on success
Exceptions:     none
Notes: Demonstrats use of combined files
 */
int main( int argc, char **argv )
{
    // intialize func/vars
    int configAccessResult, mdAccessResult;
    char configFileName[ MAX_STR_LEN ];
    char mdFileName[ MAX_STR_LEN ];
    ConfigDataType *configDataPtr;
    OpCodeType *mdData;

    // display component title
        // function: printf
    printf("\nConfig File Upload Component\n");
    printf("\n========================\n");

    // check for not correct number of command line arguments (two)
    if( argc < 2 )
    {
        // print missing command line argument error
        printf( "ERROR: Program requires file name for config file " );
        printf( "as command line argument\n" );
        printf( "Program Terminated\n" );

        // return non-normal program result
        return 1;
    }

    // get data from config file
        // function: copyString, getConfigData
    copyString( configFileName, argv[ 1 ] );
    configAccessResult = getConfigData( configFileName, &configDataPtr );

    // check for successful upload
    if( configAccessResult == NO_ERR )
    {
        // display config file
            // function: displayConfigData
        displayConfigData( configDataPtr );
    }

    // otherwise, assume failed upload
    else
    {
        // display configuration upload error
            // function: displayConfigError
        displayConfigError( configAccessResult );
    }

    // display component title
        // func: printf
    printf( "\nMeta Data File Upload Component\n" );
    printf( "==========================\n" );

    // get data from meta data file
        // func: copyString, getOpCodes
    copyString( mdFileName, configDataPtr->metaDataFileName );
    mdAccessResult = getOpCodes( mdFileName, &mdData );

    // check for succesful upload
    if( mdAccessResult == NO_ERR )
    {
        // display meta data file
            // func: displayMetaData
        displayMetaData( mdData );
    }

    // otherwise, assume unsuccessful upload
    else
    {
        // display meta data error message
            // func: displayMetaDataError
        displayMetaDataError( mdAccessResult );
    }

    // shut down, clean up program
    // clear configuration data
        // function: clearConfigData
    clearConfigData( &configDataPtr );

    // clear meta data
        // func: clearMetaDataList
    mdData = clearMetaDataList( mdData );

    // add endline for vertical spacing
        // func: printf
    printf( "\n" );

    // return success
    return 0;
}



