// logStrHeader files
#include "SimUtils.h"

/*
Func. Name:     createPCBs
Algorithm:      iterates through op code list to identify and store
                all program (A) and stores them seperate in a doubly linked
                list
pre-condition:  opCodeList is passed in at the first (A) program following the initial
                (S) system start
post-condition: pcb's will be linked through doubly linked list internally
Notes:          For PA02 there is only one process and will create just one pcb
 */
void createPCBs( ProcessControlBlock *pcbHead, ConfigDataType *configData, OpCodeType *opCodeHead )
{
    // TODO implement while loop to parse through op codes to create multiple
    // pcbHeads which point to each other
    pcbHead->state = NEW;
    pcbHead->procNum = 0;
    pcbHead->procCycleRate = configData->procCycleRate;
    pcbHead->ioCycleRate = configData->ioCycleRate;
    pcbHead->programCounter = opCodeHead;
    getProcRuntime( pcbHead );
    pcbHead->prev = NULL;
    pcbHead->next = NULL;
}

/*
    changes process control board state to passed in value
 */
void changeProcState( ProcessControlBlock *pcb, int state )
{
    pcb->state = state;
}

/*
    adds up total runtime for passed in pcb and sets it to the pcb runtime var
 */
void getProcRuntime( ProcessControlBlock *pcb )
{
    int ioRate = pcb->ioCycleRate;
    int procRate = pcb->procCycleRate;
    pcb->runTime = 0;
    OpCodeType *localPtr = pcb->programCounter;

    while( isEndOfProcess( localPtr ) == False )
    {
        if( localPtr->opLtr == 'P')
        {
            pcb->runTime += procRate * localPtr->opValue;
        }
        else if( localPtr->opLtr == 'I' || localPtr->opLtr == 'O')
        {
            pcb->runTime += ioRate * localPtr->opValue;
        }
        localPtr = localPtr->next;
    }
}

/*
    function that is passed into pthread_create to run Input and Output
    processes in OSSimulator
 */
void runIOThread( void *runTime )
{
    runTimer( (int) runTime );
}

/*
    checks for end of sim, i.e. "S(end)0"
 */
Boolean isEndOfSim( OpCodeType *localPtr )
{
    if( localPtr->opLtr == 'S' && compareString( localPtr->opName, "end" ) == STR_EQ )
    {
        return True;
    }

    return False;
}

/*
    checks for end of process, i.e. "A(end)0"
 */
Boolean isEndOfProcess( OpCodeType *localPtr )
{
    // if opcode is A(end) return true
    if( localPtr->opLtr == 'A' && compareString( localPtr->opName, "end" ) == STR_EQ )
    {
        return True;
    }
    // assume not at end of current process
    return False;
}

/*
    master sim output function which controls the timer; checks for the log
    code and calls the corresponding function(s),
 */
void handleSimOutput( int sysMessage, int logToCode, LogString **logStrHead, ProcessControlBlock *pcb )
{
    // default to lap timer to get current timer value
    char timerStr[ 10 ];
    double timerValue = accessTimer( LAP_TIMER, timerStr );

    // if start message, zero timer to begin
    if( sysMessage == SYS_START_MSG )
    {
        timerValue = accessTimer( ZERO_TIMER, timerStr );
    }

    // if stop message, stop timer
    else if( sysMessage == SYS_STOP_MSG )
    {
        timerValue = accessTimer( STOP_TIMER, timerStr);
    }

    switch ( logToCode )
    {
        case LOGTO_MONITOR_CODE:

            displayToMonitor( sysMessage, timerStr, pcb );
            break;

        case LOGTO_FILE_CODE:

            logToStr( sysMessage, logStrHead, timerStr, pcb );
            break;

        case LOGTO_BOTH_CODE:

            displayToMonitor( sysMessage, timerStr, pcb );
            logToStr( sysMessage, logStrHead, timerStr, pcb );
            break;
    }
}

/*
    display function to output to screen depending on system message
 */
