#ifndef SIM_UTILS_H
#define SIM_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "StringUtils.h"
#include "simtimer.h"
#include "ConfigAccess.h"
#include "MetaDataAccess.h"
#include "OSSimulator.h"

typedef enum {  SYS_START_MSG = 1,
                CREATE_PCB_MSG,
                INIT_PCB_MSG,
                RDY_PCB_MSG,
                PROC_NUM_SELECT_TIME_REM_MSG,
                PROC_NUM_RUNNING_MSG,
                PROC_NUM_RUN_START,
                PROC_NUM_RUN_STOP,
                PROC_NUM_INPUT_START,
                PROC_NUM_INPUT_STOP,
                PROC_NUM_OUTPUT_START,
                PROC_NUM_OUTPUT_STOP,
                PROC_NUM_EXIT_MSG,
                SYS_STOP_MSG } SystemMessages;

typedef enum {  NEW,
                READY,
                RUNNING,
                EXIT } StateStatus;

typedef struct ProcessControlBlock
{
    int state;
    int procNum;
    int procCycleRate;
    int ioCycleRate;
    int runTime;
    OpCodeType *programCounter;

    struct ProcessControlBlock *prev;
    struct ProcessControlBlock *next;
} ProcessControlBlock;

typedef struct LogString
{
    char logStr[ 80 ];

    struct LogString *next;
} LogString;

// functoin prototypes
void createPCBs( ProcessControlBlock *pcbHead, ConfigDataType *configData, OpCodeType *opCodeHead );
void changeProcState( ProcessControlBlock *pcb, int state );
void getProcRuntime( ProcessControlBlock *pcb );
void runIOThread( void *runTime );
Boolean isEndOfSim( OpCodeType *opCodeList );
Boolean isEndOfProcess( OpCodeType *localPtr );
void handleSimOutput( int sysMessage, int logToCode, LogString **logStrHead, ProcessControlBlock *pcb );
void displayToMonitor( int sysMessage, char *timerStr, ProcessControlBlock *pcb );
void logToStr(int sysMessage, LogString **logStrHead, char *timerStr, ProcessControlBlock *pcb);
LogString *addLogStrNode( LogString *logStrHead, char *logStr );
LogString *createNode( char *str );
void logToFile( LogString *logStrHead, ConfigDataType *configData );


#endif // SIM_UTILS_H

