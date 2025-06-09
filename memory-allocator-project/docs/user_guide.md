# Memory Allocator User Guide

## Table of Contents
1. [Quick Start](#quick-start)
2. [Building the Project](#building-the-project)
3. [Using the Allocators](#using-the-allocators)
4. [Web Demo Interface](#web-demo-interface)
5. [Testing and Benchmarking](#testing-and-benchmarking)
6. [Configuration Options](#configuration-options)
7. [Troubleshooting](#troubleshooting)

## Quick Start

### Prerequisites
- **C++ Compiler**: GCC 9.0+ or MSVC 2019+
- **Operating System**: Windows 10+, Linux Ubuntu 18.04+, macOS 10.15+
- **Memory**: 4GB RAM minimum, 8GB recommended
- **Web Browser**: Chrome, Firefox, Safari, or Edge for demo interface

### 1-Minute Setup
```bash
# Clone or download the project
cd memory-allocator-project

# Build the project
make all

# Run basic tests
./bin/memory_allocator

# Open web demo
# Navigate to demo/index.html in your browser
```

## Building the Project

### Windows (Command Prompt)
```cmd
# Using MinGW-w64 or MSYS2
make all

# Or build individual components
make main
make tests
make performance
```

### Linux/macOS (Terminal)
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential

# Build project
make all

# Build with debug symbols
make DEBUG=1 all

# Clean build files
make clean
```

### Build Targets
```bash
make main          # Main testing program
make tests          # Unit test suite
make performance    # Performance benchmarks
make all           # All executables
make clean         # Remove build files
```

### Compiler Options
The Makefile includes optimized compiler flags:
```makefile
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread
```

For debugging, add `-g -DDEBUG`:
```bash
make DEBUG=1 all
```

## Using the Allocators

### Basic Usage Pattern

#### 1. Include Headers
```cpp
#include "includes/memory_allocator.h"
#include "includes/buddy_allocator.h"
#include "includes/slab_allocator.h"
#include "includes/pool_allocator.h"
#include "includes/hybrid_allocator.h"
```

#### 2. Create Allocator Instance
```cpp
// Buddy allocator with 1MB memory
auto buddy = std::make_unique<BuddyAllocator>(1024 * 1024);

// Slab allocator with 512KB memory
auto slab = std::make_unique<SlabAllocator>(512 * 1024);

// Pool allocator with specific configuration
PoolAllocator::PoolConfig config;
config.block_sizes = {32, 64, 128, 256, 512};
config.blocks_per_pool = {1000, 800, 600, 400, 200};
auto pool = std::make_unique<PoolAllocator>(config);

// Hybrid allocator with default settings
auto hybrid = std::make_unique<HybridAllocator>(2 * 1024 * 1024);
```

#### 3. Allocate and Deallocate Memory
```cpp
// Allocate memory
void* ptr1 = allocator->allocate(256);
void* ptr2 = allocator->allocate(1024);

// Use the memory
if (ptr1 && ptr2) {
    memset(ptr1, 0, 256);
    memset(ptr2, 0, 1024);
}

// Deallocate memory
allocator->deallocate(ptr1);
allocator->deallocate(ptr2);
```

#### 4. Monitor Performance
```cpp
// Get statistics
std::string stats = allocator->getStats();
std::cout << stats << std::endl;

// Check fragmentation
size_t fragmentation = allocator->getFragmentation();
std::cout << "Fragmentation: " << fragmentation << "%" << std::endl;

// Get memory layout
auto layout = allocator->getMemoryLayout();
for (const auto& block : layout) {
    std::cout << "Block at " << block.offset 
              << ", size " << block.size 
              << ", " << (block.is_free ? "free" : "used") 
              << std::endl;
}
```

### Allocator-Specific Usage

#### Buddy System Allocator
```cpp
// Best for: Variable-size allocations, general-purpose use
BuddyAllocator buddy(1024 * 1024, 64); // 1MB total, 64-byte minimum

// Allocations are rounded up to power of 2
void* ptr = buddy.allocate(100);  // Actually allocates 128 bytes
buddy.deallocate(ptr);

// Check tree structure
auto layout = buddy.getMemoryLayout();
// Each block represents a node in the binary tree
```

#### Slab Allocator
```cpp
// Best for: Fixed-size objects, cache-friendly allocation
SlabAllocator slab(512 * 1024, 4096); // 512KB total, 4KB slabs

// Create slabs for specific object sizes
void* obj1 = slab.allocate(64);   // Creates 64-byte object slab
void* obj2 = slab.allocate(128);  // Creates 128-byte object slab
void* obj3 = slab.allocate(64);   // Reuses existing 64-byte slab

slab.deallocate(obj1);
slab.deallocate(obj2);
slab.deallocate(obj3);
```

#### Pool Allocator
```cpp
// Best for: Predictable allocation patterns, real-time systems
PoolAllocator::PoolConfig config;
config.block_sizes = {32, 64, 128, 256};
config.blocks_per_pool = {1000, 500, 250, 125};
PoolAllocator pool(config);

// O(1) allocation from pre-allocated pools
void* ptr32 = pool.allocate(32);   // From 32-byte pool
void* ptr64 = pool.allocate(64);   // From 64-byte pool
void* ptr100 = pool.allocate(100); // From 128-byte pool (rounded up)

pool.deallocate(ptr32);
pool.deallocate(ptr64);
pool.deallocate(ptr100);
```

#### Hybrid Allocator
```cpp
// Best for: Mixed workloads, general applications
HybridAllocator::HybridConfig config;
config.pool_memory_ratio = 0.3;    // 30% for pools
config.slab_memory_ratio = 0.3;    // 30% for slabs
config.pool_max_size = 256;        // Pool threshold
config.slab_max_size = 1024;       // Slab threshold

HybridAllocator hybrid(2 * 1024 * 1024, config);

// Automatically selects best allocator based on size
void* small = hybrid.allocate(64);    // Uses pool allocator
void* medium = hybrid.allocate(512);  // Uses slab allocator
void* large = hybrid.allocate(2048);  // Uses buddy allocator

hybrid.deallocate(small);
hybrid.deallocate(medium);
hybrid.deallocate(large);

// Monitor efficiency
double efficiency = hybrid.getEfficiencyScore();
std::cout << "Efficiency: " << efficiency << std::endl;
```

### Advanced Usage Patterns

#### Custom Allocation Patterns
```cpp
// Simulate web server request handling
void simulateWebServer(MemoryAllocator& allocator) {
    std::vector<void*> request_buffers;
    
    // Allocate request buffers
    for (int i = 0; i < 1000; ++i) {
        size_t size = 64 + (rand() % 448); // 64-512 bytes
        void* buffer = allocator.allocate(size);
        if (buffer) {
            request_buffers.push_back(buffer);
        }
    }
    
    // Process requests (use memory)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Free buffers
    for (void* buffer : request_buffers) {
        allocator.deallocate(buffer);
    }
}
```

#### Memory Pressure Testing
```cpp
void memoryPressureTest(MemoryAllocator& allocator) {
    std::vector<void*> allocations;
    size_t allocation_size = 1024;
    
    // Allocate until memory is exhausted
    while (true) {
        void* ptr = allocator.allocate(allocation_size);
        if (!ptr) {
            std::cout << "Memory exhausted after " 
                      << allocations.size() << " allocations" << std::endl;
            break;
        }
        allocations.push_back(ptr);
    }
    
    // Free half the allocations
    for (size_t i = 0; i < allocations.size() / 2; ++i) {
        allocator.deallocate(allocations[i]);
    }
    
    // Try to allocate again
    void* ptr = allocator.allocate(allocation_size);
    if (ptr) {
        std::cout << "Successfully allocated after freeing" << std::endl;
        allocator.deallocate(ptr);
    }
    
    // Clean up remaining allocations
    for (size_t i = allocations.size() / 2; i < allocations.size(); ++i) {
        allocator.deallocate(allocations[i]);
    }
}
```

## Web Demo Interface

### Accessing the Demo
1. **Local File**: Open `demo/index.html` directly in your browser
2. **Local Server**: For full functionality, serve via HTTP:
   ```bash
   # Python 3
   cd demo && python -m http.server 8000
   
   # Node.js (if you have live-server)
   cd demo && npx live-server
   
   # Then visit http://localhost:8000
   ```

### Demo Features

#### Algorithm Comparison Tab
- **Interactive Controls**: Select allocator type, memory size, allocation pattern
- **Real-time Visualization**: Memory layout, allocation timeline
- **Performance Metrics**: Speed, fragmentation, efficiency scores
- **Comparative Analysis**: Side-by-side allocator comparison

#### Performance Benchmarks Tab
- **Benchmark Selection**: Choose from predefined test scenarios
- **Custom Parameters**: Adjust test parameters (iterations, sizes, patterns)
- **Results Display**: Charts showing performance metrics over time
- **Export Function**: Download results as CSV or JSON

#### Memory Visualization Tab
- **Live Memory Map**: Real-time visualization of memory state
- **Allocation Animation**: Watch allocations and deallocations in real-time
- **Fragmentation View**: Visual representation of memory fragmentation
- **Statistics Panel**: Detailed metrics and efficiency scores

#### Algorithm Education Tab
- **Interactive Tutorials**: Step-by-step algorithm explanations
- **Visual Demonstrations**: Animated algorithm execution
- **Code Examples**: Sample usage patterns with syntax highlighting
- **Concept Explanations**: Theory behind each allocation strategy

### Demo Controls

#### Basic Operations
```javascript
// The demo interface provides these controls:
- Memory Size Slider: 1KB to 10MB
- Allocation Size: 8 bytes to 8KB
- Pattern Selection: Sequential, Random, Mixed
- Speed Control: Animation speed adjustment
- Reset Button: Clear all allocations
```

#### Advanced Features
```javascript
// Configuration options
- Hybrid Config: Adjust thresholds and ratios
- Pool Config: Set block sizes and counts
- Slab Config: Configure slab size and object sizes
- Performance Monitoring: Enable/disable real-time stats
```

## Testing and Benchmarking

### Unit Tests
```bash
# Run all unit tests
./bin/unit_tests

# Run specific test category
./bin/unit_tests buddy
./bin/unit_tests slab
./bin/unit_tests pool
./bin/unit_tests hybrid
```

### Performance Tests
```bash
# Run performance benchmarks
./bin/performance_tests

# Run specific benchmark
./bin/performance_tests allocation_speed
./bin/performance_tests fragmentation
./bin/performance_tests threading
```

### Custom Testing
```cpp
// Create custom test scenarios
#include "tests/test_framework.h"

void customTest() {
    TestFramework framework;
    
    // Test specific allocation pattern
    auto allocator = std::make_unique<HybridAllocator>(1024 * 1024);
    
    framework.testAllocationPattern(
        *allocator,
        "Custom Pattern",
        {64, 128, 256, 512}, // Allocation sizes
        1000,                // Number of allocations
        0.5                  // Deallocation probability
    );
    
    framework.printResults();
}
```

## Configuration Options

### Buddy Allocator Configuration
```cpp
BuddyAllocator(
    size_t memory_size,     // Total memory (must be power of 2)
    size_t min_block_size   // Minimum allocation size (default: 64)
);

// Example configurations:
BuddyAllocator small(64 * 1024, 32);      // 64KB, 32-byte minimum
BuddyAllocator medium(1024 * 1024, 64);   // 1MB, 64-byte minimum
BuddyAllocator large(16 * 1024 * 1024, 128); // 16MB, 128-byte minimum
```

### Slab Allocator Configuration
```cpp
SlabAllocator(
    size_t memory_size,  // Total memory available
    size_t slab_size     // Size of each slab (default: 4096)
);

// Example configurations:
SlabAllocator kernel(512 * 1024, 4096);   // Kernel-style, 4KB slabs
SlabAllocator cache(1024 * 1024, 8192);   // Cache-friendly, 8KB slabs
SlabAllocator embedded(128 * 1024, 1024); // Embedded, 1KB slabs
```

### Pool Allocator Configuration
```cpp
struct PoolConfig {
    std::vector<size_t> block_sizes;      // Available block sizes
    std::vector<size_t> blocks_per_pool;  // Number of blocks per size
};

// Example configurations:
// Real-time system
PoolConfig realtime_config;
realtime_config.block_sizes = {32, 64, 128, 256, 512, 1024};
realtime_config.blocks_per_pool = {2000, 1500, 1000, 500, 250, 100};

// Memory-constrained system
PoolConfig embedded_config;
embedded_config.block_sizes = {16, 32, 64, 128};
embedded_config.blocks_per_pool = {100, 80, 60, 40};

// High-performance system
PoolConfig performance_config;
performance_config.block_sizes = {64, 128, 256, 512, 1024, 2048};
performance_config.blocks_per_pool = {5000, 4000, 3000, 2000, 1000, 500};
```

### Hybrid Allocator Configuration
```cpp
struct HybridConfig {
    double pool_memory_ratio;  // Fraction of memory for pools (0.0-1.0)
    double slab_memory_ratio;  // Fraction of memory for slabs (0.0-1.0)
    size_t pool_max_size;      // Maximum size for pool allocation
    size_t slab_max_size;      // Maximum size for slab allocation
};

// Example configurations:
// Web server optimized
HybridConfig web_config = {
    .pool_memory_ratio = 0.4,
    .slab_memory_ratio = 0.3,
    .pool_max_size = 512,
    .slab_max_size = 2048
};

// Game engine optimized
HybridConfig game_config = {
    .pool_memory_ratio = 0.5,
    .slab_memory_ratio = 0.2,
    .pool_max_size = 1024,
    .slab_max_size = 4096
};

// Database optimized
HybridConfig db_config = {
    .pool_memory_ratio = 0.6,
    .slab_memory_ratio = 0.2,
    .pool_max_size = 4096,
    .slab_max_size = 8192
};
```

## Troubleshooting

### Common Build Issues

#### "Command not found: make"
```bash
# Windows: Install MinGW-w64 or MSYS2
# Linux: sudo apt-get install build-essential
# macOS: xcode-select --install
```

#### "undefined reference to pthread"
```bash
# Add -pthread flag to compilation
CXXFLAGS += -pthread
```

#### "error: 'aligned_alloc' was not declared"
```cpp
// For older compilers, use alternative:
#ifdef _WIN32
    #include <malloc.h>
    #define aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
    #define aligned_free(ptr) _aligned_free(ptr)
#else
    #include <stdlib.h>
#endif
```

### Runtime Issues

#### Segmentation Fault
```cpp
// Common causes and solutions:
1. Double deallocation:
   - Check that each pointer is deallocated only once
   - Set pointers to nullptr after deallocation

2. Use after free:
   - Don't access memory after deallocating it
   - Use debugging tools like Valgrind or AddressSanitizer

3. Buffer overrun:
   - Ensure writes don't exceed allocated size
   - Use bounds checking in debug builds
```

#### Memory Leaks
```cpp
// Detection and prevention:
1. Use RAII patterns:
   std::unique_ptr<char[]> buffer(
       static_cast<char*>(allocator->allocate(size))
   );

2. Enable debug mode:
   make DEBUG=1 all

3. Use memory debugging tools:
   valgrind --leak-check=full ./bin/memory_allocator
```

#### Performance Issues
```cpp
// Optimization strategies:
1. Choose appropriate allocator:
   - Small, frequent: Pool allocator
   - Fixed sizes: Slab allocator
   - Variable sizes: Buddy system
   - Mixed workload: Hybrid allocator

2. Configure for your workload:
   - Analyze allocation patterns
   - Adjust thresholds and ratios
   - Monitor fragmentation levels

3. Enable compiler optimizations:
   make CXXFLAGS="-O3 -march=native" all
```

### Web Demo Issues

#### Demo not loading
```javascript
// Solutions:
1. Check browser console for errors
2. Serve via HTTP server instead of file://
3. Update browser to latest version
4. Try different browser (Chrome, Firefox, Safari)
```

#### Visualization not working
```javascript
// Check browser compatibility:
- Canvas support required
- JavaScript enabled
- Modern browser (ES6+ support)
```

#### Performance slow in demo
```javascript
// Optimization:
1. Reduce visualization complexity
2. Lower animation frame rate
3. Decrease memory size in demo
4. Close other browser tabs
```

### Getting Help

#### Documentation
- **Design Document**: `docs/design_document.md`
- **Algorithm Details**: `docs/algorithms.md`
- **Performance Analysis**: `docs/performance_analysis.md`
- **Source Code**: Well-commented headers in `src/includes/`

#### Debugging Tips
1. **Enable Debug Mode**: `make DEBUG=1 all`
2. **Use Static Analysis**: Run with `-Wall -Wextra -Wpedantic`
3. **Memory Tools**: Valgrind, AddressSanitizer, Dr. Memory
4. **Profiling**: gprof, Intel VTune, perf

#### Common Questions

**Q: Which allocator should I use?**
A: Depends on your use case:
- **Unknown/Mixed patterns**: Hybrid allocator
- **Real-time systems**: Pool allocator
- **Object-oriented code**: Slab allocator
- **General purpose**: Buddy system

**Q: How do I optimize for my specific workload?**
A: 
1. Profile your allocation patterns
2. Run benchmarks with different configurations
3. Monitor fragmentation and efficiency metrics
4. Adjust allocator parameters based on results

**Q: Can I use these allocators in production?**
A: These implementations are educational and may need additional features for production use:
- Error handling and recovery
- Security hardening
- Advanced debugging support
- Platform-specific optimizations

This user guide provides comprehensive information for using the memory allocator project effectively. For additional support, refer to the source code comments and documentation files.
