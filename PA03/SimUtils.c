// Header files
#include "SimUtils.h"

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
void runSimulator( ConfigDataType *configData, OpCodeType *opCodeList)
{
    // initialize variables and functions
    LogString *logStringPtr = NULL;
    char tempString[MAX_STR_LEN];
    char timerString[MAX_STR_LEN];
    int numProcesses;
    int logToCode = configData->logToCode;

    // create log file header if necessary
    if(logToCode != LOGTO_MONITOR_CODE)
    {
        logStringPtr = createLogFileHeader(configData, logStringPtr);
    }

    logStringPtr = outputLine(logToCode, logStringPtr,
                                                         "\n==================================\n");
    logStringPtr = outputLine(logToCode, logStringPtr,
                                                        "Begin Simulation\n\n");

    // begin timer and display system start
    accessTimer(ZERO_TIMER, timerString);
    sprintf(tempString, "  %s, OS: System Start\n", timerString);
    logStringPtr = outputLine(logToCode, logStringPtr, tempString);

    // get next op command, system has been started
    opCodeList = opCodeList->next;

    // create process control blocks inside array and output message
    accessTimer(LAP_TIMER, timerString);
    sprintf(tempString, "  %s, OS: Create Process Control Blocks\n", timerString);
    logStringPtr = outputLine(logToCode, logStringPtr, tempString);
    numProcesses = getNumProcesses( opCodeList );
    ProcessControlBlock pcbArray[ numProcesses ];
    createPCBs( pcbArray, configData, opCodeList );

    // processes are initialized in new state
    accessTimer(LAP_TIMER, timerString);
    sprintf(tempString, "  %s, OS: All Processes initialized in NEW state\n",
                                                                       timerString);
    logStringPtr = outputLine(logToCode, logStringPtr, tempString);

    // if shortest job first, sort accordingly
    if( configData->cpuSchedCode == CPU_SCHED_SJF_N_CODE )
    {
        sortForSJF( pcbArray, numProcesses );
    }

    // set all processes in 'ready' state.
    for( int index = 0; index < numProcesses; index++)
    {
        pcbArray[ index ].state = READY;
    }

    // output processes ready message
    accessTimer(LAP_TIMER, timerString);
    sprintf(tempString, "  %s, OS: All Processes now set in READY state\n", timerString);
    logStringPtr = outputLine(logToCode, logStringPtr, tempString);

    // run each process
    for( int index = 0; index < numProcesses; index++)
    {
        runCurrPcb(pcbArray[index], configData, logStringPtr);
    }

    //out of loop - system run has ended. output messages
    accessTimer(LAP_TIMER, timerString);
    sprintf(tempString, "  %s, OS: System Stop\n", timerString);
    logStringPtr = outputLine(logToCode, logStringPtr, tempString);

    logStringPtr = outputLine(logToCode, logStringPtr,
                                                "\nEnd Simulation - Complete\n");
    logStringPtr = outputLine(logToCode, logStringPtr,
                                                "==================================\n");
    // create log file if necessary
    if(logToCode != LOGTO_MONITOR_CODE)
    {
        outputToFile(logStringPtr, configData->logToFileName);
    }

    // clear log string
    logStringPtr = clearLogString(logStringPtr);
}

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

void createPCBs( ProcessControlBlock pcbArray[], ConfigDataType *configData, OpCodeType *opCodeList )
{
    ProcessControlBlock newPcb;
    int counter = 0;

    while( isEndOfSim(opCodeList) == False )
    {
        if( isProcessStart(opCodeList) == True )
        {
            newPcb.state = NEW;
            newPcb.procNum = counter;
            newPcb.progCntr= opCodeList;
            newPcb.totalMemAvailable = configData->memAvailable;
            newPcb.memOpHead = NULL;
            newPcb.timeRemaining = setProcRuntimes( newPcb, configData->procCycleRate, configData->ioCycleRate );
            pcbArray[ counter ] = newPcb;
            counter++;
        }
        opCodeList = opCodeList->next;
    }
}

int getNumProcesses( OpCodeType *opCodeList )
{
    int count = 0;

    while( isEndOfSim( opCodeList ) == False )
    {
        if( isProcessStart( opCodeList ) == True )
        {
            count++;
        }
        opCodeList = opCodeList->next;
    }
    return count;
}

Boolean isProcessStart( OpCodeType *opCodeList )
{
    if( opCodeList->opLtr == 'A'
        && compareString( opCodeList->opName, "start" ) == STR_EQ )
    {
        return True;
    }
    return False;
}

/*
    adds up total runtime for passed in pcb and sets it to the pcb runtime var
 */
