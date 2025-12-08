#pragma once

namespace eerie_leap::utilities::dev_tools {

class SystemInfo {
public:
    /** @brief Run the thread analyzer and print stack size statistics.
     *
     *  This function runs the thread analyzer and prints the output in
     *  standard form. In the special case when Kconfig option
     *  THREAD_ANALYZER_AUTO_SEPARATE_CORES is set, the function analyzes
     *  only the threads running on the specified cpu.
     *
     *  @param cpu cpu to analyze, ignored if THREAD_ANALYZER_AUTO_SEPARATE_CORES=n
     */
    static void PrintThreadInfo(int cpu = 0);
    static void PrintStackInfo(int cpu = 0, const char *thread_name = nullptr);
    static void PrintCpuInfo(int cpu = 0, const char *thread_name = nullptr);
    static void PrintHeapInfo();
};

} // namespace eerie_leap::utilities::dev_tools
