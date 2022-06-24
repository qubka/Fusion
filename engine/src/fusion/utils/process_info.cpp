#include "process_info.hpp"

#ifdef PLATFORM_WINDOWS
#include "psapi.h"
#pragma comment(lib, "psapi.lib")
#include "tlhelp32.h"
#elif PLATFORM_LINUX
#include <sys/sysinfo.h>
#include <unistd.h>
#define LINEBUFFLEN 2048
#endif

using namespace fe;

ProcessInfo::ProcessInfo(uint32_t processId) : processId{processId} {
#ifdef _WIN32
    // get number of processors
	SYSTEM_INFO lSysInfo;
	GetSystemInfo(&lSysInfo);
	mNumOfProcessors =  lSysInfo.dwNumberOfProcessors;

	// get system time
	FILETIME lFileTime;
	GetSystemTimeAsFileTime(&lFileTime);
	memcpy(&mPrevSystemTime, &lFileTime, sizeof(FILETIME));

	// get amount of time ran in kernel and user mode
	FILETIME lCreationTime, lExitTime, lKernelTime, lUserTime;
	HANDLE lProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, mProcessId);
	if (lProcessHandle != NULL)
	{
		BOOL lSuccess = GetProcessTimes(lProcessHandle, &lCreationTime, &lExitTime, &lKernelTime, &lUserTime);
		if (lSuccess)
		{
			memcpy(&mCreationTime, &lCreationTime, sizeof(FILETIME));
			memcpy(&mPrevKernelTime, &lKernelTime, sizeof(FILETIME));
			memcpy(&mPrevUserTime, &lUserTime, sizeof(FILETIME));
		}

		CloseHandle(lProcessHandle);
	}

#elif PLATFORM_LINUX
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
        uint64_t lTime;
        int lValuesToRead = 4;
        for (int i = 0; i < lValuesToRead; i++) {
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
        for (int i = 0; i < lValuesToSkip; i++)
            fscanf(lpFile, "%s", lTemp);
        fscanf(lpFile, "%llu %llu", &prevUserTime, &prevKernelTime);

        // skip unnecessary content
        lValuesToSkip = 6;
        for (int i = 0; i < lValuesToSkip; i++)
            fscanf(lpFile, "%s", lTemp);
        uint64_t lStartTimeSinceBoot;
        fscanf(lpFile, "%llu", &lStartTimeSinceBoot);
        startTimeSinceBoot = lStartTimeSinceBoot / jiffiesPerSecond;

        fclose(lpFile);
    }
#endif
}

ProcessInfo::~ProcessInfo() {
}

uint32_t ProcessInfo::getProcessId() {
    return processId;
}

uint64_t ProcessInfo::getProcessUptime() {
    uint64_t lUptimeInSec = -1;

#ifdef _WIN32
    FILETIME lCurrTime;
	GetSystemTimeAsFileTime(&lCurrTime);

	ULARGE_INTEGER ulCurrTime;
	memcpy(&ulCurrTime, &lCurrTime, sizeof(FILETIME));

	// The FILETIME structure represents the number of 100-nanosecond intervals,
	// so we need to divide by 10 million to get actual seconds
	lUptimeInSec = (ulCurrTime.QuadPart - mCreationTime.QuadPart) / 10000000;

	return lUptimeInSec;

#elif PLATFORM_LINUX
    struct sysinfo lSysinfo;
    int lReturn = sysinfo(&lSysinfo);

    if (lReturn == 0)
        lUptimeInSec = lSysinfo.uptime - startTimeSinceBoot;

#endif
    return lUptimeInSec;
}

