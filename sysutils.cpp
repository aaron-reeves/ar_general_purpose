/*
sysutils.h/cpp
--------------
Begin: 2019-02-05
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include "sysutils.h"

#ifdef PSAPI_USED
  #include <windows.h>
  #include <stdio.h>
  #include <psapi.h>
#endif

void PrintMemoryInfo( /*DWORD processID*/ ) {
  #ifdef PSAPI_USED
    PROCESS_MEMORY_COUNTERS pmc;

    if ( GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof(pmc)) )  {
      printf( "\tPageFaultCount: %lu\n", pmc.PageFaultCount );
      printf( "\tPeakWorkingSetSize: %lu\n",  pmc.PeakWorkingSetSize );
      printf( "\tWorkingSetSize: %lu\n", pmc.WorkingSetSize );
      printf( "\tQuotaPeakPagedPoolUsage: %lu\n", pmc.QuotaPeakPagedPoolUsage );
      printf( "\tQuotaPagedPoolUsage: %lu\n", pmc.QuotaPagedPoolUsage );
      printf( "\tQuotaPeakNonPagedPoolUsage: %lu\n", pmc.QuotaPeakNonPagedPoolUsage );
      printf( "\tQuotaNonPagedPoolUsage: %lu\n", pmc.QuotaNonPagedPoolUsage );
      printf( "\tPagefileUsage: %lu\n", pmc.PagefileUsage );
      printf( "\tPeakPagefileUsage: %lu\n", pmc.PeakPagefileUsage );
    }
  #endif
}


unsigned long procWorkingSet() {
  unsigned long result = 0;

  #ifdef PSAPI_USED
    PROCESS_MEMORY_COUNTERS pmc;

    if ( GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof(pmc)) )  {
      result = pmc.WorkingSetSize;
    }

  #endif

  return result;
}


unsigned long procPrivateWorkingSet() {
  unsigned long result = 0;

  #ifdef PSAPI_USED
    PROCESS_MEMORY_COUNTERS_EX pmc;

    if ( GetProcessMemoryInfo( GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS *>(&pmc), sizeof(pmc)) )  {
      result = pmc.PrivateUsage;
    }

  #endif

  return result;
}
