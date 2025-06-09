#include "../includes/hybrid_allocator.h"
#include <algorithm>
#include <cmath>

HybridAllocator::HybridAllocator(size_t total_memory, const HybridConfig& config)
    : MemoryAllocator(total_memory), config_(config) {
    
    // Calculate memory distribution
    size_t pool_memory = static_cast<size_t>(total_memory * config.pool_memory_ratio);
    size_t slab_memory = static_cast<size_t>(total_memory * config.slab_memory_ratio);
    size_t buddy_memory = total_memory - pool_memory - slab_memory;
    
    // Ensure minimum memory for each allocator
    if (buddy_memory < 1024) {
        buddy_memory = 1024;
        slab_memory = (total_memory - buddy_memory) / 2;
        pool_memory = total_memory - buddy_memory - slab_memory;
    }
    
    // Initialize allocators
    buddy_allocator_ = std::make_unique<BuddyAllocator>(buddy_memory);
    
    // Create multiple pool allocators for different block sizes
    createPoolAllocators(pool_memory);
    
    // Create multiple slab allocators for different object sizes
    createSlabAllocators(slab_memory);
    
    // Initialize statistics
    reset();
}

HybridAllocator::HybridAllocator(size_t total_memory)
    : HybridAllocator(total_memory, HybridConfig{}) {
}

HybridAllocator::~HybridAllocator() = default;

void* HybridAllocator::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    void* ptr = nullptr;
    AllocatorType used_type = selectAllocator(size);
    
    switch (used_type) {
        case AllocatorType::POOL:
            ptr = allocateFromPool(size);
            break;
        case AllocatorType::SLAB:
            ptr = allocateFromSlab(size);
            break;
        case AllocatorType::BUDDY:
            ptr = buddy_allocator_->allocate(size);
            break;
    }
    
    if (ptr) {
        allocation_map_[ptr] = used_type;
        allocated_size_ += size;
        allocation_count_++;
        updateStatistics(used_type, size, true);
    }
    
    return ptr;
}

void HybridAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocation_map_.find(ptr);
    if (it == allocation_map_.end()) {
        return; // Unknown pointer
    }
    
    AllocatorType type = it->second;
    allocation_map_.erase(it);
    
    switch (type) {
        case AllocatorType::POOL:
            deallocateFromPool(ptr);
            break;
        case AllocatorType::SLAB:
            deallocateFromSlab(ptr);
            break;
        case AllocatorType::BUDDY:
            buddy_allocator_->deallocate(ptr);
            break;
    }
    
    deallocation_count_++;
    updateStatistics(type, 0, false);
}

size_t HybridAllocator::getFragmentation() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    size_t total_memory = 0;
    size_t fragmented_memory = 0;
    
    // Calculate fragmentation from buddy allocator
    size_t buddy_frag = buddy_allocator_->getFragmentation();
    size_t buddy_total = buddy_allocator_->getTotalMemory();
    total_memory += buddy_total;
    fragmented_memory += (buddy_frag * buddy_total) / 100;
    
    // Calculate fragmentation from pool allocators
    for (const auto& pool : pool_allocators_) {
        size_t pool_frag = pool->getFragmentation();
        size_t pool_total = pool->getTotalMemory();
        total_memory += pool_total;
        fragmented_memory += (pool_frag * pool_total) / 100;
    }
    
    // Calculate fragmentation from slab allocators
    for (const auto& slab : slab_allocators_) {
        size_t slab_frag = slab->getFragmentation();
        size_t slab_total = slab->getTotalMemory();
        total_memory += slab_total;
        fragmented_memory += (slab_frag * slab_total) / 100;
    }
    
    return total_memory > 0 ? (fragmented_memory * 100) / total_memory : 0;
}

