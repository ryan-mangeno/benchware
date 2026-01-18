#include "metal_gpu.hpp"


#import <Metal/Metal.h>

namespace benchware {

    bool MetalContext::Init() {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        m_Device = (__bridge_retained void*)device;

        if (m_Device) return false;

        id<MTLCommandQueue> commandQueue = [device newCommandQueue];
        m_CommandQueue = (__bridge_retained void*)commandQueue;

        if (!m_CommandQueue) return false;

        return true;
    }

    bool MetalContext::SetCurCommandBuffer() {
        id<MTLDevice> device = (__bridge id<MTLDevice>)m_Device;
        if (!device) return false;

        id<MTLCommandQueue> queue = (__bridge id<MTLCommandQueue>)m_CommandQueue;
        if (!queue) return false;

        MTLCommandBufferDescriptor *desc = [MTLCommandBufferDescriptor new];
        if (!desc) return false;
        desc.errorOptions = MTLCommandBufferErrorOptionEncoderExecutionStatus;

        id<MTLCommandBuffer> buf = [queue commandBufferWithDescriptor:desc];
        if (!buf) return false;
        m_CurCommandBuffer = (__bridge void*)buf;

        return true;
    }
    
    void MetalContext::Record(const char* name) {
        id<MTLCommandBuffer> cmdBuffer = (__bridge id<MTLCommandBuffer>)m_CurCommandBuffer;
        [cmdBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
            // extract hardware timestamps 
            double start = buffer.GPUStartTime;
            double end = buffer.GPUEndTime;

            // record to the Instrumentor
            Instrumentor::Get().WriteProfile({
                name,
                static_cast<long long>(start * 1e6), // Convert seconds to microseconds
                static_cast<long long>(end * 1e6),
                0,
                "GPU"
            });
        }];
    }

    MetalTimer::MetalTimer() {
    }
}