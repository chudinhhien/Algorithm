#include "pool_allocator.h"
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <iomanip>

// MemoryPool implementation
PoolAllocator::MemoryPool::MemoryPool(size_t block_size, size_t num_blocks)
    : memory(nullptr), free_list(nullptr), block_size(block_size), 
      total_blocks(num_blocks), free_blocks(0) {
}

PoolAllocator::MemoryPool::~MemoryPool() {
    if (memory) {
        std::free(memory);
    }
}

bool PoolAllocator::MemoryPool::initialize() {
    if (memory) {
        std::free(memory);
    }
      // Allocate memory for all blocks
    size_t total_size = block_size * total_blocks;
    memory = std::malloc(total_size);
    
    if (!memory) {
        return false;
    }
    
    // Initialize free list - link all blocks
    free_list = nullptr;
    char* current_block = static_cast<char*>(memory);
    
    for (size_t i = 0; i < total_blocks; ++i) {
        FreeBlock* block = reinterpret_cast<FreeBlock*>(current_block);
        block->next = free_list;
        free_list = block;
        current_block += block_size;
    }
    
    free_blocks = total_blocks;
    return true;
}

void* PoolAllocator::MemoryPool::allocate_block() {
    if (!free_list) {
        return nullptr;
    }
    
    FreeBlock* block = free_list;
    free_list = free_list->next;
    --free_blocks;
    
    return block;
}

void PoolAllocator::MemoryPool::deallocate_block(void* ptr) {
    if (!ptr || !contains_address(ptr)) {
        return;
    }
    
    FreeBlock* block = static_cast<FreeBlock*>(ptr);
    block->next = free_list;
    free_list = block;
    ++free_blocks;
}

bool PoolAllocator::MemoryPool::contains_address(void* ptr) const {
    if (!memory || !ptr) {
        return false;
    }
    
    char* start = static_cast<char*>(memory);
    char* end = start + (block_size * total_blocks);
    char* address = static_cast<char*>(ptr);
    
    return address >= start && address < end && 
           ((address - start) % block_size == 0);
}

double PoolAllocator::MemoryPool::get_utilization() const {
    if (total_blocks == 0) return 0.0;
    return static_cast<double>(total_blocks - free_blocks) / total_blocks;
}

// PoolAllocator implementation
PoolAllocator::PoolAllocator(const PoolConfig& config) : MemoryAllocator(config.total_memory) {
    if (config.block_sizes.size() != config.blocks_per_pool.size()) {
        throw std::invalid_argument("block_sizes and blocks_per_pool must have same size");
    }
    
    for (size_t i = 0; i < config.block_sizes.size(); ++i) {
        auto pool = std::make_unique<MemoryPool>(config.block_sizes[i], config.blocks_per_pool[i]);
        if (!pool->initialize()) {
            throw std::runtime_error("Failed to initialize memory pool");
        }
        pools_.push_back(std::move(pool));
    }
    
    // Sort pools by block size for efficient lookup
    std::sort(pools_.begin(), pools_.end(), 
              [](const std::unique_ptr<MemoryPool>& a, const std::unique_ptr<MemoryPool>& b) {
                  return a->block_size < b->block_size;
              });
}

PoolAllocator::~PoolAllocator() {
    // Pools will be automatically destroyed due to unique_ptr
}

void* PoolAllocator::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryPool* pool = findPoolForSize(size);
    if (!pool) {
        stats_.failed_allocations++;
        return nullptr;
    }
    
    void* ptr = pool->allocate_block();
    if (!ptr) {
        stats_.failed_allocations++;
        return nullptr;
    }
    
    allocation_map_[ptr] = pool;
    stats_.total_allocations++;
    stats_.current_allocated += pool->block_size;
    stats_.peak_allocated = std::max(stats_.peak_allocated, stats_.current_allocated);
    
    return ptr;
}

void PoolAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocation_map_.find(ptr);
    if (it == allocation_map_.end()) {
        return; // Invalid pointer
    }
    
    MemoryPool* pool = it->second;
    pool->deallocate_block(ptr);
    
    stats_.total_deallocations++;
    stats_.current_allocated -= pool->block_size;
    
    allocation_map_.erase(it);
}

size_t PoolAllocator::getFragmentation() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Pool allocators have no internal fragmentation for their block sizes
    // External fragmentation occurs when we can't find a suitable pool
    return 0;
}

std::string PoolAllocator::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Pool Allocator Statistics:\n";
    oss << "  Total Allocations: " << stats_.total_allocations << "\n";
    oss << "  Total Deallocations: " << stats_.total_deallocations << "\n";
    oss << "  Failed Allocations: " << stats_.failed_allocations << "\n";
    oss << "  Current Allocated: " << stats_.current_allocated << " bytes\n";
    oss << "  Peak Allocated: " << stats_.peak_allocated << " bytes\n";
    oss << "  Active Allocations: " << (stats_.total_allocations - stats_.total_deallocations) << "\n";
    oss << "  Number of Pools: " << pools_.size() << "\n";
    oss << "  Average Utilization: " << (getAverageUtilization() * 100) << "%\n";
    
    oss << "\nPool Details:\n";
    for (size_t i = 0; i < pools_.size(); ++i) {
        const auto& pool = pools_[i];
        oss << "  Pool " << i << " (size " << pool->block_size << "): "
            << (pool->total_blocks - pool->free_blocks) << "/" << pool->total_blocks 
            << " blocks used (" << (pool->get_utilization() * 100) << "%)\n";
    }
    
    return oss.str();
}

std::vector<MemoryAllocator::MemoryBlock> PoolAllocator::getMemoryLayout() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<MemoryBlock> layout;
    
    for (size_t i = 0; i < pools_.size(); ++i) {
        const auto& pool = pools_[i];
        if (!pool->memory) continue;
        
        char* start = static_cast<char*>(pool->memory);
          for (size_t j = 0; j < pool->total_blocks; ++j) {
            MemoryAllocator::MemoryBlock block;
            block.address = reinterpret_cast<size_t>(start + (j * pool->block_size));
            block.size = pool->block_size;
            
            // Check if this block is allocated
            bool is_allocated = false;
            void* block_addr = reinterpret_cast<void*>(block.address);
            for (const auto& pair : allocation_map_) {
                if (pair.first == block_addr && pair.second == pool.get()) {
                    is_allocated = true;
                    break;
                }
            }
            
            block.is_free = !is_allocated;
            block.type = "Pool";
            layout.push_back(block);
        }
    }
    
    return layout;
}

void PoolAllocator::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Reinitialize all pools
    for (auto& pool : pools_) {
        pool->initialize();
    }
    
    allocation_map_.clear();
    
    // Reset statistics
    stats_ = AllocatorStats{};
}

double PoolAllocator::getAverageUtilization() const {
    if (pools_.empty()) return 0.0;
    
    double total_utilization = 0.0;
    for (const auto& pool : pools_) {
        total_utilization += pool->get_utilization();
    }
    
    return total_utilization / pools_.size();
}

PoolAllocator::MemoryPool* PoolAllocator::findPoolForSize(size_t size) {
    // Find the smallest pool that can accommodate the size
    for (auto& pool : pools_) {
        if (pool->block_size >= size && pool->free_blocks > 0) {
            return pool.get();
        }
    }
    return nullptr;
}

PoolAllocator::MemoryPool* PoolAllocator::findPoolForAddress(void* ptr) {
    for (auto& pool : pools_) {
        if (pool->contains_address(ptr)) {
            return pool.get();
        }
    }
    return nullptr;
}

bool PoolAllocator::canAllocate(size_t size) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if any pool can handle this size and has free blocks
    for (const auto& pool : pools_) {
        if (pool->block_size >= size && pool->free_blocks > 0) {
            return true;
        }
    }
    return false;
}
