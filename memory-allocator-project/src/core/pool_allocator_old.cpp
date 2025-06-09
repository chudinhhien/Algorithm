#include "../includes/pool_allocator.h"
#include <cstring>
#include <algorithm>

PoolAllocator::PoolAllocator(size_t block_size, size_t num_blocks, size_t total_memory)
    : MemoryAllocator(total_memory), block_size_(block_size), num_blocks_(num_blocks) {
    
    // Ensure block size is at least size of pointer for free list
    if (block_size_ < sizeof(void*)) {
        block_size_ = sizeof(void*);
    }
    
    // Calculate required memory
    size_t required_memory = block_size_ * num_blocks_;
    if (required_memory > total_memory) {
        num_blocks_ = total_memory / block_size_;
    }
    
    // Initialize memory pool
    memory_pool_ = new char[total_memory];
    std::memset(memory_pool_, 0, total_memory);
    
    // Initialize free list
    initializeFreeList();
}

PoolAllocator::~PoolAllocator() {
    delete[] memory_pool_;
}

void* PoolAllocator::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (size > block_size_) {
        return nullptr; // Size too large for this pool
    }
    
    if (!free_head_) {
        return nullptr; // No free blocks available
    }
    
    // Get block from free list
    void* ptr = free_head_;
    free_head_ = *static_cast<void**>(free_head_);
    
    // Update statistics
    allocated_size_ += block_size_;
    allocation_count_++;
    free_blocks_--;
    
    // Clear the allocated memory
    std::memset(ptr, 0, block_size_);
    
    return ptr;
}

void PoolAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Verify pointer is within our memory pool
    if (!isValidPointer(ptr)) {
        return;
    }
    
    // Add block back to free list
    *static_cast<void**>(ptr) = free_head_;
    free_head_ = ptr;
    
    // Update statistics
    allocated_size_ -= block_size_;
    deallocation_count_++;
    free_blocks_++;
}

size_t PoolAllocator::getFragmentation() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Pool allocator has no internal fragmentation (fixed block size)
    // External fragmentation is the percentage of free blocks
    if (num_blocks_ == 0) return 0;
    
    return (free_blocks_ * 100) / num_blocks_;
}

std::string PoolAllocator::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string stats = MemoryAllocator::getStats();
    stats += "Pool Allocator Stats:\n";
    stats += "  Block Size: " + std::to_string(block_size_) + " bytes\n";
    stats += "  Total Blocks: " + std::to_string(num_blocks_) + "\n";
    stats += "  Free Blocks: " + std::to_string(free_blocks_) + "\n";
    stats += "  Allocated Blocks: " + std::to_string(num_blocks_ - free_blocks_) + "\n";
    stats += "  Pool Utilization: " + std::to_string(((num_blocks_ - free_blocks_) * 100) / num_blocks_) + "%\n";
    
    return stats;
}

void PoolAllocator::initializeFreeList() {
    free_blocks_ = num_blocks_;
    free_head_ = nullptr;
    
    // Link all blocks in a free list
    for (size_t i = 0; i < num_blocks_; ++i) {
        void* block = memory_pool_ + i * block_size_;
        *static_cast<void**>(block) = free_head_;
        free_head_ = block;
    }
}

bool PoolAllocator::isValidPointer(void* ptr) const {
    char* char_ptr = static_cast<char*>(ptr);
    char* pool_start = memory_pool_;
    char* pool_end = memory_pool_ + (num_blocks_ * block_size_);
    
    // Check if pointer is within pool bounds
    if (char_ptr < pool_start || char_ptr >= pool_end) {
        return false;
    }
    
    // Check if pointer is aligned to block boundary
    size_t offset = char_ptr - pool_start;
    return (offset % block_size_) == 0;
}

std::vector<MemoryBlock> PoolAllocator::getMemoryLayout() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<MemoryBlock> layout;
    
    // Build set of free blocks for quick lookup
    std::set<void*> free_blocks_set;
    void* current = free_head_;
    while (current) {
        free_blocks_set.insert(current);
        current = *static_cast<void**>(current);
    }
    
    // Create memory layout
    for (size_t i = 0; i < num_blocks_; ++i) {
        MemoryBlock block;
        block.address = i * block_size_;
        block.size = block_size_;
        
        void* block_ptr = memory_pool_ + block.address;
        block.is_free = free_blocks_set.count(block_ptr) > 0;
        block.type = block.is_free ? "Free Block" : "Allocated Block";
        
        layout.push_back(block);
    }
    
    return layout;
}

void PoolAllocator::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Reset statistics
    allocated_size_ = 0;
    allocation_count_ = 0;
    deallocation_count_ = 0;
    
    // Clear memory and reinitialize free list
    std::memset(memory_pool_, 0, total_memory_);
    initializeFreeList();
}

size_t PoolAllocator::getAvailableBlocks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return free_blocks_;
}

size_t PoolAllocator::getAllocatedBlocks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return num_blocks_ - free_blocks_;
}

bool PoolAllocator::canAllocate(size_t size) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return (size <= block_size_) && (free_blocks_ > 0);
}

void* PoolAllocator::tryAllocate(size_t size) {
    if (!canAllocate(size)) {
        return nullptr;
    }
    return allocate(size);
}

void PoolAllocator::defragment() {
    // Pool allocator doesn't need defragmentation since all blocks are fixed size
    // This is a no-op but kept for interface compatibility
}

double PoolAllocator::getUtilizationRatio() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (num_blocks_ == 0) return 0.0;
    return static_cast<double>(num_blocks_ - free_blocks_) / num_blocks_;
}

size_t PoolAllocator::getLargestFreeBlock() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return free_blocks_ > 0 ? block_size_ : 0;
}
