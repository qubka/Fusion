#include "process_info.h"

#if FUSION_PLATFORM_WINDOWS
#include "psapi.h"
#pragma comment(lib, "psapi.lib")
#include "tlhelp32.h"
#elif FUSION_PLATFORM_LINUX
#include <sys/sysinfo.h>
#include <unistd.h>
#define LINEBUFFLEN 2048
#endif

using namespace fe;

ProcessInfo::ProcessInfo(unsigned int id) : processId{id} {
#if FUSION_PLATFORM_WINDOWS
    // get number of processors
    SYSTEM_INFO lSysInfo;
    GetSystemInfo(&lSysInfo);
    numOfProcessors = lSysInfo.dwNumberOfProcessors;

    // get system time
    FILETIME lFileTime;
    GetSystemTimeAsFileTime(&lFileTime);
    std::memcpy(&prevSystemTime, &lFileTime, sizeof(FILETIME));

    // get amount of time ran in kernel and user mode
    FILETIME lCreationTime, lExitTime, lKernelTime, lUserTime;
    HANDLE lProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (lProcessHandle != NULL) {
        BOOL lSuccess = GetProcessTimes(lProcessHandle, &lCreationTime, &lExitTime, &lKernelTime, &lUserTime);
        if (lSuccess) {
            std::memcpy(&creationTime, &lCreationTime, sizeof(FILETIME));
            std::memcpy(&prevKernelTime, &lKernelTime, sizeof(FILETIME));
            std::memcpy(&prevUserTime, &lUserTime, sizeof(FILETIME));
        }

        CloseHandle(lProcessHandle);
    }

#elif FUSION_PLATFORM_LINUX
    jiffiesPerSecond = sysconf(_SC_CLK_TCK);
    prevSystemTime = 0;
    prevUserTime = 0;
    prevKernelTime = 0;

    // calculate total system time from file /proc/stat,
    // the content is like: cpu 7967 550 4155 489328
    FILE* lpFile = fopen("/proc/stat", "r");
    if (lpFile) {
        // skip unnecessary content
        fscanf(lpFile, "cpu");
        unsigned long long lTime;
        int lValuesToRead = 4;
        for (int i = 0; i < lValuesToRead; ++i) {
            fscanf(lpFile, "%llu", &lTime);
            prevSystemTime += lTime;
        }
        fclose(lpFile);
    }

    // get user mode time, kernel mode time, start time
    // for current process from file /proc/[pid]/stat
    char lFileName[256];
    sprintf(lFileName, "/proc/%d/stat", processId);
    lpFile = fopen(lFileName, "r");
    if (lpFile) {
        // skip unnecessary content
        int lValuesToSkip = 13;
        char lTemp[LINEBUFFLEN];
        for (int i = 0; i < lValuesToSkip; ++i)
            fscanf(lpFile, "%s", lTemp);
        fscanf(lpFile, "%llu %llu", &prevUserTime, &prevKernelTime);

        // skip unnecessary content
        lValuesToSkip = 6;
        for (int i = 0; i < lValuesToSkip; ++i)
            fscanf(lpFile, "%s", lTemp);
        unsigned long long lStartTimeSinceBoot;
        fscanf(lpFile, "%llu", &lStartTimeSinceBoot);
        startTimeSinceBoot = lStartTimeSinceBoot / jiffiesPerSecond;

        fclose(lpFile);
    }
#endif
}

unsigned int ProcessInfo::getProcessId() {
    return processId;
}

unsigned long long ProcessInfo::getProcessUptime() {
    unsigned long long lUptimeInSec = -1;

#if FUSION_PLATFORM_WINDOWS
    FILETIME lCurrTime;
    GetSystemTimeAsFileTime(&lCurrTime);

    ULARGE_INTEGER ulCurrTime;
    std::memcpy(&ulCurrTime, &lCurrTime, sizeof(FILETIME));

    // The FILETIME structure represents the number of 100-nanosecond intervals,
    // so we need to divide by 10 million to get actual seconds
    lUptimeInSec = (ulCurrTime.QuadPart - creationTime.QuadPart) / 10000000;

    return lUptimeInSec;

#elif FUSION_PLATFORM_LINUX
    struct sysinfo lSysinfo;
    int lReturn = sysinfo(&lSysinfo);

    if (lReturn == 0)
        lUptimeInSec = lSysinfo.uptime - startTimeSinceBoot;

#endif
    return lUptimeInSec;
}

