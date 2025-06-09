#include "../src/includes/buddy_allocator.h"
#include "../src/includes/slab_allocator.h"
#include "../src/includes/pool_allocator.h"
#include "../src/includes/hybrid_allocator.h"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>

struct BenchmarkResult {
    std::string allocator_name;
    double allocation_time_ms;
    double deallocation_time_ms;
    double total_time_ms;
    size_t successful_allocations;
    size_t total_attempts;
    double success_rate;
    size_t fragmentation;
    double throughput_ops_per_sec;
};

class MemoryAllocatorBenchmark {
public:
    static void runComprehensiveBenchmarks() {
        std::cout << "Memory Allocator Performance Benchmark Suite\n";
        std::cout << "============================================\n\n";
        
        // Different test scenarios
        runFixedSizeBenchmark();
        runVariableSizeBenchmark();
        runFragmentationBenchmark();
        runStressBenchmark();
        runRealWorldSimulation();
        
        std::cout << "\nBenchmark suite completed!\n";
    }

private:
    static void runFixedSizeBenchmark() {
        std::cout << "1. Fixed Size Allocation Benchmark (64 bytes)\n";
        std::cout << "----------------------------------------------\n";
        
        const size_t iterations = 50000;
        const size_t alloc_size = 64;
        const size_t memory_size = 1024 * 1024; // 1MB
        
        std::vector<BenchmarkResult> results;
        
        // Benchmark Buddy Allocator
        results.push_back(benchmarkBuddyAllocator("Buddy", memory_size, iterations, alloc_size));
        
        // Benchmark Slab Allocator
        results.push_back(benchmarkSlabAllocator("Slab", alloc_size, 64, memory_size, iterations, alloc_size));
        
        // Benchmark Pool Allocator
        results.push_back(benchmarkPoolAllocator("Pool", alloc_size, iterations, memory_size, iterations, alloc_size));
        
        // Benchmark Hybrid Allocator
        results.push_back(benchmarkHybridAllocator("Hybrid", memory_size, iterations, alloc_size));
        
        printBenchmarkResults(results);
        std::cout << "\n";
    }
    
    static void runVariableSizeBenchmark() {
        std::cout << "2. Variable Size Allocation Benchmark (16-2048 bytes)\n";
        std::cout << "----------------------------------------------------\n";
        
        const size_t iterations = 25000;
        const size_t memory_size = 2 * 1024 * 1024; // 2MB
        
        std::vector<BenchmarkResult> results;
        
        // Generate random sizes
        std::vector<size_t> sizes = generateRandomSizes(iterations, 16, 2048);
        
        // Benchmark allocators with variable sizes
        results.push_back(benchmarkBuddyAllocatorVariable("Buddy", memory_size, sizes));
        results.push_back(benchmarkHybridAllocatorVariable("Hybrid", memory_size, sizes));
        
        printBenchmarkResults(results);
        std::cout << "\n";
    }
    
    static void runFragmentationBenchmark() {
        std::cout << "3. Fragmentation Test\n";
        std::cout << "--------------------\n";
        
        const size_t memory_size = 1024 * 1024; // 1MB
        
        // Test fragmentation with alternating allocate/deallocate pattern
        testFragmentation<BuddyAllocator>("Buddy", memory_size);
        testFragmentation<HybridAllocator>("Hybrid", memory_size);
        
        std::cout << "\n";
    }
    
    static void runStressBenchmark() {
        std::cout << "4. Stress Test (High Frequency Allocation/Deallocation)\n";
        std::cout << "------------------------------------------------------\n";
        
        const size_t iterations = 100000;
        const size_t memory_size = 4 * 1024 * 1024; // 4MB
        
        std::vector<BenchmarkResult> results;
        
        // High frequency random allocation/deallocation
        results.push_back(stressBenchmarkBuddy("Buddy", memory_size, iterations));
        results.push_back(stressBenchmarkHybrid("Hybrid", memory_size, iterations));
        
        printBenchmarkResults(results);
        std::cout << "\n";
    }
    
    static void runRealWorldSimulation() {
        std::cout << "5. Real-World Usage Simulation\n";
        std::cout << "------------------------------\n";
        
        // Simulate typical application usage patterns
        simulateWebServerWorkload();
        simulateGameEngineWorkload();
        
        std::cout << "\n";
    }
    