double ProcessInfo::getProcessCpuUsage() {
    double lCPUUsage = -1;
#ifdef _WIN32
    HANDLE lProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, mProcessId);
	if (lProcessHandle != NULL) {
		// get current system time
		FILETIME lFileTime;
		GetSystemTimeAsFileTime(&lFileTime);
		ULARGE_INTEGER lCurrSystemTime;
		memcpy(&lCurrSystemTime, &lFileTime, sizeof(FILETIME));

		// get amount of time ran in kernel and user mode
		FILETIME lKernelTime, lUserTime;
		BOOL lSuccess = GetProcessTimes(lProcessHandle, &lFileTime, &lFileTime, &lKernelTime, &lUserTime);
		if (lSuccess) {
			ULARGE_INTEGER lCurrKernelTime, lCurrUserTime;
			memcpy(&lCurrKernelTime, &lKernelTime, sizeof(FILETIME));
			memcpy(&lCurrUserTime, &lUserTime, sizeof(FILETIME));

			// calculate process cpu usage
			ULONGLONG lTotalProcess = (lCurrKernelTime.QuadPart - mPrevKernelTime.QuadPart) + (lCurrUserTime.QuadPart - mPrevUserTime.QuadPart);
			ULONGLONG lTotalSystem = lCurrSystemTime.QuadPart - mPrevSystemTime.QuadPart;
			if (lTotalSystem > 0)
				lCPUUsage = (lTotalProcess * 100.0) / (lTotalSystem * mNumOfProcessors);

			// store current time info
			mPrevSystemTime = lCurrSystemTime;
			mPrevKernelTime = lCurrKernelTime;
			mPrevUserTime = lCurrUserTime;
		}

		CloseHandle(lProcessHandle);
	}

#elif PLATFORM_LINUX
    uint64_t lCurrSystemTime = 0;
    uint64_t lCurrUserTime = 0;
    uint64_t lCurrKernelTime = 0;

    // calculate total system time from file /proc/stat,
    // the content is like: cpu 7967 550 4155 489328
    FILE* lpFile = fopen("/proc/stat", "r");
    if (lpFile) {
        // skip unnecessary content
        fscanf(lpFile, "cpu");
        uint64_t lTime;
        int lValuesToRead = 4;
        for (int i = 0; i < lValuesToRead; i++) {
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
        for (int i = 0; i < lValuesToSkip; i++)
            fscanf(lpFile, "%s", lTemp);

        fscanf(lpFile, "%llu %llu", &lCurrUserTime, &lCurrKernelTime);
        fclose(lpFile);
    }

    uint64_t lTotalProcess = (lCurrUserTime - prevUserTime) + (lCurrKernelTime - prevKernelTime);
    uint64_t lTotalSystem = lCurrSystemTime - prevSystemTime;
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

#ifdef _WIN32
    HANDLE lProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, mProcessId);
	if (lProcessHandle != NULL) {
		PROCESS_MEMORY_COUNTERS lPMC;
		BOOL lSuccess = GetProcessMemoryInfo(lProcessHandle, &lPMC, sizeof(lPMC));
		if (lSuccess) {
			// size in MB
			lMemUsed = lPMC.WorkingSetSize / (1024.0 * 1024.0);
		}

		CloseHandle(lProcessHandle);
	}

#elif PLATFORM_LINUX
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

uint64_t ProcessInfo::getProcessThreadCount() {
    uint64_t lThreadCnt = -1;

#ifdef _WIN32
    // get a process list snapshot
	HANDLE lSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (lSnapshot != NULL) {
		// initialize the process entry structure
		PROCESSENTRY32 lEntry;
		lEntry.dwSize = sizeof(PROCESSENTRY32);

		// get the first process info
		BOOL lSuccess = Process32First(lSnapshot, &lEntry);
		while (lSuccess) {
			if (lEntry.th32ProcessID == mProcessId) {
				lThreadCnt = lEntry.cntThreads;
				break;
			}
			lSuccess = Process32Next(lSnapshot, &lEntry);
		}

		CloseHandle(lSnapshot);
	}

#elif PLATFORM_LINUX
    // get number of threads from file /proc/[pid]/stat
    char lFileName[256];
    sprintf(lFileName, "/proc/%d/stat", processId);
    FILE* lpFile = fopen(lFileName, "r");
    if (lpFile) {
        // skip unnecessary content
        char lTemp[LINEBUFFLEN];
        int lValuesToSkip = 19;
        for (int i = 0; i < lValuesToSkip; i++)
            fscanf(lpFile, "%s", lTemp);
        fscanf(lpFile, "%lu", &lThreadCnt);
        fclose(lpFile);
    }
#endif
    return lThreadCnt;
}