#ifndef OS_SIMULATOR_H
#define OS_SIMULATOR_H

// header files
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "StringUtils.h"
#include "simtimer.h"
#include "ConfigAccess.h"
#include "MetaDataAccess.h"
#include "SimUtils.h"


// function prototypes
void startSimulator( ConfigDataType *configData, OpCodeType *opCodeHead);

#endif  // OS_SIMULATOR_H

