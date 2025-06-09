#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <cstddef>
#include <memory>
#include <chrono>
#include <string>
#include <vector>

/**
 * @brief Base interface for all memory allocators
 * 
 * Định nghĩa interface chung cho tất cả các loại memory allocator
 * Cung cấp các phương thức cơ bản và thống kê hiệu suất
 */
class MemoryAllocator {
public:
    struct AllocationStats {
        size_t total_allocated = 0;      // Tổng bộ nhớ đã cấp phát
        size_t total_freed = 0;          // Tổng bộ nhớ đã giải phóng
        size_t current_allocated = 0;    // Bộ nhớ hiện tại đang sử dụng
        size_t peak_allocated = 0;       // Peak memory usage
        size_t num_allocations = 0;      // Số lần allocation
        size_t num_deallocations = 0;    // Số lần deallocation
        double avg_alloc_time_ns = 0.0;  // Thời gian allocation trung bình (nanoseconds)
        double avg_dealloc_time_ns = 0.0; // Thời gian deallocation trung bình
        double fragmentation_ratio = 0.0; // Tỷ lệ phân mảnh (0.0 = no fragmentation, 1.0 = high fragmentation)
    };    struct MemoryBlock {
        size_t address;
        size_t size;
        bool is_free;
        std::string type;
    };

public:    MemoryAllocator(size_t total_memory); // Updated constructor
    virtual ~MemoryAllocator();

    // Core allocation methods
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* ptr) = 0;

    // Memory management
    virtual void reset() {}
    virtual size_t getTotalMemory() const { return total_memory_; }
    
    // Statistics and monitoring
    virtual size_t getFragmentation() const = 0;
    virtual std::string getStats() const;
    virtual std::vector<MemoryAllocator::MemoryBlock> getMemoryLayout() const = 0;
    
    // Performance testing
    void benchmarkAllocation(size_t num_iterations, size_t alloc_size);
    void stressTest(size_t duration_seconds);    // Utility methods
    bool isValidPointer(void* ptr) const;
    
    // Statistics helpers
    size_t getAllocationCount() const { return allocation_count_; }
    size_t getDeallocationCount() const { return deallocation_count_; }
    size_t getAllocatedSize() const { return allocated_size_; }

protected:
    size_t total_memory_;
    size_t allocated_size_;
    size_t allocation_count_;
    size_t deallocation_count_;
    std::chrono::steady_clock::time_point start_time_;
};

// Factory pattern for creating allocators
class AllocatorFactory {
public:
    enum class AllocatorType {
        BUDDY_SYSTEM,
        SLAB,
        MEMORY_POOL,
        HYBRID
    };

    static std::unique_ptr<MemoryAllocator> create_allocator(
        AllocatorType type, 
        size_t initial_size = 1024 * 1024,
        const std::string& config = ""
    );
};

#endif // MEMORY_ALLOCATOR_H