std::string HybridAllocator::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string stats = MemoryAllocator::getStats();
    stats += "Hybrid Allocator Stats:\n";
    stats += "  Pool Allocations: " + std::to_string(pool_stats_.allocations) + "\n";
    stats += "  Slab Allocations: " + std::to_string(slab_stats_.allocations) + "\n";
    stats += "  Buddy Allocations: " + std::to_string(buddy_stats_.allocations) + "\n";
    stats += "  Pool Memory: " + std::to_string(pool_stats_.total_allocated) + " bytes\n";
    stats += "  Slab Memory: " + std::to_string(slab_stats_.total_allocated) + " bytes\n";
    stats += "  Buddy Memory: " + std::to_string(buddy_stats_.total_allocated) + " bytes\n";
    
    // Add individual allocator stats
    stats += "\nBuddy Allocator:\n" + buddy_allocator_->getStats() + "\n";
    
    for (size_t i = 0; i < pool_allocators_.size(); ++i) {
        stats += "\nPool Allocator " + std::to_string(i) + ":\n";
        stats += pool_allocators_[i]->getStats() + "\n";
    }
    
    for (size_t i = 0; i < slab_allocators_.size(); ++i) {
        stats += "\nSlab Allocator " + std::to_string(i) + ":\n";
        stats += slab_allocators_[i]->getStats() + "\n";
    }
    
    return stats;
}

HybridAllocator::AllocatorType HybridAllocator::selectAllocator(size_t size) {
    // Use pool allocator for small, fixed-size allocations
    if (size <= config_.pool_max_size) {
        for (const auto& pool : pool_allocators_) {
            if (pool->canAllocate(size)) {
                return AllocatorType::POOL;
            }
        }
    }
    
    // Use slab allocator for medium-sized, frequent allocations
    if (size <= config_.slab_max_size) {
        for (const auto& slab : slab_allocators_) {
            if (size <= slab->getObjectSize()) {
                return AllocatorType::SLAB;
            }
        }
    }
    
    // Use buddy allocator for large or variable-size allocations
    return AllocatorType::BUDDY;
}

void* HybridAllocator::allocateFromPool(size_t size) {
    // Try to find suitable pool allocator
    for (auto& pool : pool_allocators_) {
        if (pool->canAllocate(size)) {
            return pool->allocate(size);
        }
    }
    return nullptr;
}

void* HybridAllocator::allocateFromSlab(size_t size) {
    // Find smallest suitable slab allocator
    SlabAllocator* best_slab = nullptr;
    size_t best_size = SIZE_MAX;
    
    for (auto& slab : slab_allocators_) {
        size_t obj_size = slab->getObjectSize();
        if (obj_size >= size && obj_size < best_size) {
            best_slab = slab.get();
            best_size = obj_size;
        }
    }
    
    if (best_slab) {
        return best_slab->allocate(size);
    }
    return nullptr;
}

void HybridAllocator::deallocateFromPool(void* ptr) {
    for (auto& pool : pool_allocators_) {
        if (pool->isValidPointer(ptr)) {
            pool->deallocate(ptr);
            return;
        }
    }
}

void HybridAllocator::deallocateFromSlab(void* ptr) {
    for (auto& slab : slab_allocators_) {
        // Try to deallocate from each slab (they will check if pointer belongs to them)
        size_t old_count = slab->getDeallocationCount();
        slab->deallocate(ptr);
        if (slab->getDeallocationCount() > old_count) {
            return; // Successfully deallocated
        }
    }
}

void HybridAllocator::createPoolAllocators(size_t total_memory) {
    // Create pool allocators for common small sizes
    std::vector<size_t> pool_sizes = {8, 16, 32, 64, 128, 256};
    size_t memory_per_pool = total_memory / pool_sizes.size();
    
    for (size_t block_size : pool_sizes) {
        size_t num_blocks = memory_per_pool / block_size;
        if (num_blocks > 0) {
            PoolAllocator::PoolConfig config;
            config.block_sizes = {block_size};
            config.blocks_per_pool = {num_blocks};
            config.total_memory = memory_per_pool;
            
            auto pool = std::make_unique<PoolAllocator>(config);
            pool_allocators_.push_back(std::move(pool));
        }
    }
}