    // Specific allocator benchmarks
    static BenchmarkResult benchmarkBuddyAllocator(const std::string& name, size_t memory_size,
                                                  size_t iterations, size_t alloc_size) {
        BuddyAllocator allocator(memory_size);
        return runAllocationBenchmark(allocator, name, iterations, alloc_size);
    }
    
    static BenchmarkResult benchmarkSlabAllocator(const std::string& name, size_t object_size,
                                                 size_t objects_per_slab, size_t memory_size,
                                                 size_t iterations, size_t alloc_size) {
        SlabAllocator allocator(object_size, objects_per_slab, memory_size);
        return runAllocationBenchmark(allocator, name, iterations, alloc_size);
    }
    
    static BenchmarkResult benchmarkPoolAllocator(const std::string& name, size_t block_size,
                                                 size_t num_blocks, size_t memory_size,
                                                 size_t iterations, size_t alloc_size) {
        PoolAllocator allocator(block_size, num_blocks, memory_size);
        return runAllocationBenchmark(allocator, name, iterations, alloc_size);
    }
    
    static BenchmarkResult benchmarkHybridAllocator(const std::string& name, size_t memory_size,
                                                   size_t iterations, size_t alloc_size) {
        HybridAllocator allocator(memory_size);
        return runAllocationBenchmark(allocator, name, iterations, alloc_size);
    }
    
    // Generic benchmark function
    template<typename AllocatorType>
    static BenchmarkResult runAllocationBenchmark(AllocatorType& allocator, const std::string& name,
                                                 size_t iterations, size_t alloc_size) {
        BenchmarkResult result;
        result.allocator_name = name;
        result.total_attempts = iterations;
        
        std::vector<void*> ptrs;
        ptrs.reserve(iterations);
        
        // Allocation phase
        auto alloc_start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            void* ptr = allocator.allocate(alloc_size);
            if (ptr) {
                ptrs.push_back(ptr);
            }
        }
        
        auto alloc_end = std::chrono::high_resolution_clock::now();
        result.allocation_time_ms = std::chrono::duration<double, std::milli>(alloc_end - alloc_start).count();
        result.successful_allocations = ptrs.size();
        
        // Deallocation phase
        auto dealloc_start = std::chrono::high_resolution_clock::now();
        
        for (void* ptr : ptrs) {
            allocator.deallocate(ptr);
        }
        
        auto dealloc_end = std::chrono::high_resolution_clock::now();
        result.deallocation_time_ms = std::chrono::duration<double, std::milli>(dealloc_end - dealloc_start).count();
        
        // Calculate metrics
        result.total_time_ms = result.allocation_time_ms + result.deallocation_time_ms;
        result.success_rate = (double)result.successful_allocations / result.total_attempts * 100.0;
        result.fragmentation = allocator.getFragmentation();
        result.throughput_ops_per_sec = (result.successful_allocations * 2) / (result.total_time_ms / 1000.0); // *2 for alloc+dealloc
        
