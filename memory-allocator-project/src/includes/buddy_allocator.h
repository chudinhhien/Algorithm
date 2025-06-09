#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include "memory_allocator.h"
#include <map>
#include <list>
#include <mutex>

/**
 * @brief Buddy System Memory Allocator
 * 
 * Triển khai thuật toán Buddy System:
 * - Chia bộ nhớ thành các khối có kích thước là lũy thừa của 2
 * - Khi cần allocation, tìm khối nhỏ nhất phù hợp và chia đôi nếu cần
 * - Khi deallocate, gộp với buddy block nếu có thể (coalescing)
 * - Giảm fragmentation ngoại bộ hiệu quả
 */
class BuddyAllocator : public MemoryAllocator {
private:
    struct BuddyBlock {
        size_t size;
        bool is_free;
        void* address;
        BuddyBlock* buddy;
        BuddyBlock* parent;
        BuddyBlock* left_child;
        BuddyBlock* right_child;
        int level; // Level trong buddy tree

        BuddyBlock(size_t s, void* addr, int lvl) 
            : size(s), is_free(true), address(addr), buddy(nullptr), 
              parent(nullptr), left_child(nullptr), right_child(nullptr), level(lvl) {}
    };

public:
    explicit BuddyAllocator(size_t initial_size = 1024 * 1024);
    ~BuddyAllocator() override;    // Core allocation methods
    void* allocate(size_t size) override;
    void deallocate(void* ptr) override;

    // Statistics and info
    size_t getFragmentation() const override;
    std::string getStats() const override;
    std::vector<MemoryAllocator::MemoryBlock> getMemoryLayout() const override;
    
    // Memory management
    void reset() override;
    
    // Buddy-specific methods
    size_t get_min_block_size() const { return min_block_size_; }
    size_t get_max_block_size() const { return max_block_size_; }
    int get_current_max_level() const;  // Changed from get_max_level() to reflect dynamic nature
    
    // Visualization and debugging
    void print_buddy_tree() const;
    std::vector<std::pair<void*, size_t>> get_free_blocks() const;
    std::vector<std::pair<void*, size_t>> get_allocated_blocks() const;

private:
    // Helper methods
    size_t next_power_of_2(size_t size) const;
    int get_level_for_size(size_t size) const;
    BuddyBlock* find_free_block(size_t size);
    BuddyBlock* split_block(BuddyBlock* block, size_t target_size);
    void coalesce_block(BuddyBlock* block);
    BuddyBlock* get_buddy(BuddyBlock* block) const;
    BuddyBlock* find_block_by_address(void* addr) const;
    
    // Tree traversal helpers
    void print_tree_recursive(BuddyBlock* node, int depth) const;
    void collect_free_blocks(BuddyBlock* node, std::vector<std::pair<void*, size_t>>& blocks) const;
    void collect_allocated_blocks(BuddyBlock* node, std::vector<std::pair<void*, size_t>>& blocks) const;

private:
    BuddyBlock* root_block_;           // Root của buddy tree
    void* memory_pool_;                // Memory pool pointer
    size_t min_block_size_;            // Kích thước block nhỏ nhất (thường là 32 bytes)
    size_t max_block_size_;            // Kích thước block lớn nhất (= total_size)
    
    // Free lists for different block sizes (dynamic based on splits)
    std::map<int, std::list<BuddyBlock*>> free_lists_;
    
    // Map để track allocated blocks
    std::map<void*, BuddyBlock*> allocated_blocks_;
    
    // Thread safety
    mutable std::mutex allocator_mutex_;
    
    // Statistics
    size_t total_splits_;              // Số lần split block
    size_t total_coalesces_;           // Số lần coalesce block
    size_t failed_coalesces_;          // Số lần coalesce thất bại
};

#endif // BUDDY_ALLOCATOR_H
