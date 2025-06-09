#include "../includes/memory_allocator.h"
#include "../includes/buddy_allocator.h"
#include "../includes/slab_allocator.h"
#include "../includes/pool_allocator.h"
#include "../includes/hybrid_allocator.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <random>

MemoryAllocator::MemoryAllocator(const std::string& name, size_t initial_size)
    : name_(name), total_size_(initial_size), memory_pool_(nullptr) {
    
    // Allocate the main memory pool
    memory_pool_ = std::malloc(total_size_);
    if (!memory_pool_) {
        throw std::bad_alloc();
    }
    
    // Initialize stats
    reset_stats();
    
    std::cout << "Created " << name_ << " with " << total_size_ << " bytes\n";
}

MemoryAllocator::~MemoryAllocator() {
    if (memory_pool_) {
        std::free(memory_pool_);
        memory_pool_ = nullptr;
    }
    
    std::cout << "Destroyed " << name_ << "\n";
    std::cout << "Final Stats:\n";
    std::cout << "  Total allocated: " << stats_.total_allocated << " bytes\n";
    std::cout << "  Total freed: " << stats_.total_freed << " bytes\n";
    std::cout << "  Peak usage: " << stats_.peak_allocated << " bytes\n";
    std::cout << "  Total allocations: " << stats_.num_allocations << "\n";
    std::cout << "  Avg alloc time: " << stats_.avg_alloc_time_ns << " ns\n";
    std::cout << "  Final fragmentation: " << stats_.fragmentation_ratio << "\n";
}

void MemoryAllocator::reset_stats() {
    stats_ = AllocationStats{};
}

void MemoryAllocator::reset() {
    reset_stats();
    memory_blocks_.clear();
    
    // Clear memory pool
    if (memory_pool_) {
        std::memset(memory_pool_, 0, total_size_);
    }
}

std::vector<MemoryAllocator::MemoryBlock> MemoryAllocator::get_memory_layout() const {
    return memory_blocks_;
}

void MemoryAllocator::update_allocation_stats(size_t size, double alloc_time_ns) {
    stats_.total_allocated += size;
    stats_.current_allocated += size;
    stats_.num_allocations++;
    
    // Update peak
    if (stats_.current_allocated > stats_.peak_allocated) {
        stats_.peak_allocated = stats_.current_allocated;
    }
    
    // Update average allocation time
    double total_time = stats_.avg_alloc_time_ns * (stats_.num_allocations - 1) + alloc_time_ns;
    stats_.avg_alloc_time_ns = total_time / stats_.num_allocations;
    
    calculate_fragmentation();
}

void MemoryAllocator::update_deallocation_stats(size_t size, double dealloc_time_ns) {
    stats_.total_freed += size;
    stats_.current_allocated -= size;
    stats_.num_deallocations++;
    
    // Update average deallocation time
    if (stats_.num_deallocations > 0) {
        double total_time = stats_.avg_dealloc_time_ns * (stats_.num_deallocations - 1) + dealloc_time_ns;
        stats_.avg_dealloc_time_ns = total_time / stats_.num_deallocations;
    }
    
    calculate_fragmentation();
}

void MemoryAllocator::calculate_fragmentation() {
    if (total_size_ == 0) {
        stats_.fragmentation_ratio = 0.0;
        return;
    }
    
    // Simple fragmentation calculation based on largest free block vs total free space
    size_t total_free = total_size_ - stats_.current_allocated;
    if (total_free == 0) {
        stats_.fragmentation_ratio = 0.0;
        return;
    }
    
    // This is a simplified calculation - subclasses should override for more accurate measurement
    size_t largest_free_block = get_available_size();
    if (largest_free_block == 0) {
        stats_.fragmentation_ratio = 1.0;
    } else {
        stats_.fragmentation_ratio = 1.0 - (static_cast<double>(largest_free_block) / total_free);
    }
}

bool MemoryAllocator::is_valid_pointer(void* ptr) const {
    if (!ptr || !memory_pool_) return false;
    
    uintptr_t pool_start = reinterpret_cast<uintptr_t>(memory_pool_);
    uintptr_t pool_end = pool_start + total_size_;
    uintptr_t ptr_addr = reinterpret_cast<uintptr_t>(ptr);
    
    return ptr_addr >= pool_start && ptr_addr < pool_end;
}

