#include "../includes/pool_allocator.h"
#include <cstring>
#include <algorithm>
#include <sstream>

// MemoryPool implementation
PoolAllocator::MemoryPool::MemoryPool(size_t block_size, size_t num_blocks) 
    : memory(nullptr), free_list(nullptr), block_size(block_size), 
      total_blocks(num_blocks), free_blocks(num_blocks) {
    initialize();
}

PoolAllocator::MemoryPool::~MemoryPool() {
    if (memory) {
        std::free(memory);
    }
}

bool PoolAllocator::MemoryPool::initialize() {
    // Ensure block size can hold a pointer
    if (block_size < sizeof(FreeBlock*)) {
        block_size = sizeof(FreeBlock*);
    }
    
    // Allocate memory for all blocks
    memory = std::aligned_alloc(alignof(std::max_align_t), block_size * total_blocks);
    if (!memory) return false;
    
    // Initialize free list
    free_list = nullptr;
    char* current = static_cast<char*>(memory);
    
    for (size_t i = 0; i < total_blocks; ++i) {
        FreeBlock* block = reinterpret_cast<FreeBlock*>(current);
        block->next = free_list;
        free_list = block;
        current += block_size;
    }
    
    return true;
}

void* PoolAllocator::MemoryPool::allocate_block() {
    if (!free_list) return nullptr;
    
    FreeBlock* block = free_list;
    free_list = free_list->next;
    free_blocks--;
    
    return block;
}

void PoolAllocator::MemoryPool::deallocate_block(void* ptr) {
    if (!ptr) return;
    
    FreeBlock* block = static_cast<FreeBlock*>(ptr);
    block->next = free_list;
    free_list = block;
    free_blocks++;
}

bool PoolAllocator::MemoryPool::contains_address(void* ptr) const {
    if (!ptr || !memory) return false;
    
    char* mem_start = static_cast<char*>(memory);
    char* mem_end = mem_start + (block_size * total_blocks);
    char* check_ptr = static_cast<char*>(ptr);
    
    return (check_ptr >= mem_start && check_ptr < mem_end);
}

double PoolAllocator::MemoryPool::get_utilization() const {
    if (total_blocks == 0) return 0.0;
    return static_cast<double>(total_blocks - free_blocks) / total_blocks;
}

// PoolAllocator implementation
PoolAllocator::PoolAllocator(const PoolConfig& config) : MemoryAllocator(0) {
    if (config.block_sizes.size() != config.blocks_per_pool.size()) {
        throw std::invalid_argument("Block sizes and blocks per pool vectors must have same size");
    }
    
    size_t total_memory = 0;
    for (size_t i = 0; i < config.block_sizes.size(); ++i) {
        auto pool = std::make_unique<MemoryPool>(config.block_sizes[i], config.blocks_per_pool[i]);
        total_memory += config.block_sizes[i] * config.blocks_per_pool[i];
        pools_.push_back(std::move(pool));
    }
    
    total_memory_ = total_memory;
}

PoolAllocator::~PoolAllocator() {
    pools_.clear();
}

void* PoolAllocator::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    MemoryPool* pool = findPoolForSize(size);
    if (!pool) return nullptr;
    
    void* ptr = pool->allocate_block();
    if (ptr) {
        allocation_map_[ptr] = pool;
        allocated_size_ += pool->block_size;
        allocation_count_++;
    }
    
    return ptr;
}

void PoolAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocation_map_.find(ptr);
    if (it != allocation_map_.end()) {
        MemoryPool* pool = it->second;
        pool->deallocate_block(ptr);
        
        allocated_size_ -= pool->block_size;
        deallocation_count_++;
        allocation_map_.erase(it);
    }
}

