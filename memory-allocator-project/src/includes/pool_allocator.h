#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "memory_allocator.h"
#include <vector>
#include <unordered_map>
#include <mutex>

/**
 * @brief Pool Memory Allocator
 * 
 * Implementation of Pool Allocation algorithm:
 * - Pre-allocates fixed-size blocks
 * - Fast allocation/deallocation (O(1))
 * - No fragmentation for uniform block sizes
 * - Ideal for frequent allocation/deallocation of same-sized objects
 */
class PoolAllocator : public MemoryAllocator {
public:    struct PoolConfig {
        std::vector<size_t> block_sizes;      // Available block sizes
        std::vector<size_t> blocks_per_pool;  // Number of blocks per size
        size_t total_memory;                  // Total memory to allocate
    };

    struct FreeBlock {
        FreeBlock* next;
    };

    struct MemoryPool {
        void* memory;                    // Pool memory region
        FreeBlock* free_list;           // Free block list
        size_t block_size;              // Size of each block
        size_t total_blocks;            // Total blocks in pool
        size_t free_blocks;             // Available blocks
        
        MemoryPool(size_t block_size, size_t num_blocks);
        ~MemoryPool();
        
        bool initialize();
        void* allocate_block();
        void deallocate_block(void* ptr);
        bool contains_address(void* ptr) const;
        double get_utilization() const;
    };

    explicit PoolAllocator(const PoolConfig& config);
    ~PoolAllocator() override;

    // Core allocation methods
    void* allocate(size_t size) override;
    void deallocate(void* ptr) override;
    
    // Statistics and info
    size_t getFragmentation() const override;
    std::string getStats() const override;
    std::vector<MemoryAllocator::MemoryBlock> getMemoryLayout() const override;
    
    // Pool-specific methods
    void reset() override;
    bool canAllocate(size_t size) const;
    size_t getPoolCount() const { return pools_.size(); }
    double getAverageUtilization() const;

private:    struct AllocatorStats {
        size_t total_allocations = 0;
        size_t total_deallocations = 0;
        size_t failed_allocations = 0;
        size_t current_allocated = 0;
        size_t peak_allocated = 0;
    };
    
    MemoryPool* findPoolForSize(size_t size);
    MemoryPool* findPoolForAddress(void* ptr);
    
    std::vector<std::unique_ptr<MemoryPool>> pools_;
    std::unordered_map<void*, MemoryPool*> allocation_map_;
    AllocatorStats stats_;
    
    mutable std::mutex mutex_;
};

#endif // POOL_ALLOCATOR_H
