#include "sysutils.h"

#if defined(_WIN32) || defined(WIN32)
  #include <windows.h>
  #include <stdio.h>
  #include <psapi.h>
#endif // _WIN32

void PrintMemoryInfo( /*DWORD processID*/ ) {
  #if defined(_WIN32) || defined(WIN32)
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
  #endif // _WIN32
}


unsigned long procWorkingSet() {
  int result = 0;

  #if defined(_WIN32) || defined(WIN32)
    PROCESS_MEMORY_COUNTERS pmc;

    if ( GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof(pmc)) )  {
      result = pmc.WorkingSetSize;
    }

  #endif // _WIN32

  return result;
}


unsigned long procPrivateWorkingSet() {
  int result = 0;

  #if defined(_WIN32) || defined(WIN32)
    PROCESS_MEMORY_COUNTERS_EX pmc;

    if ( GetProcessMemoryInfo( GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *) &pmc, sizeof(pmc)) )  {
      result = pmc.PrivateUsage;
    }

  #endif // _WIN32

  return result;
}
