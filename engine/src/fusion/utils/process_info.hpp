#pragma once

#ifdef PLATFORM_WINDOWs
#include <windows.h>
#elif PLATFORM_LINUX
#include <unistd.h>
#endif

namespace fe {
    class ProcessInfo {
    public:
        ProcessInfo(uint32_t processId);
        ~ProcessInfo();
        
        uint32_t getProcessId();
        uint64_t getProcessUptime();
        double getProcessCpuUsage();
        double getProcessMemoryUsed();
        uint64_t getProcessThreadCount();

    private:
        uint32_t processId;
#ifdef PLATFORM_WINDOWS
        int numOfProcessors; // numbre of processors
        ULARGE_INTEGER creationTime; // process creation time
        ULARGE_INTEGER prevSystemTime; // previously measured system time
        ULARGE_INTEGER prevKernelTime; // amount of time ran in kernel mode
        ULARGE_INTEGER prevUserTime; // amount of time ran in user mode
#elif PLATFORM_LINUX
        int64_t jiffiesPerSecond;
        uint64_t startTimeSinceBoot;
        uint64_t prevSystemTime;
        uint64_t prevUserTime;
        uint64_t prevKernelTime;
#endif
    };
}