double setProcRuntimes( ProcessControlBlock newPcb, int procRate, int ioRate )
{
    double timeRemaining = 0.00;
    OpCodeType *localPtr = newPcb.progCntr;

    while( isEndOfProcess( localPtr ) == False )
    {
        if( localPtr->opLtr == 'P')
        {
            localPtr->timeRemaining = procRate * localPtr->opValue;
            timeRemaining += (double)localPtr->timeRemaining;
        }
        else if( localPtr->opLtr == 'I' || localPtr->opLtr == 'O')
        {
            localPtr->timeRemaining = ioRate * localPtr->opValue;
            timeRemaining += (double)localPtr->timeRemaining;
        }
        localPtr = localPtr->next;
    }
    return timeRemaining;
}

/*
    Simple Bubble sort algorithm to order process control block
    array into shortest run time process to longest run time process
 */
void sortForSJF( ProcessControlBlock pcbArray[], int numProcesses )
{
    ProcessControlBlock min, otherProc;

    for(int outerIndex = 0; outerIndex < numProcesses - 1; outerIndex++)
    {
        min = pcbArray[outerIndex];

        for(int innerIndex = outerIndex + 1; innerIndex < numProcesses; innerIndex++)
        {
            otherProc = pcbArray[innerIndex];
            if(otherProc.timeRemaining < min.timeRemaining)
            {
                swap(outerIndex, innerIndex, pcbArray);
                min = pcbArray[outerIndex];
            }
        }
    }
}

void swap( int index1, int index2, ProcessControlBlock pcbArray[] )
{
    ProcessControlBlock temp = pcbArray[index1];
    pcbArray[index1] = pcbArray[index2];
    pcbArray[index2] = temp;
}

/*
    function that is passed into pthread_create to run Input and Output
    processes in OSSimulator
 */
void *runThread( void *thread )
{
    ThreadRunner *threadToRun = (ThreadRunner *) thread;
    char *outString = threadToRun->opCode->opName;
    char timerString[MAX_STR_LEN];
    char tempString[MAX_STR_LEN];

    switch( threadToRun->opCode->opLtr )
    {
        case 'P':
            concatenateString(outString, " operation");
            break;
        case 'I':
            concatenateString(outString, " input");
            break;
        case 'O':
            concatenateString(outString, " output");
            break;
    }

    accessTimer(LAP_TIMER, timerString);
    sprintf(tempString, "  %s, Process: %d, %s start\n", timerString, threadToRun->procNum, outString);
    threadToRun->logStringPtr = outputLine(threadToRun->logToCode, threadToRun->logStringPtr, tempString);

    runTimer(threadToRun->opCode->timeRemaining);

    accessTimer(LAP_TIMER, timerString);
    sprintf(tempString, "  %s, Process: %d, %s end\n", timerString, threadToRun->procNum, outString);
    threadToRun->logStringPtr = outputLine(threadToRun->logToCode, threadToRun->logStringPtr, tempString);

    pthread_exit(NULL);
}

void runCurrPcb( ProcessControlBlock currPcb, ConfigDataType *configData, LogString *logStringPtr )
{
    char tempString[MAX_STR_LEN];
    char timerString[MAX_STR_LEN];
    Boolean segFaultOccurred;

    ThreadRunner threadToRun;
    threadToRun.logStringPtr = logStringPtr;
    threadToRun.logToCode = configData->logToCode;
    threadToRun.procNum = currPcb.procNum;

    pthread_t tid;
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);

    // output process selection
    accessTimer(LAP_TIMER, timerString);
    sprintf(tempString, "  %s, OS: Process %d selected with %.0f ms remaining\n",
                timerString, currPcb.procNum, currPcb.timeRemaining);
    logStringPtr = outputLine(configData->logToCode, logStringPtr, tempString);

    // output process running
    accessTimer(LAP_TIMER, timerString);
    sprintf(tempString, "  %s, OS: Process %d set in RUNNING state\n\n", timerString,
                                                        currPcb.procNum);
    logStringPtr = outputLine(configData->logToCode, logStringPtr, tempString);
    currPcb.state = RUNNING;

    // skip A(start) that is passed in
    currPcb.progCntr = currPcb.progCntr->next;

    while( isEndOfProcess( currPcb.progCntr ) == False )
    {
        if( currPcb.progCntr->opLtr == 'M')
        {
             segFaultOccurred = runMemoryMgr( configData->logToCode, logStringPtr, currPcb );

            if( segFaultOccurred == True )
            {
                accessTimer( LAP_TIMER, timerString );
                sprintf( tempString, "  %s, Process: %d, experiences segmentation fault\n",
                                                timerString, currPcb.procNum );
                logStringPtr = outputLine(configData->logToCode, logStringPtr, tempString);
                break;
            }
        }
        else
        {
            // Run thread
            threadToRun.opCode = currPcb.progCntr;
            pthread_create(&tid, &tattr, runThread, &threadToRun);
            pthread_join(tid, NULL);

            // update time remaining for process
            currPcb.timeRemaining -= threadToRun.timeRemaining;
        }
        currPcb.progCntr = currPcb.progCntr->next;
    }
    // Clear memory list after each process run
    currPcb.memOpHead = clearMemoryList(currPcb.memOpHead);

    accessTimer( LAP_TIMER, timerString );
    sprintf( tempString, "\n  %s, OS: Process %d ended and set in EXIT state\n",
                                                timerString, currPcb.procNum );
    logStringPtr = outputLine( configData->logToCode, logStringPtr, tempString );
    currPcb.state = EXIT;
}

