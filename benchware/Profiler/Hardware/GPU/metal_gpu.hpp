#ifdef BW_PLATFORM_APPLE

#pragma once
#include "benchware/Profiler/Instrumentor.hpp"

namespace benchware {

    class MetalContext {

        static MetalContext& GetCtx() noexcept {
            static MetalContext s_Ctx;
            return s_Ctx;
        }

        bool SetCurCommandBuffer();
        void Record(const char* name);
        bool Init();
        
        private:
            void* m_CommandQueue;
            void* m_CurCommandBuffer;
            void* m_Device;

        private:
            MetalContext();
    };

    class MetalTimer {
        public:    
            MetalTimer();
            ~MetalTimer();
    };
}

#endif