#pragma once
#include <Psapi.h>

class RAMUsage
{
public:
    static void ReadRAM()
    {
        MEMORYSTATUSEX mem{};
        mem.dwLength = sizeof(MEMORYSTATUSEX);

        GlobalMemoryStatusEx(&mem);
        unsigned long long available = mem.ullAvailPhys;
        unsigned long long total = mem.ullTotalPhys;

        // for convertion to MB and GB
        const DWORD dwMBFactor = 0x00100000;

        PROCESS_MEMORY_COUNTERS_EX pcmex{};

        if (!GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pcmex, sizeof(PROCESS_MEMORY_COUNTERS_EX)))
            return;

        if (!GlobalMemoryStatusEx(&mem))
            return;

        {
            AvailableRAM = mem.ullAvailPhys / dwMBFactor;
            TotalRAM = mem.ullTotalPhys / dwMBFactor;
            unsigned long long temp = (mem.ullAvailPhys / dwMBFactor) / (mem.ullTotalPhys / dwMBFactor);
            float available = static_cast<float>(mem.ullAvailPhys / dwMBFactor);
            float total = static_cast<float>(mem.ullTotalPhys / dwMBFactor);
            InUsePercentage = (100.0f - (available / total) * 100.0f);
            MemoryUsed = static_cast<float>(pcmex.PrivateUsage / (1024.0f * 1024.0f));
        }
    }

    static unsigned long long AvailableRAM;
    static unsigned long long TotalRAM;
    static float InUsePercentage;
    static float MemoryUsed;
};

unsigned long long RAMUsage::AvailableRAM = 0;
unsigned long long RAMUsage::TotalRAM = 0;
float RAMUsage::InUsePercentage = 0.0f;
float RAMUsage::MemoryUsed = 0.0f;