void displayToMonitor( int sysMessage, char *timerStr, ProcessControlBlock *pcb )
{
    switch( sysMessage )
    {
        case SYS_START_MSG:
            printf("  %s, OS: System Start\n", timerStr);
            break;

        case CREATE_PCB_MSG:
            printf("  %s, OS: Create Process Control Blocks\n", timerStr);
            break;

        case INIT_PCB_MSG:
            printf("  %s, OS: All processes initialized in NEW state\n", timerStr);
            break;

        case RDY_PCB_MSG:
            printf("  %s, OS: All processes now set in READY state\n", timerStr);
            break;

        case PROC_NUM_SELECT_TIME_REM_MSG:
            printf("  %s, OS: Process %d selected with %d ms remaining\n", timerStr, pcb->procNum, pcb->runTime);
            break;

        case PROC_NUM_RUNNING_MSG:
            printf("  %s, OS: Process %d set in RUNNING state\n\n", timerStr, pcb->procNum);
            break;

        case PROC_NUM_EXIT_MSG:
            printf("\n  %s, OS: Process %d ended and set in EXIT state\n", timerStr, pcb->procNum);
            break;

        case PROC_NUM_RUN_START:
            printf("  %s, Process %d, run operation start\n", timerStr, pcb->procNum);
            break;

        case PROC_NUM_RUN_STOP:
            printf("  %s, Process %d, run operation end\n", timerStr, pcb->procNum);
            break;

        case PROC_NUM_INPUT_START:
            printf("  %s, Process %d, %s input start\n", timerStr, pcb->procNum, pcb->programCounter->opName );
            break;

        case PROC_NUM_INPUT_STOP:
            printf("  %s, Process %d, %s input end\n", timerStr, pcb->procNum, pcb->programCounter->opName );
            break;

        case PROC_NUM_OUTPUT_START:
            printf("  %s, Process %d, %s output start\n", timerStr, pcb->procNum, pcb->programCounter->opName );
            break;

        case PROC_NUM_OUTPUT_STOP:
            printf("  %s, Process %d, %s output end\n", timerStr, pcb->procNum, pcb->programCounter->opName );
            break;

        case SYS_STOP_MSG:
            printf("  %s, OS: System Stop\n", timerStr );
            break;
    }
}

/*
    called by master sim output, logs given system message to a string for
    output later and adds the str to the linked list of strings to be logged
    at end of sim
 */
void logToStr(int sysMessage, LogString **logStrHead, char *timerStr, ProcessControlBlock *pcb )
{
    char currStr[ STD_STR_LEN ];

    switch(sysMessage) {
        case SYS_START_MSG:
            sprintf(currStr, "  %s, OS: System Start\n", timerStr);
            break;

        case CREATE_PCB_MSG:
            sprintf(currStr, "  %s, OS: Create Process Control Blocks\n", timerStr);
            break;

        case INIT_PCB_MSG:
            sprintf(currStr, "  %s, OS: All processes initialized in NEW state\n", timerStr);
            break;

        case RDY_PCB_MSG:
            sprintf(currStr, "  %s, OS: All processes now set in READY state\n", timerStr);
            break;

        case PROC_NUM_SELECT_TIME_REM_MSG:
            sprintf(currStr, "  %s, OS: Process %d selected with %d ms remaining\n", timerStr, pcb->procNum,
                    pcb->runTime);
            break;

        case PROC_NUM_RUNNING_MSG:
            sprintf(currStr, "  %s, OS: Process %d set in RUNNING state\n\n", timerStr, pcb->procNum);
            break;

        case PROC_NUM_EXIT_MSG:
            sprintf(currStr, "\n  %s, OS: Process %d ended and set in EXIT state\n", timerStr, pcb->procNum);
            break;

        case PROC_NUM_RUN_START:
            sprintf(currStr, "  %s, Process %d, run operation start\n", timerStr, pcb->procNum);
            break;

        case PROC_NUM_RUN_STOP:
            sprintf(currStr, "  %s, Process %d, run operation end\n", timerStr, pcb->procNum);
            break;

        case PROC_NUM_INPUT_START:
            sprintf(currStr, "  %s, Process %d, %s input start\n", timerStr, pcb->procNum,
                    pcb->programCounter->opName);
            break;

        case PROC_NUM_INPUT_STOP:
            sprintf(currStr, "  %s, Process %d, %s input end\n", timerStr, pcb->procNum,
                    pcb->programCounter->opName);
            break;

        case PROC_NUM_OUTPUT_START:
            sprintf(currStr, "  %s, Process %d, %s output start\n", timerStr, pcb->procNum,
                    pcb->programCounter->opName);
            break;

        case PROC_NUM_OUTPUT_STOP:
            sprintf(currStr, "  %s, Process %d, %s output end\n", timerStr, pcb->procNum,
                    pcb->programCounter->opName);
            break;

        case SYS_STOP_MSG:
            sprintf(currStr, "  %s, OS: System Stop\n", timerStr);
            break;
    }
    *logStrHead = addLogStrNode(*logStrHead, (char *) currStr);
}