void HybridAllocator::createSlabAllocators(size_t total_memory) {
    // Create slab allocators for common object sizes
    std::vector<std::pair<size_t, size_t>> slab_configs = {
        {64, 32},   // 64-byte objects, 32 per slab
        {128, 24},  // 128-byte objects, 24 per slab
        {256, 16},  // 256-byte objects, 16 per slab
        {512, 8}    // 512-byte objects, 8 per slab
    };
    
    size_t memory_per_slab = total_memory / slab_configs.size();
    
    for (const auto& config : slab_configs) {
        size_t object_size = config.first;
        size_t objects_per_slab = config.second;
        
        auto slab = std::make_unique<SlabAllocator>(object_size, objects_per_slab, memory_per_slab);
        slab_allocators_.push_back(std::move(slab));
    }
}

void HybridAllocator::updateStatistics(AllocatorType type, size_t size, bool allocation) {
    AllocatorStats* stats = nullptr;
    
    switch (type) {
        case AllocatorType::POOL:
            stats = &pool_stats_;
            break;
        case AllocatorType::SLAB:
            stats = &slab_stats_;
            break;
        case AllocatorType::BUDDY:
            stats = &buddy_stats_;
            break;
    }
    
    if (stats) {
        if (allocation) {
            stats->allocations++;
            stats->total_allocated += size;
        } else {
            stats->deallocations++;
        }
    }
}

std::vector<MemoryAllocator::MemoryBlock> HybridAllocator::getMemoryLayout() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<MemoryBlock> layout;
    size_t offset = 0;
    
    // Add buddy allocator layout
    auto buddy_layout = buddy_allocator_->getMemoryLayout();
    for (auto& block : buddy_layout) {
        block.address += offset;
        block.type = "Buddy: " + block.type;
        layout.push_back(block);
    }
    offset += buddy_allocator_->getTotalMemory();
    
    // Add pool allocator layouts
    for (size_t i = 0; i < pool_allocators_.size(); ++i) {
        auto pool_layout = pool_allocators_[i]->getMemoryLayout();
        for (auto& block : pool_layout) {
            block.address += offset;
            block.type = "Pool" + std::to_string(i) + ": " + block.type;
            layout.push_back(block);
        }
        offset += pool_allocators_[i]->getTotalMemory();
    }
    
    // Add slab allocator layouts
    for (size_t i = 0; i < slab_allocators_.size(); ++i) {
        auto slab_layout = slab_allocators_[i]->getMemoryLayout();
        for (auto& block : slab_layout) {
            block.address += offset;
            block.type = "Slab" + std::to_string(i) + ": " + block.type;
            layout.push_back(block);
        }
        offset += slab_allocators_[i]->getTotalMemory();
    }
    
    return layout;
}

void HybridAllocator::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Reset base class statistics
    allocated_size_ = 0;
    allocation_count_ = 0;
    deallocation_count_ = 0;
    
    // Clear allocation map
    allocation_map_.clear();
    
    // Reset allocator-specific statistics
    pool_stats_ = {};
    slab_stats_ = {};
    buddy_stats_ = {};
    
    // Reset individual allocators
    buddy_allocator_->reset();
    for (auto& pool : pool_allocators_) {
        pool->reset();
    }
    // Note: Slab allocators don't have reset method in our interface
}

double HybridAllocator::getEfficiencyScore() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Calculate efficiency based on fragmentation and utilization
    double fragmentation = static_cast<double>(getFragmentation()) / 100.0;
    double utilization = static_cast<double>(allocated_size_) / static_cast<double>(total_memory_);
    
    // Efficiency score: high utilization, low fragmentation
    return utilization * (1.0 - fragmentation);
}