Boolean runMemoryMgr( int logToCode, LogString *logStringPtr, ProcessControlBlock currPcb )
{
    // variables and calculation to store memory data needed
    int id = currPcb.progCntr->opValue / 1000000;
    int base = (currPcb.progCntr->opValue / 1000) % 1000;
    int offset = currPcb.progCntr->opValue % 1000;
    char tempString[MAX_STR_LEN];
    char timerString[MAX_STR_LEN];
    Boolean segFaultOccurred = True;

    if( compareString( currPcb.progCntr->opName, "allocate" ) == STR_EQ )
    {
        // output message
        accessTimer( LAP_TIMER, timerString );
        sprintf(tempString, "  %s, Process: %d, MMU attempt to allocate %d/%d/%d\n",
                             timerString, currPcb.procNum, id, base, offset );
        logStringPtr = outputLine( logToCode, logStringPtr, tempString );

        // check if mem space is already allocated to or base larger than capac.
        if( isMemSpaceFree( currPcb.memOpHead, currPcb.procNum, base, id ) == False
            || base > currPcb.totalMemAvailable )
        {
            // output message
            accessTimer( LAP_TIMER, timerString );
            sprintf( tempString, "  %s, Process: %d, MMU failed to allocate\n", timerString, currPcb.procNum);
            logStringPtr = outputLine( logToCode, logStringPtr, tempString );

            return segFaultOccurred;
        }

        // output message
        currPcb.memOpHead = addMemOpNode( currPcb.memOpHead, id, base, offset, currPcb.procNum );
        accessTimer( LAP_TIMER, timerString );
        sprintf( tempString, "  %s, Process: %d, MMU successful allocation\n",
                                                       timerString, currPcb.procNum );
        logStringPtr = outputLine( logToCode, logStringPtr, tempString );
    }
    else if( compareString( currPcb.progCntr->opName, "access" ) == STR_EQ )
    {
        accessTimer( LAP_TIMER, timerString );
        sprintf( tempString, "  %s, Process: %d, MMU attempt to access %d/%d/%d\n", timerString, currPcb.procNum, id, base, offset );
        logStringPtr = outputLine( logToCode, logStringPtr, tempString );

        if ( requestMemAccess( currPcb.memOpHead, currPcb.procNum, id, base, offset ) == False )
        {
            accessTimer(LAP_TIMER, timerString);
            sprintf( tempString, "  %s, Process: %d, MMU failed to access\n", timerString, currPcb.procNum );
            logStringPtr = outputLine( logToCode, logStringPtr, tempString );

            return segFaultOccurred;
        }
    }
    // Seg Fault did not occur
    return False;
}

MemMgmtUnit *addMemOpNode( MemMgmtUnit *memOpHead, int memId, int memBase, int memOffset, int procNum )
{
    if(memOpHead == NULL)
    {
        memOpHead = (MemMgmtUnit *) malloc( sizeof( MemMgmtUnit ) );
        memOpHead->procNum = procNum;
        memOpHead->id = memId;
        memOpHead->base = memBase;
        memOpHead->offset = memOffset;
        memOpHead->next = NULL;
        return memOpHead;
    }

    memOpHead->next = addMemOpNode(memOpHead->next, memId, memBase, memOffset, procNum);
    return memOpHead;
}

Boolean isMemSpaceFree( MemMgmtUnit *memOpPtr, int procNumCheck, int baseCheck, int idCheck )
{
    if( memOpPtr == NULL )
    {
        return True;
    }
    else if( memOpPtr->base == baseCheck
        || (memOpPtr->procNum == procNumCheck && memOpPtr->id == idCheck) )
    {
        return False;
    }
    return isMemSpaceFree( memOpPtr->next, procNumCheck, baseCheck, idCheck );
}

Boolean requestMemAccess( MemMgmtUnit *memOpHead, int procNum, int memId, int memBase, int memOffset )
{
    // If requested memory has not been found or we have not added
    // any nodes to the memory list besides the placeholder node.
    if( memOpHead == NULL )
    {
        return False;
    }
    else if( memOpHead->procNum == procNum
                 && memOpHead->base == memBase
                     && memOpHead->id == memId )
    {
        if(memOffset <= memOpHead->offset)
        {
            return True;
        }
        else
        {
            return False;
        }
    }

    return requestMemAccess( memOpHead->next, procNum, memId, memBase, memOffset );
}

