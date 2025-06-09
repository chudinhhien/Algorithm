#ifndef HYBRID_ALLOCATOR_H
#define HYBRID_ALLOCATOR_H

#include "memory_allocator.h"
#include "buddy_allocator.h"
#include "slab_allocator.h"
#include "pool_allocator.h"
#include <memory>
#include <unordered_map>
#include <mutex>

/**
 * @brief Hybrid Memory Allocator
 * 
 * Adaptive allocator that combines multiple allocation strategies:
 * - Pool allocator for small, frequent allocations
 * - Slab allocator for medium-sized objects
 * - Buddy allocator for large or variable-size allocations
 */
class HybridAllocator : public MemoryAllocator {
public:
    struct HybridConfig {
        double pool_memory_ratio = 0.3;
        double slab_memory_ratio = 0.3;
        size_t pool_max_size = 256;
        size_t slab_max_size = 1024;
    };

    enum class AllocatorType {
        POOL,
        SLAB,
        BUDDY
    };

    HybridAllocator(size_t total_memory, const HybridConfig& config = HybridConfig{});
    ~HybridAllocator() override;

    // Core allocation methods
    void* allocate(size_t size) override;
    void deallocate(void* ptr) override;
    
    // Statistics and info
    size_t getFragmentation() const override;
    std::string getStats() const override;
    std::vector<MemoryBlock> getMemoryLayout() const override;
    
    // Hybrid-specific methods
    void reset() override;
    double getEfficiencyScore() const;

private:
    struct AllocatorStats {
        size_t allocations = 0;
        size_t deallocations = 0;
        size_t total_allocated = 0;
    };

    AllocatorType selectAllocator(size_t size);
    void* allocateFromPool(size_t size);
    void* allocateFromSlab(size_t size);
    void deallocateFromPool(void* ptr);
    void deallocateFromSlab(void* ptr);
    void createPoolAllocators(size_t total_memory);
    void createSlabAllocators(size_t total_memory);
    void updateStatistics(AllocatorType type, size_t size, bool allocation);

private:
    HybridConfig config_;
    std::unique_ptr<BuddyAllocator> buddy_allocator_;
    std::vector<std::unique_ptr<PoolAllocator>> pool_allocators_;
    std::vector<std::unique_ptr<SlabAllocator>> slab_allocators_;
    
    std::unordered_map<void*, AllocatorType> allocation_map_;
    
    AllocatorStats pool_stats_;
    AllocatorStats slab_stats_;
    AllocatorStats buddy_stats_;
    
    mutable std::mutex mutex_;
};

#endif // HYBRID_ALLOCATOR_H
