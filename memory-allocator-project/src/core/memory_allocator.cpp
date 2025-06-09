#include "memory_allocator.h"
#include "buddy_allocator.h"
#include "slab_allocator.h"
#include "pool_allocator.h"
#include "hybrid_allocator.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <chrono>
#include <vector>

MemoryAllocator::MemoryAllocator(size_t total_memory) 
    : total_memory_(total_memory), allocated_size_(0), allocation_count_(0), deallocation_count_(0) {
}

MemoryAllocator::~MemoryAllocator() {
    std::cout << "Destroyed memory allocator\n";
    std::cout << "  Total allocations: " << allocation_count_ << "\n";
    std::cout << "  Total deallocations: " << deallocation_count_ << "\n";
    std::cout << "  Allocated size: " << allocated_size_ << " bytes\n";
}

std::string MemoryAllocator::getStats() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Memory Allocator Statistics:\n";
    oss << "  Total Memory: " << total_memory_ << " bytes\n";
    oss << "  Total Allocations: " << allocation_count_ << "\n";
    oss << "  Total Deallocations: " << deallocation_count_ << "\n";
    oss << "  Current Allocated: " << allocated_size_ << " bytes\n";
    oss << "  Utilization: " << (100.0 * allocated_size_ / total_memory_) << "%\n";
    oss << "  Current Fragmentation: " << getFragmentation() << " bytes\n";
    
    return oss.str();
}

void MemoryAllocator::benchmarkAllocation(size_t num_iterations, size_t alloc_size) {
    std::cout << "Benchmarking allocation performance:\n";
    std::cout << "  Iterations: " << num_iterations << "\n";
    std::cout << "  Allocation size: " << alloc_size << " bytes\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<void*> ptrs;
    ptrs.reserve(num_iterations);
    
    // Allocation phase
    for (size_t i = 0; i < num_iterations; ++i) {
        void* ptr = allocate(alloc_size);
        if (ptr) {
            ptrs.push_back(ptr);
        }
    }
    
    auto mid = std::chrono::high_resolution_clock::now();
    
    // Deallocation phase
    for (void* ptr : ptrs) {
        if (ptr) {
            deallocate(ptr);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto alloc_time = std::chrono::duration_cast<std::chrono::microseconds>(mid - start);
    auto dealloc_time = std::chrono::duration_cast<std::chrono::microseconds>(end - mid);
    
    std::cout << "  Allocation time: " << alloc_time.count() << " μs\n";
    std::cout << "  Deallocation time: " << dealloc_time.count() << " μs\n";
    std::cout << "  Total time: " << (alloc_time + dealloc_time).count() << " μs\n";
    std::cout << "  Successful allocations: " << ptrs.size() << "/" << num_iterations << "\n";
}

void MemoryAllocator::stressTest(size_t duration_seconds) {
    std::cout << "Running stress test for " << duration_seconds << " seconds...\n";
    
    auto start_time = std::chrono::steady_clock::now();
    auto end_time = start_time + std::chrono::seconds(duration_seconds);
    
    std::vector<void*> active_ptrs;
    size_t operations = 0;
    
    while (std::chrono::steady_clock::now() < end_time) {
        ++operations;
        
        if (active_ptrs.empty() || (operations % 3 == 0)) {
            // Allocate
            size_t size = 32 + (operations % 512); // Variable sizes 32-544 bytes
            void* ptr = allocate(size);
            if (ptr) {
                active_ptrs.push_back(ptr);
            }
        } else {
            // Deallocate random pointer
            size_t index = operations % active_ptrs.size();
            if (active_ptrs[index]) {
                deallocate(active_ptrs[index]);
                active_ptrs.erase(active_ptrs.begin() + index);
            }
        }
    }
    
    // Clean up remaining allocations
    for (void* ptr : active_ptrs) {
        if (ptr) {
            deallocate(ptr);
        }
    }
    
    std::cout << "Stress test completed:\n";
    std::cout << "  Total operations: " << operations << "\n";
    std::cout << "  Final state: " << getAllocationCount() << " allocations, " 
              << getDeallocationCount() << " deallocations\n";
}

bool MemoryAllocator::isValidPointer(void* ptr) const {
    // Basic pointer validation - can be enhanced by derived classes
    return ptr != nullptr;
}

// Factory implementation
std::unique_ptr<MemoryAllocator> AllocatorFactory::create_allocator(
    AllocatorType type, size_t initial_size, const std::string& config) {
    
    switch (type) {
        case AllocatorType::BUDDY_SYSTEM:
            return std::make_unique<BuddyAllocator>(initial_size);
            
        case AllocatorType::SLAB: {
            // For slab allocator, use default parameters
            return std::make_unique<SlabAllocator>(64, 32, initial_size);
        }
            
        case AllocatorType::MEMORY_POOL: {
            // For pool allocator, use default configuration
            PoolAllocator::PoolConfig pool_config;
            pool_config.block_sizes = {32, 64, 128, 256};
            pool_config.blocks_per_pool = {100, 80, 60, 40};
            pool_config.total_memory = initial_size;
            return std::make_unique<PoolAllocator>(pool_config);
        }
            
        case AllocatorType::HYBRID:
            return std::make_unique<HybridAllocator>(initial_size);
            
        default:
            throw std::invalid_argument("Unknown allocator type");
    }
}
