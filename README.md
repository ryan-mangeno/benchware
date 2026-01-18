# Benchware

Benchware is a high-performance C++ instrumentation and profiling library designed to measure hardware execution across hardware components. 

## Quick Start (macOS)

The included setup script automates the environment configuration, including the retrieval of LibTorch binaries for Apple Silicon and virtual environment initialization
Further windows support will come soon ...

```bash
# Initialize the environment and dependencies
bash ./scripts/macos_setup.sh

# Build the project using all available cores
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.ncpu)
```

## Usage

Benchware uses a session-based profiling model. Wrap your target code in a session to generate a trace file.

### Basic Example

```cpp
#include "benchware/Profiler/Instrumentor.hpp"
#include "benchware/Profiler/Hardware/cpu.hpp"

int main() {
    // Initialize the session
    PROFILE_BEGIN_SESSION("Compute Benchmark");

    {
        // Profile a specific scope (instantiates CPUTimer)
        PROFILE_SCOPE_CPU("Core Logic");
        
        // Your compute-intensive code here
    }

    // Close session and export to results.json
    PROFILE_END_SESSION(); 
}
```

### Automatic Function Profiling

Use `PROFILE_FUNCTION()` to automatically capture the function signature as the scope name:

```cpp
void RunComplexCalculation() {
    PROFILE_FUNCTION(); 
    // Measurement starts here and ends when the function returns
}
```

## Visualizing Results

The instrumentor generates a `results.json` file using the Chrome Trace Event Format for deep inspection of thread activity.

1. Open Google Chrome or Edge
2. Navigate to `chrome://tracing` or use ui.perfetto.dev
3. Drag and drop your `results.json` file into the browser

## Architecture

The system is built on an abstract hardware layer to allow for cross-platform and multi-device profiling.

### Core Components

**Instrumentor**: A thread-safe singleton manager that buffers `ProfileResult` objects in memory to avoid I/O bottlenecks during execution.

**InstrumentationTimer**: The abstract base class that manages the `m_Name` and `m_Stopped` state.

**CPUTimer**: A concrete subclass using `std::chrono::high_resolution_clock`.

### Extending to New Hardware

To profile other hardware (like a GPU or NPU), simply inherit from the base class:

```cpp
namespace benchware {
    class GPUTimer : public InstrumentationTimer {
    public:
        GPUTimer(const char* name) : InstrumentationTimer(name) { 
            /* Hardware start */ 
        }
        
        void Stop() override { 
            // Calculate delta and call Instrumentor::Get().WriteProfile()
        }
    };
}
```

## Repository Configuration

### Dependencies

- C++17 (Required)
- CMake 3.12+
- LibTorch (Handled by setup script)
- Accelerate Framework (macOS BLAS)

## Future Extensions

Planned hardware backends:

- Metal GPU Timer using `MTLTimestamp` API for GPU profiling
- CUDA Timer for NVIDIA GPU profiling
- Vulkan Timer for cross-platform GPU profiling