/*
    simple add node utility function for the log string
 */
LogString *addLogStrNode( LogString *logStrHead, char *logStr )
{
    if(logStrHead == NULL)
    {
        LogString *newNode = createNode( logStr );
        logStrHead = newNode;
        return logStrHead;
    }
    // get to the last node
    LogString *cursor = logStrHead;
    while(cursor->next != NULL)
        cursor = cursor->next;

    /* create a new node */
    LogString *newNode =  createNode( logStr );
    cursor->next = newNode;

    return logStrHead;
}

/*
    create a new node initialize the str and next field to null
*/
LogString *createNode( char *str )
{
    LogString *newNode = (LogString *) malloc( sizeof( LogString ) );

    copyString( newNode->logStr, str);
    newNode->next = NULL;

    return newNode;
}

/*
    starting at head of log string linked list, this function logs each string
    one by one to the log file at the end of the sim run
 */
void logToFile( LogString *logStrHead, ConfigDataType *configData )
{
    const char WRITE_FILE_FLAG[] = "w";

    FILE *filePtr = fopen( configData->logToFileName, WRITE_FILE_FLAG );

    // intialize func/var
    char displayString[ STD_STR_LEN ];
    char opString[ STD_STR_LEN ];

    sprintf( displayString, "==================================================\n" );
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    clearString( displayString );
    sprintf( displayString, "Simulator Log File Header\n\n" );
    fwrite( displayString, 1, sizeof( displayString ), filePtr );

    clearString( displayString );
    sprintf( displayString, "Program file name               : %s\n", configData->metaDataFileName );
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    clearString( displayString );
    configCodeToString( configData->cpuSchedCode, opString );
    sprintf( displayString, "CPU schedule selection          : %s\n", opString  );
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    clearString( displayString );
    sprintf( displayString, "Quantum Cycles                  : %d\n", configData->quantumCycles );
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    clearString( displayString );
    sprintf( displayString, "Memory Available (KB)           : %ld\n", configData->memAvailable  );
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    clearString( displayString );
    sprintf( displayString, "Processor Cycle Rate (ms/cycle) : %d\n", configData->procCycleRate  );
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    clearString( displayString );
    sprintf( displayString, "I/O cycle rate (ms/cycle)       : %d\n", configData->ioCycleRate );
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    clearString( displayString );
    sprintf( displayString, "\n======================\n");
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    clearString( displayString );
    sprintf( displayString, "Begin Simulation\n\n" );
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    while( logStrHead != NULL )
    {
        fwrite( logStrHead->logStr, sizeof( char ), sizeof( logStrHead->logStr ), filePtr );
        logStrHead = logStrHead->next;
    }

    clearString( displayString );
    sprintf( displayString, "\nEnd Simulation - Complete\n" );
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    clearString( displayString );
    sprintf( displayString, "\n=========================\n");
    fwrite( displayString, sizeof( char ), sizeof( displayString ), filePtr );

    fclose(filePtr);
}