size_t PoolAllocator::getFragmentation() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Pool allocator has no internal fragmentation for exact matches
    // Calculate external fragmentation as unusable memory
    size_t total_free_space = 0;
    size_t largest_free_block = 0;
    
    for (const auto& pool : pools_) {
        if (pool->free_blocks > 0) {
            size_t pool_free_space = pool->free_blocks * pool->block_size;
            total_free_space += pool_free_space;
            largest_free_block = std::max(largest_free_block, pool->block_size);
        }
    }
    
    if (total_free_space == 0) return 0;
    
    // Fragmentation = (total_free - largest_block) / total_free * 100
    return ((total_free_space - largest_free_block) * 100) / total_free_space;
}

std::string PoolAllocator::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "=== Pool Allocator Statistics ===\\n";
    oss << "Total Memory: " << total_memory_ << " bytes\\n";
    oss << "Allocated: " << allocated_size_ << " bytes\\n";
    oss << "Free: " << (total_memory_ - allocated_size_) << " bytes\\n";
    oss << "Allocations: " << allocation_count_ << "\\n";
    oss << "Deallocations: " << deallocation_count_ << "\\n";
    oss << "Active Allocations: " << (allocation_count_ - deallocation_count_) << "\\n";
    oss << "Pool Count: " << pools_.size() << "\\n";
    oss << "Average Utilization: " << (getAverageUtilization() * 100) << "%\\n";
    
    for (size_t i = 0; i < pools_.size(); ++i) {
        const auto& pool = pools_[i];
        oss << "Pool " << i << ": " << pool->block_size << " bytes, ";
        oss << pool->total_blocks << " blocks, ";
        oss << pool->free_blocks << " free, ";
        oss << (pool->get_utilization() * 100) << "% utilized\\n";
    }
    
    return oss.str();
}

std::vector<MemoryBlock> PoolAllocator::getMemoryLayout() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<MemoryBlock> layout;
    size_t offset = 0;
    
    for (size_t pool_idx = 0; pool_idx < pools_.size(); ++pool_idx) {
        const auto& pool = pools_[pool_idx];
        char* pool_memory = static_cast<char*>(pool->memory);
        
        for (size_t block_idx = 0; block_idx < pool->total_blocks; ++block_idx) {
            void* block_ptr = pool_memory + (block_idx * pool->block_size);
            bool is_free = true;
            
            // Check if block is allocated
            for (const auto& alloc : allocation_map_) {
                if (alloc.first == block_ptr) {
                    is_free = false;
                    break;
                }
            }
            
            MemoryBlock block;
            block.offset = offset;
            block.size = pool->block_size;
            block.is_free = is_free;
            block.info = "Pool " + std::to_string(pool_idx) + 
                        ", Block " + std::to_string(block_idx);
            
            layout.push_back(block);
            offset += pool->block_size;
        }
    }
    
    return layout;
}

void PoolAllocator::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    allocation_map_.clear();
    allocated_size_ = 0;
    allocation_count_ = 0;
    deallocation_count_ = 0;
    
    // Reset all pools
    for (auto& pool : pools_) {
        pool->free_blocks = pool->total_blocks;
        
        // Reinitialize free list
        pool->free_list = nullptr;
        char* current = static_cast<char*>(pool->memory);
        
        for (size_t i = 0; i < pool->total_blocks; ++i) {
            FreeBlock* block = reinterpret_cast<FreeBlock*>(current);
            block->next = pool->free_list;
            pool->free_list = block;
            current += pool->block_size;
        }
    }
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
    // Find the smallest pool that can accommodate the requested size
    MemoryPool* best_pool = nullptr;
    
    for (auto& pool : pools_) {
        if (pool->block_size >= size && pool->free_blocks > 0) {
            if (!best_pool || pool->block_size < best_pool->block_size) {
                best_pool = pool.get();
            }
        }
    }
    
    return best_pool;
}

PoolAllocator::MemoryPool* PoolAllocator::findPoolForAddress(void* ptr) {
    for (auto& pool : pools_) {
        if (pool->contains_address(ptr)) {
            return pool.get();
        }
    }
    return nullptr;
}
