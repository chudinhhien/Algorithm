#include "../includes/buddy_allocator.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <sstream>

BuddyAllocator::BuddyAllocator(size_t initial_size)
    : MemoryAllocator(initial_size),
      root_block_(nullptr), min_block_size_(32), 
      total_splits_(0), total_coalesces_(0), failed_coalesces_(0) {
    
    // Ensure initial_size is power of 2
    max_block_size_ = next_power_of_2(initial_size);
    if (max_block_size_ != initial_size) {
        // Update total memory to correct size
        total_memory_ = max_block_size_;
    }
    
    // Allocate memory pool
    memory_pool_ = std::malloc(max_block_size_);
    if (!memory_pool_) {
        throw std::bad_alloc();
    }
    
    // Create root block at level 0 - tree will grow dynamically as needed
    root_block_ = new BuddyBlock(max_block_size_, memory_pool_, 0);
    
    // Initialize free list for root level only - other levels created as needed
    free_lists_[0] = std::list<BuddyBlock*>();    
    // Add root block to level 0 free list
    free_lists_[0].push_back(root_block_);
    
    std::cout << "Buddy Allocator initialized:\n";
    std::cout << "  Total size: " << max_block_size_ << " bytes\n";
    std::cout << "  Min block size: " << min_block_size_ << " bytes\n";
    std::cout << "  Tree grows dynamically based on allocations\n";
}

BuddyAllocator::~BuddyAllocator() {
    std::cout << "Buddy Allocator Statistics:\n";
    std::cout << "  Total splits: " << total_splits_ << "\n";
    std::cout << "  Total coalesces: " << total_coalesces_ << "\n";
    std::cout << "  Failed coalesces: " << failed_coalesces_ << "\n";
    
    // Clean up buddy tree (recursive deletion handled by destructor)
    delete root_block_;
    
    // Free memory pool
    if (memory_pool_) {
        std::free(memory_pool_);
    }
}

void* BuddyAllocator::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(allocator_mutex_);
    
    if (size == 0) return nullptr;
    
    // Find appropriate block size (next power of 2, at least min_block_size)
    size_t block_size = std::max(next_power_of_2(size), min_block_size_);
    
    // Find a free block
    BuddyBlock* block = find_free_block(block_size);
    if (!block) {
        return nullptr; // Out of memory
    }
    
    // Split block if necessary
    if (block->size > block_size) {
        block = split_block(block, block_size);
    }
    
    // Mark block as allocated
    block->is_free = false;
    allocated_blocks_[block->address] = block;
    
    // Update statistics
    allocated_size_ += block->size;
    allocation_count_++;
    
    return block->address;
}

void BuddyAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(allocator_mutex_);
    
    // Find the block
    auto it = allocated_blocks_.find(ptr);
    if (it == allocated_blocks_.end()) {
        std::cerr << "Error: Trying to deallocate unallocated pointer\n";
        return;
    }
    
    BuddyBlock* block = it->second;
    allocated_blocks_.erase(it);
    
    // Mark as free
    block->is_free = true;
    
    // Add to appropriate free list (create if needed)
    int level = get_level_for_size(block->size);
    if (free_lists_.find(level) == free_lists_.end()) {
        free_lists_[level] = std::list<BuddyBlock*>();
    }
    free_lists_[level].push_back(block);
    
    // Try to coalesce with buddy
    coalesce_block(block);
    
    // Update statistics
    allocated_size_ -= block->size;
    deallocation_count_++;
}

// Helper method implementations
size_t BuddyAllocator::next_power_of_2(size_t size) const {
    if (size <= 1) return 1;
    
    size_t power = 1;
    while (power < size) {
        power <<= 1;
    }
    return power;
}

int BuddyAllocator::get_level_for_size(size_t size) const {
    // Calculate level dynamically based on size ratio
    // Level 0 = max_block_size_, Level 1 = max_block_size_/2, etc.
    return static_cast<int>(std::log2(max_block_size_ / size));
}

