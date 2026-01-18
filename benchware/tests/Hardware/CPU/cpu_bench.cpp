#include "benchware/Profiler/Hardware/CPU/cpu.hpp"

#include <torch/torch.h>
#include <iostream>

void run_test() {
    PROFILE_FUNCTION_CPU();
    torch::Tensor tensor = torch::rand({2, 2});
    // check for mps support
    torch::Tensor gpu_tensor = tensor.to(torch::kMPS);
}

int main(void) {
    PROFILE_BEGIN_SESSION("CPU-Benchmark");
    {
        PROFILE_SCOPE_CPU("Tensor Creating Device Usage");
        for ( int i=0 ; i<10000; i++){
            run_test();
        }
    }
    PROFILE_END_SESSION();
}