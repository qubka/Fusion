#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace fe {
    class ProcessInfo {
    public:
        ProcessInfo(unsigned int processId);
        ~ProcessInfo();

        unsigned int getProcessId();
        unsigned long long getProcessUptime();
        double getProcessCpuUsage();
        double getProcessMemoryUsed();
        unsigned long getProcessThreadCount();

    private:
        unsigned int processId;
#ifdef _WIN32
        int numOfProcessors; // numbre of processors
        ULARGE_INTEGER creationTime; // process creation time
        ULARGE_INTEGER prevSystemTime; // previously measured system time
        ULARGE_INTEGER prevKernelTime; // amount of time ran in kernel mode
        ULARGE_INTEGER prevUserTime; // amount of time ran in user mode
#elif defined(linux)
        long jiffiesPerSecond;
        unsigned long long startTimeSinceBoot;
        unsigned long long prevSystemTime;
        unsigned long long prevUserTime;
        unsigned long long prevKernelTime;
#endif
    };
}