int BuddyAllocator::get_current_max_level() const {
    // Return the deepest level that currently exists in the tree
    // This is determined by finding the highest level key in free_lists
    // or by traversing the tree to find the deepest allocated block
    int max_level = 0;
    for (const auto& pair : free_lists_) {
        if (!pair.second.empty()) {
            max_level = std::max(max_level, pair.first);
        }
    }
    
    // Also check allocated blocks for deeper levels
    for (const auto& pair : allocated_blocks_) {
        BuddyBlock* block = pair.second;
        max_level = std::max(max_level, block->level);
    }
    
    return max_level;
}

BuddyAllocator::BuddyBlock* BuddyAllocator::find_free_block(size_t size) {
    int target_level = get_level_for_size(size);
    
    // Search from target level up to root
    for (int level = target_level; level >= 0; --level) {
        auto& free_list = free_lists_[level];
        if (!free_list.empty()) {
            BuddyBlock* block = free_list.front();
            free_list.pop_front();
            return block;
        }
    }
    
    return nullptr; // No suitable block found
}

BuddyAllocator::BuddyBlock* BuddyAllocator::split_block(BuddyBlock* block, size_t target_size) {
    while (block->size > target_size) {
        total_splits_++;
        
        size_t half_size = block->size / 2;
        int child_level = block->level + 1;
        
        // Create left and right children
        block->left_child = new BuddyBlock(half_size, block->address, child_level);
        block->right_child = new BuddyBlock(half_size, 
            static_cast<char*>(block->address) + half_size, child_level);
        
        // Set up parent-child relationships
        block->left_child->parent = block;
        block->right_child->parent = block;
        block->left_child->buddy = block->right_child;
        block->right_child->buddy = block->left_child;
        
        // Create free list for this level if it doesn't exist
        if (free_lists_.find(child_level) == free_lists_.end()) {
            free_lists_[child_level] = std::list<BuddyBlock*>();
        }
        
        // Add right child to free list (we'll continue splitting left child)
        free_lists_[child_level].push_back(block->right_child);
        
        // Continue with left child
        block = block->left_child;
    }
    
    return block;
}

void BuddyAllocator::coalesce_block(BuddyBlock* block) {
    while (block->parent && block->buddy && block->buddy->is_free) {
        total_coalesces_++;
        
        // Remove buddy from free list
        int level = block->level;
        auto& free_list = free_lists_[level];
        auto buddy_it = std::find(free_list.begin(), free_list.end(), block->buddy);
        if (buddy_it != free_list.end()) {
            free_list.erase(buddy_it);
        }
        
        // Remove current block from free list
        auto block_it = std::find(free_list.begin(), free_list.end(), block);
        if (block_it != free_list.end()) {
            free_list.erase(block_it);
        }
        
        // Mark parent as free and add to appropriate free list
        BuddyBlock* parent = block->parent;
        parent->is_free = true;
        
        // Create free list for parent level if needed
        if (free_lists_.find(parent->level) == free_lists_.end()) {
            free_lists_[parent->level] = std::list<BuddyBlock*>();
        }
        free_lists_[parent->level].push_back(parent);
        
        // Continue coalescing with parent
        block = parent;
    }
}

BuddyAllocator::BuddyBlock* BuddyAllocator::find_block_by_address(void* addr) const {
    // This is a simplified implementation - in practice, you'd traverse the tree
    auto it = allocated_blocks_.find(addr);
    return (it != allocated_blocks_.end()) ? it->second : nullptr;
}

void BuddyAllocator::print_buddy_tree() const {
    std::cout << "Buddy Tree Structure:\n";
    print_tree_recursive(root_block_, 0);
}