std::chrono::steady_clock::time_point MemoryAllocator::start_timer() {
    return std::chrono::steady_clock::now();
}

double MemoryAllocator::end_timer(const std::chrono::steady_clock::time_point& start) {
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    return static_cast<double>(duration.count());
}

void MemoryAllocator::benchmark_allocation(size_t num_iterations, size_t alloc_size) {
    std::cout << "\n=== Benchmarking " << name_ << " ===\n";
    std::cout << "Iterations: " << num_iterations << ", Size: " << alloc_size << " bytes\n";
    
    std::vector<void*> allocated_ptrs;
    allocated_ptrs.reserve(num_iterations);
    
    // Allocation benchmark
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < num_iterations; ++i) {
        void* ptr = allocate(alloc_size);
        if (ptr) {
            allocated_ptrs.push_back(ptr);
        }
    }
    
    auto alloc_end_time = std::chrono::high_resolution_clock::now();
    
    // Deallocation benchmark
    for (void* ptr : allocated_ptrs) {
        deallocate(ptr);
    }
    
    auto dealloc_end_time = std::chrono::high_resolution_clock::now();
    
    // Calculate times
    auto alloc_duration = std::chrono::duration_cast<std::chrono::microseconds>(
        alloc_end_time - start_time);
    auto dealloc_duration = std::chrono::duration_cast<std::chrono::microseconds>(
        dealloc_end_time - alloc_end_time);
    
    std::cout << "Allocation time: " << alloc_duration.count() << " μs\n";
    std::cout << "Deallocation time: " << dealloc_duration.count() << " μs\n";
    std::cout << "Avg allocation time: " << (alloc_duration.count() / static_cast<double>(num_iterations)) << " μs\n";
    std::cout << "Successful allocations: " << allocated_ptrs.size() << "/" << num_iterations << "\n";
}

void MemoryAllocator::stress_test(size_t duration_seconds) {
    std::cout << "\n=== Stress Testing " << name_ << " for " << duration_seconds << " seconds ===\n";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> size_dist(8, 1024);
    std::uniform_real_distribution<double> action_dist(0.0, 1.0);
    
    std::vector<std::pair<void*, size_t>> allocated_blocks;
    auto start_time = std::chrono::steady_clock::now();
    size_t operations = 0;
    size_t failed_allocations = 0;
    
    while (true) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time);
        if (elapsed.count() >= static_cast<long>(duration_seconds)) break;
        
        operations++;
        
        // Decide whether to allocate or deallocate
        bool should_allocate = allocated_blocks.empty() || 
                              (action_dist(gen) < 0.6 && allocated_blocks.size() < 1000);
        
        if (should_allocate) {
            size_t size = size_dist(gen);
            void* ptr = allocate(size);
            if (ptr) {
                allocated_blocks.emplace_back(ptr, size);
            } else {
                failed_allocations++;
            }
        } else if (!allocated_blocks.empty()) {
            // Random deallocation
            std::uniform_int_distribution<size_t> index_dist(0, allocated_blocks.size() - 1);
            size_t index = index_dist(gen);
            deallocate(allocated_blocks[index].first);
            allocated_blocks.erase(allocated_blocks.begin() + index);
        }
    }
    
    // Cleanup remaining allocations
    for (auto& block : allocated_blocks) {
        deallocate(block.first);
    }
    
    std::cout << "Stress test completed:\n";
    std::cout << "  Total operations: " << operations << "\n";
    std::cout << "  Failed allocations: " << failed_allocations << "\n";
    std::cout << "  Success rate: " << (100.0 * (operations - failed_allocations) / operations) << "%\n";
    std::cout << "  Final fragmentation: " << stats_.fragmentation_ratio << "\n";
}

// Factory implementation
std::unique_ptr<MemoryAllocator> AllocatorFactory::create_allocator(
    AllocatorType type, size_t initial_size, const std::string& config) {
    
    switch (type) {
        case AllocatorType::BUDDY_SYSTEM:
            return std::make_unique<BuddyAllocator>(initial_size);
            
        case AllocatorType::SLAB:
            return std::make_unique<SlabAllocator>(initial_size);
            
        case AllocatorType::MEMORY_POOL:
            return std::make_unique<PoolAllocator>(initial_size);
            
        case AllocatorType::HYBRID:
            return std::make_unique<HybridAllocator>(initial_size);
            
        default:
            throw std::invalid_argument("Unknown allocator type");
    }
}
