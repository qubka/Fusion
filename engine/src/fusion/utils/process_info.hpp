#pragma once

#if FUSION_PLATFORM_WINDOWS
#include <Windows.h>
#elif FUSION_PLATFORM_LINUX
#include <unistd.h>
#endif

namespace fe {
    class ProcessInfo {
    public:
        ProcessInfo(unsigned int processId);
        ~ProcessInfo();

        unsigned int getProcessId();
        unsigned long long getProcessUptime();
        double getProcessCPUUsage();
        double getProcessMemoryUsed();
        unsigned long getProcessThreadCount();

    private:
        unsigned int mProcessId;
#if FUSION_PLATFORM_WINDOWS
        int mNumOfProcessors; // numbre of processors
        ULARGE_INTEGER mCreationTime; // process creation time
        ULARGE_INTEGER mPrevSystemTime; // previously measured system time
        ULARGE_INTEGER mPrevKernelTime; // amount of time ran in kernel mode
        ULARGE_INTEGER mPrevUserTime; // amount of time ran in user mode
#elif FUSION_PLATFORM_LINUX
        long mJiffiesPerSecond;
        unsigned long long mStartTimeSinceBoot;
        unsigned long long mPrevSystemTime;
        unsigned long long mPrevUserTime;
        unsigned long long mPrevKernelTime;
#endif
    };
}