void BuddyAllocator::print_tree_recursive(BuddyBlock* node, int depth) const {
    if (!node) return;
    
    std::cout << std::string(depth * 2, ' ') 
              << "Level " << node->level 
              << ": " << node->size << " bytes"
              << (node->is_free ? " [FREE]" : " [ALLOCATED]")
              << " @" << node->address << "\n";
    
    if (node->left_child) {
        print_tree_recursive(node->left_child, depth + 1);
    }
    if (node->right_child) {
        print_tree_recursive(node->right_child, depth + 1);
    }
}

std::vector<std::pair<void*, size_t>> BuddyAllocator::get_free_blocks() const {
    std::vector<std::pair<void*, size_t>> free_blocks;
    collect_free_blocks(root_block_, free_blocks);
    return free_blocks;
}

std::vector<std::pair<void*, size_t>> BuddyAllocator::get_allocated_blocks() const {
    std::vector<std::pair<void*, size_t>> allocated_blocks;
    collect_allocated_blocks(root_block_, allocated_blocks);
    return allocated_blocks;
}

void BuddyAllocator::collect_free_blocks(BuddyBlock* node, std::vector<std::pair<void*, size_t>>& blocks) const {
    if (!node) return;
    
    if (node->is_free && !node->left_child && !node->right_child) {
        blocks.emplace_back(node->address, node->size);
    }
    
    if (node->left_child) {
        collect_free_blocks(node->left_child, blocks);
    }
    if (node->right_child) {
        collect_free_blocks(node->right_child, blocks);
    }
}

void BuddyAllocator::collect_allocated_blocks(BuddyBlock* node, std::vector<std::pair<void*, size_t>>& blocks) const {
    if (!node) return;
    
    if (!node->is_free && !node->left_child && !node->right_child) {
        blocks.emplace_back(node->address, node->size);
    }
    
    if (node->left_child) {
        collect_allocated_blocks(node->left_child, blocks);
    }
    if (node->right_child) {
        collect_allocated_blocks(node->right_child, blocks);
    }
}

std::vector<MemoryAllocator::MemoryBlock> BuddyAllocator::getMemoryLayout() const {
    std::vector<MemoryAllocator::MemoryBlock> blocks;
    
    // This is a simplified implementation
    // In a real buddy allocator, you'd traverse the buddy tree
    MemoryAllocator::MemoryBlock block;
    block.address = reinterpret_cast<size_t>(memory_pool_);
    block.size = total_memory_;
    block.is_free = (allocated_size_ == 0);
    block.type = "buddy";
    
    blocks.push_back(block);
    return blocks;
}

std::string BuddyAllocator::getStats() const {
    std::ostringstream oss;
    oss << "Buddy System Allocator Statistics:\n";
    oss << "  Total Memory: " << total_memory_ << " bytes\n";
    oss << "  Allocated: " << allocated_size_ << " bytes\n";
    oss << "  Free: " << (total_memory_ - allocated_size_) << " bytes\n";
    oss << "  Allocations: " << allocation_count_ << "\n";
    oss << "  Deallocations: " << deallocation_count_ << "\n";
    oss << "  Fragmentation: " << getFragmentation() << "%\n";
    return oss.str();
}

size_t BuddyAllocator::getFragmentation() const {
    // Simple fragmentation calculation
    // In a real buddy allocator, this would analyze the buddy tree
    if (total_memory_ == 0) return 0;
    
    size_t free_memory = total_memory_ - allocated_size_;
    if (free_memory == 0) return 0;
    
    // Simple heuristic: fragmentation is based on number of allocations vs free space
    if (allocation_count_ > deallocation_count_ && free_memory > 0) {
        return static_cast<size_t>((100.0 * (allocation_count_ - deallocation_count_)) / allocation_count_);
    }
    
    return 0; // No fragmentation if no active allocations
}

void BuddyAllocator::reset() {
    // Reset statistics
    allocated_size_ = 0;
    allocation_count_ = 0;
    deallocation_count_ = 0;
    
    // In a real implementation, would reinitialize the buddy tree
    // For now, just reset counters
}
