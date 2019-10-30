// header files
#include "OSSimulator.h"

/*
Function:       startSimulator
Algorithm:      starts up the simulator and creates necessary process control
                blocks, set their states to ready, and run them starting with
                the first (0) process
Precondition:   configuration data and op code list are correctly configured
                with no errors and the op code list is in the correct order
                starting with S(start) -> A(start)
Postcondition:  Simulator has ended after outputing the run
Notes:           none
 */
void startSimulator( ConfigDataType *configData, OpCodeType *opCodeList)
{
    // initialize variables and functions
    LogString *logStrHead = NULL;
    ProcessControlBlock *pcbHead =
                (ProcessControlBlock *) malloc(sizeof( ProcessControlBlock ) );
    int logToCode = configData->logToCode;
    int runTime;
    pthread_t tid;

    // display start message and begin timer
    handleSimOutput( SYS_START_MSG, logToCode, &logStrHead, pcbHead );
    // get next op command, system has been started
    opCodeList = opCodeList->next;

    // create process control block and output message
    createPCBs( pcbHead, configData, opCodeList );
    handleSimOutput( CREATE_PCB_MSG, logToCode, &logStrHead, pcbHead );

    // processes are initialized in new state
    handleSimOutput( INIT_PCB_MSG, logToCode, &logStrHead, pcbHead );

    // set all processes in 'ready' state.
    // Note: using function for future modularity in a while loop
    changeProcState( pcbHead, READY );
    handleSimOutput( RDY_PCB_MSG, logToCode, &logStrHead, pcbHead );

    handleSimOutput( PROC_NUM_SELECT_TIME_REM_MSG,
                                    logToCode, &logStrHead, pcbHead );
    // set process to running state
    changeProcState( pcbHead, RUNNING );
    handleSimOutput( PROC_NUM_RUNNING_MSG,
                                    logToCode, &logStrHead, pcbHead );

    // main control loop
    while( isEndOfSim( pcbHead->programCounter ) == False )
    {
        // get next process (only one for now but more later)
        // select process and display time remaining

        // get next op code for that process
        while( isEndOfProcess( pcbHead->programCounter ) == False )
        {
            // run? -> run
            if( pcbHead->programCounter->opLtr == 'P' )
            {
                runTime = pcbHead->procCycleRate * pcbHead->programCounter->opValue;
                handleSimOutput( PROC_NUM_RUN_START, logToCode, &logStrHead, pcbHead );
                runTimer( runTime );
                // interrupt? -> interrupt manager (PA03)
                    // check for interrupt
                    // resolve interrupt (interrupt queue)
                handleSimOutput( PROC_NUM_RUN_STOP, logToCode, &logStrHead, pcbHead );
            }

            // I/O? -> call out I/o
            if( pcbHead->programCounter->opLtr == 'I' )
            {
                runTime = pcbHead->ioCycleRate * pcbHead->programCounter->opValue;
                // block process (pthread_create and join)
                handleSimOutput( PROC_NUM_INPUT_START, logToCode, &logStrHead, pcbHead );
                pthread_create( &tid, NULL, (void *)runIOThread, (void *) runTime);
                pthread_join( tid, NULL );
                handleSimOutput( PROC_NUM_INPUT_STOP, logToCode, &logStrHead, pcbHead );
            }

            if( pcbHead->programCounter->opLtr == 'O' )
            {
                runTime = pcbHead->ioCycleRate * pcbHead->programCounter->opValue;
                // block process (pthread_create and join)
                handleSimOutput( PROC_NUM_OUTPUT_START, logToCode, &logStrHead, pcbHead );
                pthread_create( &tid, NULL, (void *)runIOThread, (void *) runTime);
                pthread_join( tid, NULL );
                handleSimOutput( PROC_NUM_OUTPUT_STOP, logToCode, &logStrHead, pcbHead );
            }
            pcbHead->programCounter = pcbHead->programCounter->next;
        }

        // outside of loop so process has ended, display and increment to next
        handleSimOutput( PROC_NUM_EXIT_MSG, logToCode, &logStrHead, pcbHead );
        pcbHead->programCounter = pcbHead->programCounter->next;
    }

    // outside of main control loop so we are at the end of sim
    handleSimOutput( SYS_STOP_MSG, logToCode, &logStrHead, pcbHead );

    // check if log to file is necessary
    if( logToCode != LOGTO_MONITOR_CODE )
    {
        logToFile( logStrHead, configData );
    }

    // free pcb list
    free( pcbHead );

    // free logString
    free( logStrHead );
}