double ProcessInfo::getProcessCPUUsage() {
    double lCPUUsage = -1;

#if FUSION_PLATFORM_WINDOWS
    HANDLE lProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (lProcessHandle != NULL) {
        // get current system time
        FILETIME lFileTime;
        GetSystemTimeAsFileTime(&lFileTime);
        ULARGE_INTEGER lCurrSystemTime;
        std::memcpy(&lCurrSystemTime, &lFileTime, sizeof(FILETIME));

        // get amount of time ran in kernel and user mode
        FILETIME lKernelTime, lUserTime;
        BOOL lSuccess = GetProcessTimes(lProcessHandle, &lFileTime, &lFileTime, &lKernelTime, &lUserTime);
        if (lSuccess) {
            ULARGE_INTEGER lCurrKernelTime, lCurrUserTime;
            std::memcpy(&lCurrKernelTime, &lKernelTime, sizeof(FILETIME));
            std::memcpy(&lCurrUserTime, &lUserTime, sizeof(FILETIME));

            // calculate process cpu usage
            ULONGLONG lTotalProcess = (lCurrKernelTime.QuadPart - prevKernelTime.QuadPart) +
                                      (lCurrUserTime.QuadPart - prevUserTime.QuadPart);
            ULONGLONG lTotalSystem = lCurrSystemTime.QuadPart - prevSystemTime.QuadPart;
            if (lTotalSystem > 0)
                lCPUUsage = (lTotalProcess * 100.0) / (lTotalSystem * numOfProcessors);

            // store current time info
            prevSystemTime = lCurrSystemTime;
            prevKernelTime = lCurrKernelTime;
            prevUserTime = lCurrUserTime;
        }

        CloseHandle(lProcessHandle);
    }

#elif FUSION_PLATFORM_LINUX
    unsigned long long lCurrSystemTime = 0;
    unsigned long long lCurrUserTime = 0;
    unsigned long long lCurrKernelTime = 0;

    // calculate total system time from file /proc/stat,
    // the content is like: cpu 7967 550 4155 489328
    FILE* lpFile = fopen("/proc/stat", "r");
    if (lpFile) {
        // skip unnecessary content
        fscanf(lpFile, "cpu");
        unsigned long long lTime;
        int lValuesToRead = 4;
        for (int i = 0; i < lValuesToRead; ++i) {
            fscanf(lpFile, "%llu", &lTime);
            lCurrSystemTime += lTime;
        }
        fclose(lpFile);
    }

    // get user mode and kernel mode time for current
    // process from file /proc/[pid]/stat
    char lFileName[256];
    sprintf(lFileName, "/proc/%d/stat", processId);
    lpFile = fopen(lFileName, "r");
    if (lpFile) {
        // skip unnecessary content
        char lTemp[LINEBUFFLEN];
        int lValuesToSkip = 13;
        for (int i = 0; i < lValuesToSkip; ++i)
            fscanf(lpFile, "%s", lTemp);

        fscanf(lpFile, "%llu %llu", &lCurrUserTime, &lCurrKernelTime);
        fclose(lpFile);
    }

    unsigned long long lTotalProcess = (lCurrUserTime - prevUserTime) + (lCurrKernelTime - prevKernelTime);
    unsigned long long lTotalSystem = lCurrSystemTime - prevSystemTime;
    if (lTotalSystem > 0)
        lCPUUsage = (lTotalProcess * 100.0) / lTotalSystem;

    prevSystemTime = lCurrSystemTime;
    prevUserTime = lCurrUserTime;
    prevKernelTime = lCurrKernelTime;

#endif
    return lCPUUsage;
}

double ProcessInfo::getProcessMemoryUsed() {
    double lMemUsed = -1;

#if FUSION_PLATFORM_WINDOWS
    HANDLE lProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (lProcessHandle != NULL) {
        PROCESS_MEMORY_COUNTERS lPMC;
        BOOL lSuccess = GetProcessMemoryInfo(lProcessHandle, &lPMC, sizeof(lPMC));
        if (lSuccess) {
            // size in MB
            lMemUsed = lPMC.WorkingSetSize / (1024.0 * 1024.0);
        }

        CloseHandle(lProcessHandle);
    }

#elif FUSION_PLATFORM_LINUX
    char lFileName[256];
    sprintf(lFileName, "/proc/%d/status", processId);
    FILE* lpFile = fopen(lFileName, "r");
    char lLineBuf[LINEBUFFLEN];
    if (lpFile) {
        while (fgets(lLineBuf, LINEBUFFLEN, lpFile)) {
            if (0 == strncmp(lLineBuf, "VmRSS:", 6)) {
                char* cursor = lLineBuf + 6;
                /* Get rid of preceding blanks */
                while (!isdigit(*cursor)) {
                    cursor++;
                }
                /* Get rid of following blanks */
                char* lNumString = cursor;
                while (isdigit(*cursor)) {
                    cursor++;
                }
                *cursor = '\0';
                lMemUsed = std::stod(lNumString) / 1024.0;
                break;
            }
        }
        fclose(lpFile);
    }

#endif
    return lMemUsed;
}

unsigned long ProcessInfo::getProcessThreadCount() {
    unsigned long lThreadCnt = -1;

#if FUSION_PLATFORM_WINDOWS
    // get a process list snapshot
    HANDLE lSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    if (lSnapshot != NULL) {
        // initialize the process entry structure
        PROCESSENTRY32 lEntry;
        lEntry.dwSize = sizeof(PROCESSENTRY32);

        // get the first process info
        BOOL lSuccess = Process32First(lSnapshot, &lEntry);
        while (lSuccess) {
            if (lEntry.th32ProcessID == processId) {
                lThreadCnt = lEntry.cntThreads;
                break;
            }
            lSuccess = Process32Next(lSnapshot, &lEntry);
        }

        CloseHandle(lSnapshot);
    }

#elif FUSION_PLATFORM_LINUX
    // get number of threads from file /proc/[pid]/stat
    char lFileName[256];
    sprintf(lFileName, "/proc/%d/stat", processId);
    FILE* lpFile = fopen(lFileName, "r");
    if (lpFile) {
        // skip unnecessary content
        char lTemp[LINEBUFFLEN];
        int lValuesToSkip = 19;
        for (int i = 0; i < lValuesToSkip; ++i)
            fscanf(lpFile, "%s", lTemp);
        fscanf(lpFile, "%lu", &lThreadCnt);
        fclose(lpFile);
    }

#endif
    return lThreadCnt;
}