        return result;
    }
    
    // Variable size benchmarks
    static BenchmarkResult benchmarkBuddyAllocatorVariable(const std::string& name, size_t memory_size,
                                                         const std::vector<size_t>& sizes) {
        BuddyAllocator allocator(memory_size);
        return runVariableSizeBenchmark(allocator, name, sizes);
    }
    
    static BenchmarkResult benchmarkHybridAllocatorVariable(const std::string& name, size_t memory_size,
                                                          const std::vector<size_t>& sizes) {
        HybridAllocator allocator(memory_size);
        return runVariableSizeBenchmark(allocator, name, sizes);
    }
    
    template<typename AllocatorType>
    static BenchmarkResult runVariableSizeBenchmark(AllocatorType& allocator, const std::string& name,
                                                   const std::vector<size_t>& sizes) {
        BenchmarkResult result;
        result.allocator_name = name;
        result.total_attempts = sizes.size();
        
        std::vector<void*> ptrs;
        ptrs.reserve(sizes.size());
        
        // Allocation phase
        auto alloc_start = std::chrono::high_resolution_clock::now();
        
        for (size_t size : sizes) {
            void* ptr = allocator.allocate(size);
            if (ptr) {
                ptrs.push_back(ptr);
            }
        }
        
        auto alloc_end = std::chrono::high_resolution_clock::now();
        result.allocation_time_ms = std::chrono::duration<double, std::milli>(alloc_end - alloc_start).count();
        result.successful_allocations = ptrs.size();
        
        // Deallocation phase
        auto dealloc_start = std::chrono::high_resolution_clock::now();
        
        for (void* ptr : ptrs) {
            allocator.deallocate(ptr);
        }
        
        auto dealloc_end = std::chrono::high_resolution_clock::now();
        result.deallocation_time_ms = std::chrono::duration<double, std::milli>(dealloc_end - dealloc_start).count();
        
        // Calculate metrics
        result.total_time_ms = result.allocation_time_ms + result.deallocation_time_ms;
        result.success_rate = (double)result.successful_allocations / result.total_attempts * 100.0;
        result.fragmentation = allocator.getFragmentation();
        result.throughput_ops_per_sec = (result.successful_allocations * 2) / (result.total_time_ms / 1000.0);
        
        return result;
    }
    
    // Utility functions
    static std::vector<size_t> generateRandomSizes(size_t count, size_t min_size, size_t max_size) {
        std::vector<size_t> sizes;
        sizes.reserve(count);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dist(min_size, max_size);
        
        for (size_t i = 0; i < count; ++i) {
            sizes.push_back(dist(gen));
        }
        
        return sizes;
    }
    
    static void printBenchmarkResults(const std::vector<BenchmarkResult>& results) {
        std::cout << std::setw(10) << "Allocator" 
                  << std::setw(12) << "Alloc (ms)"
                  << std::setw(12) << "Dealloc (ms)"
                  << std::setw(12) << "Total (ms)"
                  << std::setw(12) << "Success %"
                  << std::setw(12) << "Frag %"
                  << std::setw(15) << "Throughput/s" << "\n";
        std::cout << std::string(83, '-') << "\n";
        
        for (const auto& result : results) {
            std::cout << std::setw(10) << result.allocator_name
                      << std::setw(12) << std::fixed << std::setprecision(2) << result.allocation_time_ms
                      << std::setw(12) << std::fixed << std::setprecision(2) << result.deallocation_time_ms
                      << std::setw(12) << std::fixed << std::setprecision(2) << result.total_time_ms
                      << std::setw(11) << std::fixed << std::setprecision(1) << result.success_rate << "%"
                      << std::setw(11) << result.fragmentation << "%"
                      << std::setw(15) << std::fixed << std::setprecision(0) << result.throughput_ops_per_sec << "\n";
        }
    }
    
    // Specialized tests
    template<typename AllocatorType>
    static void testFragmentation(const std::string& name, size_t memory_size) {
        AllocatorType allocator(memory_size);
        
        std::vector<void*> ptrs;
        
        // Allocate many small blocks
        for (int i = 0; i < 100; ++i) {
            void* ptr = allocator.allocate(64);
            if (ptr) ptrs.push_back(ptr);
        }
        
        // Deallocate every other block
        for (size_t i = 1; i < ptrs.size(); i += 2) {
            allocator.deallocate(ptrs[i]);
        }
        
        size_t fragmentation_after_partial_dealloc = allocator.getFragmentation();
        
        // Try to allocate larger blocks
        std::vector<void*> large_ptrs;
        for (int i = 0; i < 10; ++i) {
            void* ptr = allocator.allocate(256);
            if (ptr) large_ptrs.push_back(ptr);
        }
        
        size_t fragmentation_after_large_alloc = allocator.getFragmentation();
        
        // Clean up
        for (size_t i = 0; i < ptrs.size(); i += 2) {
            allocator.deallocate(ptrs[i]);
        }
        for (void* ptr : large_ptrs) {
            allocator.deallocate(ptr);
        }
        
        std::cout << name << " Fragmentation Test:\n";
        std::cout << "  After partial deallocation: " << fragmentation_after_partial_dealloc << "%\n";
        std::cout << "  After large allocations: " << fragmentation_after_large_alloc << "%\n";
    }
    
    static BenchmarkResult stressBenchmarkBuddy(const std::string& name, size_t memory_size, size_t iterations) {
        BuddyAllocator allocator(memory_size);
        return stressBenchmarkGeneric(allocator, name, iterations);
    }
    
    static BenchmarkResult stressBenchmarkHybrid(const std::string& name, size_t memory_size, size_t iterations) {
        HybridAllocator allocator(memory_size);
        return stressBenchmarkGeneric(allocator, name, iterations);
    }
    
    template<typename AllocatorType>
    static BenchmarkResult stressBenchmarkGeneric(AllocatorType& allocator, const std::string& name, size_t iterations) {
        BenchmarkResult result;
        result.allocator_name = name;
        result.total_attempts = iterations;
        
        std::vector<void*> active_ptrs;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> size_dist(16, 512);
        std::uniform_real_distribution<double> action_dist(0.0, 1.0);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            if (active_ptrs.empty() || action_dist(gen) < 0.6) {
                // 60% chance to allocate (or if no active pointers)
                size_t size = size_dist(gen);
                void* ptr = allocator.allocate(size);
                if (ptr) {
                    active_ptrs.push_back(ptr);
                    result.successful_allocations++;
                }
            } else {
                // 40% chance to deallocate
                size_t index = gen() % active_ptrs.size();
                allocator.deallocate(active_ptrs[index]);
                active_ptrs.erase(active_ptrs.begin() + index);
            }
        }
        
        // Clean up remaining allocations
        for (void* ptr : active_ptrs) {
            allocator.deallocate(ptr);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.total_time_ms = std::chrono::duration<double, std::milli>(end - start).count();
        result.allocation_time_ms = result.total_time_ms; // Mixed operations
        result.deallocation_time_ms = 0;
        result.success_rate = (double)result.successful_allocations / result.total_attempts * 100.0;
        result.fragmentation = allocator.getFragmentation();
        result.throughput_ops_per_sec = iterations / (result.total_time_ms / 1000.0);
        
        return result;
    }
    
    static void simulateWebServerWorkload() {
        std::cout << "Web Server Simulation (many small allocations):\n";
        
        HybridAllocator allocator(8 * 1024 * 1024); // 8MB
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate handling 1000 requests
        for (int request = 0; request < 1000; ++request) {
            std::vector<void*> request_memory;
            
            // Each request needs various small allocations
            request_memory.push_back(allocator.allocate(256)); // Request object
            request_memory.push_back(allocator.allocate(1024)); // Response buffer
            request_memory.push_back(allocator.allocate(512)); // Headers
            request_memory.push_back(allocator.allocate(128)); // Session data
            
            // Clean up request memory
            for (void* ptr : request_memory) {
                if (ptr) allocator.deallocate(ptr);
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        std::cout << "  Processed 1000 requests in " << duration << " ms\n";
        std::cout << "  Average per request: " << duration / 1000.0 << " ms\n";
        std::cout << "  Final fragmentation: " << allocator.getFragmentation() << "%\n";
    }
    
    static void simulateGameEngineWorkload() {
        std::cout << "Game Engine Simulation (mixed allocation patterns):\n";
        
        HybridAllocator allocator(16 * 1024 * 1024); // 16MB
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate game frames
        for (int frame = 0; frame < 60; ++frame) {
            std::vector<void*> frame_memory;
            
            // Each frame needs various allocations
            frame_memory.push_back(allocator.allocate(4096)); // Render commands
            frame_memory.push_back(allocator.allocate(2048)); // Vertex buffer
            frame_memory.push_back(allocator.allocate(1024)); // Texture data
            frame_memory.push_back(allocator.allocate(512)); // Audio buffer
            
            // Multiple small allocations for game objects
            for (int obj = 0; obj < 50; ++obj) {
                void* ptr = allocator.allocate(64); // Game object
                if (ptr) frame_memory.push_back(ptr);
            }
            
            // Clean up frame memory
            for (void* ptr : frame_memory) {
                if (ptr) allocator.deallocate(ptr);
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        std::cout << "  Simulated 60 frames in " << duration << " ms\n";
        std::cout << "  Average per frame: " << duration / 60.0 << " ms\n";
        std::cout << "  Final fragmentation: " << allocator.getFragmentation() << "%\n";
    }
};

int main() {
    try {
        MemoryAllocatorBenchmark::runComprehensiveBenchmarks();
    }
    catch (const std::exception& e) {
        std::cerr << "Benchmark failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