MemMgmtUnit *clearMemoryList( MemMgmtUnit *memOpHead )
{
    if( memOpHead != NULL )
    {
        if( memOpHead->next != NULL )
        {
          clearMemoryList( memOpHead->next );
        }
        free( memOpHead );
    }
    return NULL;
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

/**
 * Handles output depending on logToCode.
 * If logToCode is MONITOR, prints string.
 * If logToCode is FILE, saves string to buffer.
 * If logToCode is BOTH, prints and saves string to buffer.
 *
 * Note: the string is always added to the buffer to not mess up the
 *       output head pointer, but only outputted to file at end if
 *       logToCode is FILE or BOTH.
 */
LogString *outputLine(int logToCode, LogString *ouptutPtr, char *inputStr)
{
    ouptutPtr = addLogStrNode(ouptutPtr, inputStr);

    if(logToCode != LOGTO_FILE_CODE)
    {
        printf("%s", inputStr);
    }
    return ouptutPtr;
}


/**
 * @details Outputs buffer to the log file.
 *
 * @param[out] ouptutPtr Ouptut linked list
 *
 * @param[in] outputFileName The name of the output log file
 *
 * @return none
 */
void outputToFile(LogString *ouptutPtr, char *outputFileName)
{
    FILE *outputFile = fopen(outputFileName, "w");

    while(ouptutPtr != NULL)
    {
        fprintf(outputFile, "%s", ouptutPtr->outputStr);
        ouptutPtr = ouptutPtr->next;
    }
    fclose(outputFile);
}


/**
 * @details Creates log file header when log to code is FILE or BOTH
 *
 * @param[in] configData Pointer to config data struct
 *
 * @param[out] ouptutPtr Ouptut linked list
 *
 * @return LogString Head of linked list
 */
LogString *createLogFileHeader(ConfigDataType *configData, LogString *ouptutPtr)
{
    char tempStr[MAX_STR_LEN];

    ouptutPtr = addLogStrNode(ouptutPtr, "===================================");
    ouptutPtr = addLogStrNode(ouptutPtr,  "Simulator Log File Header\n\n");

    sprintf(tempStr, "File Name                       : %s\n",
                                                  configData->metaDataFileName);

    ouptutPtr = addLogStrNode(ouptutPtr, tempStr);

    char cpuSchedCode[MAX_STR_LEN];
    configCodeToString(configData->cpuSchedCode, cpuSchedCode);
    sprintf(tempStr, "CPU Scheduling                  : %s\n", cpuSchedCode);
    ouptutPtr = addLogStrNode(ouptutPtr, tempStr);

    sprintf(tempStr, "Quantum Cycles                  : %d\n",
                                                     configData->quantumCycles);
    ouptutPtr = addLogStrNode(ouptutPtr, tempStr);

    sprintf(tempStr, "Memory Available (KB)           : %d\n",
                                                      configData->memAvailable);
    ouptutPtr = addLogStrNode(ouptutPtr, tempStr);

    sprintf(tempStr, "Processor Cycle Rate (ms/cycle) : %d\n",
                                                     configData->procCycleRate);
    ouptutPtr = addLogStrNode(ouptutPtr, tempStr);

    sprintf(tempStr, "I/O Cycle Rate (ms/cycle)       : %d\n\n",
                                                       configData->ioCycleRate);
    ouptutPtr = addLogStrNode(ouptutPtr, tempStr);
    return ouptutPtr;
}

/**
 * @brief Creates new LogString node.
 *
 * @details Checks if the local head pointer is NULL. If so, creates
 *          new node to be the head pointer. If not, adds new node to end of
 *          linked list.
 *
 * @param[in] head LogString head pointer
 *
 * @param[in] logString Ouptut string for new node
 *
 * @return LogString Head of linked list
 */
LogString *addLogStrNode(LogString *head, char *logString)
{
    if(head == NULL)
    {
        head = (LogString *)malloc( sizeof(LogString) );
        copyString(head->outputStr, logString);
        head->next = NULL;
        return head;
    }
    // assume end of list not found yet
    head->next = addLogStrNode(head->next, logString);

    return head;
}

/**
 * @brief Clears LogString linked list.
 *
 * @details Recursively iterates through LogString linked list and
           returns memory to OS from the bottom of the list upward.
 *
 * @param[in] head LogString head pointer
 *
 * @return LogString Head pointer, which should be NULL
 */
LogString *clearLogString(LogString *head)
{
    if(head != NULL)
    {
        if(head->next != NULL)
        {
          clearLogString(head->next);
        }
        free(head);
    }
    return NULL;
}
