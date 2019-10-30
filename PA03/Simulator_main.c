// header files
#include <stdio.h>
#include "ConfigAccess.h"
#include "MetaDataAccess.h"
#include "SimUtils.h"

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
    int configAccessResult, mdAccessResult;
    char configFileName[MAX_STR_LEN];
    char mdFileName[MAX_STR_LEN];
    ConfigDataType *configDataPtr;
    OpCodeType *mdDataPtr;

    printf("\nSimulator Program\n");
    printf("==================================\n\n");

    printf("Uploading Configuration Files\n");

    if(argc < 2)
    {
        printf("ERROR: Program requires file name for config file");
        printf(" as command line argument\n");
        printf("Program terinated\n");
        return 1;
    }

    copyString(configFileName, argv[1]);
    configAccessResult = getConfigData(configFileName, &configDataPtr);

    if(configAccessResult != NO_ERR)
    {
        displayConfigError(configAccessResult);
        clearConfigData(&configDataPtr);
        printf("\n");
        return 1;
    }

    printf("\nUploading Metadata Files\n");

    copyString(mdFileName, configDataPtr->metaDataFileName);
    mdAccessResult = getOpCodes(mdFileName, &mdDataPtr);

    if(mdAccessResult != NO_ERR)
    {
        displayMetaDataError(mdAccessResult);
        clearConfigData(&configDataPtr);
        mdDataPtr = clearMetaDataList(mdDataPtr);    // returns null
        printf("\n");
        return 1;
    }

    // else there are no errors and we can begin system run

    // OS Simulator Start
    runSimulator( configDataPtr, mdDataPtr );

    // shut down, clean up program
    // clear configuration data
        // function: clearConfigData
    clearConfigData( &configDataPtr );

    // clear meta data
        // func: clearMetaDataList
    mdDataPtr = clearMetaDataList( mdDataPtr );

    // add endline for vertical spacing
        // func: printf
    printf( "\n" );

    // return success
    return 0;
}

