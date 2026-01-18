#pragma once

// Usage: include this header file somewhere in your code (eg. precompiled header), and then use like:
//
// Instrumentor::Get().BeginSession("Session Name");  or  PROFILE_BEGIN_SESSION(session_name)     // Begin session 
// {
//     InstrumentationTimer timer("Profiled Scope Name");   // Place code like this in scopes you'd like to include in profiling
//     // Code
// }
// Instrumentor::Get().EndSession();  or PROFILE_END_SESSION()                       // End Session

#include <string>
#include <chrono>
#include <vector>
#include <mutex>
#include <fstream>
#include <thread>
#include <algorithm>


namespace benchware {

    struct ProfileResult
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
        std::string Category; //  "CPU", "GPU", "Render"
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    class Instrumentor
    {
    private:
        InstrumentationSession* m_CurrentSession;
        // In-memory buffer instead of direct file I/O
        std::vector<ProfileResult> m_Results; 
        std::mutex m_Lock; // Thread safety
        
    public:
        Instrumentor()
            : m_CurrentSession(nullptr)
        {
            // Reserve space to avoid reallocs during profiling
            m_Results.reserve(100000); 
        }

        void BeginSession(const std::string& name)
        {
            std::lock_guard<std::mutex> lock(m_Lock);
            m_CurrentSession = new InstrumentationSession{ name };
            m_Results.clear();
        }

        void EndSession(const std::string& filepath = "results.json")
        {
            std::lock_guard<std::mutex> lock(m_Lock);
            if (!m_CurrentSession) return;
            
            WriteToFile(filepath);
            
            delete m_CurrentSession;
            m_CurrentSession = nullptr;
        }

        // Accepts data from any hardware source (CPU, GPU, etc.)
        void WriteProfile(const ProfileResult& result)
        {
            std::lock_guard<std::mutex> lock(m_Lock);
            if (m_CurrentSession) {
                m_Results.push_back(result);
            }
        }

        static Instrumentor& Get()
        {
            static Instrumentor instance;
            return instance;
        }

    private:
        void WriteToFile(const std::string& filepath)
        {
            std::ofstream stream(filepath);
            stream << "{\"otherData\": {},\"traceEvents\":[";

            for (size_t i = 0; i < m_Results.size(); i++)
            {
                auto& result = m_Results[i];
                std::string name = result.Name;
                std::replace(name.begin(), name.end(), '"', '\'');

                if (i > 0) stream << ",";

                stream << "{";
                stream << "\"cat\":\"" << result.Category << "\","; // Use the category
                stream << "\"dur\":" << (result.End - result.Start) << ',';
                stream << "\"name\":\"" << name << "\",";
                stream << "\"ph\":\"X\",";
                stream << "\"pid\":0,";
                stream << "\"tid\":" << result.ThreadID << ",";
                stream << "\"ts\":" << result.Start;
                stream << "}";
            }

            stream << "]}";
            stream.flush();
            stream.close();
        }
    };
}


#define PROFILE_BEGIN_SESSION(name)  ::benchware::Instrumentor::Get().BeginSession(name)
#define PROFILE_END_SESSION()  ::benchware::Instrumentor::Get().EndSession()

#if defined(_MSC_VER) 
    #define FUNC_SIG __FUNCSIG__
#else
    #define FUNC_SIG __func__
#endif

#define TOKEN_CONCAT_IMPL(x, y) x ## y
#define TOKEN_CONCAT(x, y) TOKEN_CONCAT_IMPL(x, y)
