// header files
#include <stdio.h>
#include "ConfigAccess.h"
#include "MetaDataAccess.h"
#include "simtimer.h"
#include "SimUtils.h"
#include "OSSimulator.h"

/*
Function Name:  main
Algorithm:      driver function to gather config and meta data and
                simulate processing of programs with help from
                config and metadata files.
Precondition:   none
Postconditoin:  none
Exceptions:     none
Notes:          Will output to file, monitor, or both
 */
int main( int argc, char **argv )
{
    // intialize func/vars
    int configAccessResult, mdAccessResult;
    char configFileName[ MAX_STR_LEN ];
    char mdFileName[ MAX_STR_LEN ];
    ConfigDataType *configDataPtr;
    OpCodeType *mdData;

    printf("\nSimulator Program\n");
    printf("==================\n");

    // display component title
        // function: printf
    printf("\nUploading Configuration Files...\n");

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

    // check for unsuccessful upload
    if( configAccessResult != NO_ERR )
    {
        // display configuration upload error
            // function: displayConfigError
        displayConfigError( configAccessResult );
        return 1;
    }

    // display component title
        // func: printf
    printf( "\nUploading Meta Data Files...\n" );

    // get data from meta data file
        // func: copyString, getOpCodes
    copyString( mdFileName, configDataPtr->metaDataFileName );
    mdAccessResult = getOpCodes( mdFileName, &mdData );

    // check for unsuccesful upload
    if( mdAccessResult != NO_ERR )
    {
        // display meta data error message
            // func: displayMetaDataError
        displayMetaDataError( mdAccessResult );
        return 1;
    }

    // else there are no errors and we can begin system run

    // display sim start title header
    printf("\n======================\n");
    printf("Begin Simulation\n\n");

    // OS Simulator Start
    startSimulator( configDataPtr, mdData );

    // display sim end title header
    printf("\nEnd of Simulation\n");
    printf("=====================\n");

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

