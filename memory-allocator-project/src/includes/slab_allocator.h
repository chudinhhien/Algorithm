#ifndef SLAB_ALLOCATOR_H
#define SLAB_ALLOCATOR_H

#include "memory_allocator.h"
#include <vector>
#include <set>
#include <mutex>

/**
 * @brief Slab Memory Allocator
 * 
 * Implementation of Slab Allocation algorithm:
 * - Optimized for fixed-size object allocation
 * - Reduces internal fragmentation
 * - Cache-friendly allocation pattern
 */
class SlabAllocator : public MemoryAllocator {
private:
    struct SlabHeader {
        size_t free_count;
        size_t first_free;
    };
    
    struct SlabInfo {
        size_t offset;
        size_t free_objects;
    };

public:
    SlabAllocator(size_t object_size, size_t objects_per_slab, size_t total_memory);
    ~SlabAllocator() override;

    // Core allocation methods
    void* allocate(size_t size) override;
    void deallocate(void* ptr) override;
    
    // Statistics and info
    size_t getFragmentation() const override;
    std::string getStats() const override;
    std::vector<MemoryAllocator::MemoryBlock> getMemoryLayout() const override;
    
    // Slab-specific methods
    size_t getObjectSize() const { return object_size_; }

private:
    void createSlab();
    void* allocateFromSlab(SlabInfo& slab);
    void deallocateFromSlab(SlabInfo& slab, void* ptr);

private:
    size_t object_size_;
    size_t objects_per_slab_;
    size_t slab_size_;
    size_t max_slabs_;
    std::vector<SlabInfo> slabs_;
    char* memory_pool_;
    mutable std::mutex mutex_;
};

#endif // SLAB_ALLOCATOR_H
