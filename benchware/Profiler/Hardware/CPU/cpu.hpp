#pragma once

#include <chrono>

#include "benchware/Profiler/Instrumentor.hpp"


namespace benchware {

class CPUTimer
    {
    public:
        CPUTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }

        ~CPUTimer()
        {
            if (!m_Stopped) Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            uint32_t threadID = (uint32_t)std::hash<std::thread::id>{}(std::this_thread::get_id());
            
            Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID, "CPU" });

            m_Stopped = true;
        }
    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };
}

#define PROFILE_SCOPE_CPU(name) ::benchware::CPUTimer TOKEN_CONCAT(timer, __LINE__)(name)
#define PROFILE_FUNCTION_CPU() PROFILE_SCOPE_CPU(FUNC_SIG)