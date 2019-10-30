#ifndef SIM_UTILS_H
#define SIM_UTILS_H

#include <pthread.h>
#include "StringUtils.h"
#include "simtimer.h"
#include "ConfigAccess.h"
#include "MetaDataAccess.h"

// constant for initial memory op creation
extern const int INIT_MEM;

typedef enum {  NEW,
                READY,
                RUNNING,
                EXIT } ProcStateStatus;

typedef struct MemMgmtUnit
{
    int procNum;
    int id;
    int base;
    int offset;
    struct MemMgmtUnit *next;
} MemMgmtUnit;

typedef struct
{
    int state;
    int procNum;
    double timeRemaining;
    int totalMemAvailable;
    OpCodeType *progCntr;
    MemMgmtUnit *memOpHead;
} ProcessControlBlock;

typedef struct LogString
{
    char outputStr[200];
    struct LogString *next;
} LogString;

typedef struct
{
    int logToCode;
    OpCodeType *opCode;
    int procNum;
    LogString *logStringPtr;
    double timeRemaining;
} ThreadRunner;

// functoin prototypes
void runSimulator( ConfigDataType *configData, OpCodeType *opCodeList);
void createPCBs( ProcessControlBlock pcbArray[], ConfigDataType *configData, OpCodeType *opCodeList );
int getNumProcesses( OpCodeType *opCodeList );
Boolean isProcessStart( OpCodeType *opCodeList );
double setProcRuntimes( ProcessControlBlock newPcb, int procRate, int ioRate );
void sortForSJF( ProcessControlBlock pcbArray[], int numProcesses );
void swap( int index1, int index2, ProcessControlBlock pcbArray[] );
void *runThread( void *thread );
void runCurrPcb( ProcessControlBlock currPcb, ConfigDataType *configData, LogString *logStringPtr );
Boolean runMemoryMgr( int logToCode, LogString *logStrHead, ProcessControlBlock currPcb );
MemMgmtUnit *addMemOpNode( MemMgmtUnit *memOpHead, int memId, int memBase, int memOffset, int procNum );
Boolean isMemSpaceFree( MemMgmtUnit *memOp, int procNumCheck, int baseCheck, int idCheck );
Boolean requestMemAccess( MemMgmtUnit *memOpHead, int procNum, int memId, int memBase, int memOffset );
MemMgmtUnit *clearMemoryList( MemMgmtUnit *memOpHead );
Boolean isEndOfSim( OpCodeType *opCodeList );
Boolean isEndOfProcess( OpCodeType *localPtr );
LogString *outputLine(int logToCode, LogString *ouptutPtr, char *inputStr);
void outputToFile(LogString *ouptutPtr, char *outputFileName);
LogString *createLogFileHeader(ConfigDataType *configData, LogString *ouptutPtr);
LogString *addLogStrNode(LogString *head, char *logString);
LogString *clearLogString(LogString *head);

#endif // SIM_UTILS_H

