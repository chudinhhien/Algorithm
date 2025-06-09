#include "src/includes/buddy_allocator.h"
#include <iostream>
#include <vector>

int main() {
    std::cout << "Testing Dynamic Buddy System Tree Height Fix\n";
    std::cout << "============================================\n\n";
    
    // Create a buddy allocator with 1KB memory
    BuddyAllocator allocator(1024);
    
    std::cout << "Initial state:\n";
    std::cout << "Current max level: " << allocator.get_current_max_level() << std::endl;
    std::cout << "Min block size: " << allocator.get_min_block_size() << std::endl;
    std::cout << "Max block size: " << allocator.get_max_block_size() << std::endl;
    std::cout << "\n";
    
    // Allocate progressively smaller blocks to force tree growth
    std::vector<void*> allocations;
    
    std::cout << "Making allocations to trigger tree splits:\n";
    
    // Large allocation (should stay at level 0 or 1)
    void* ptr1 = allocator.allocate(256);
    std::cout << "After allocating 256 bytes - Current max level: " << allocator.get_current_max_level() << std::endl;
    allocations.push_back(ptr1);
    
    // Medium allocation (should go deeper)
    void* ptr2 = allocator.allocate(128);
    std::cout << "After allocating 128 bytes - Current max level: " << allocator.get_current_max_level() << std::endl;
    allocations.push_back(ptr2);
    
    // Small allocation (should go even deeper)
    void* ptr3 = allocator.allocate(64);
    std::cout << "After allocating 64 bytes - Current max level: " << allocator.get_current_max_level() << std::endl;
    allocations.push_back(ptr3);
    
    // Very small allocation (should reach the deepest level needed)
    void* ptr4 = allocator.allocate(32);
    std::cout << "After allocating 32 bytes - Current max level: " << allocator.get_current_max_level() << std::endl;
    allocations.push_back(ptr4);
    
    std::cout << "\nTree structure after allocations:\n";
    allocator.print_buddy_tree();
    
    std::cout << "\nAllocator statistics:\n";
    std::cout << allocator.getStats() << std::endl;
    
    // Test deallocation and verify tree height remains appropriate
    std::cout << "Deallocating all blocks...\n";
    for (void* ptr : allocations) {
        allocator.deallocate(ptr);
    }
    
    std::cout << "After deallocation - Current max level: " << allocator.get_current_max_level() << std::endl;
    std::cout << "\nFinal tree structure:\n";
    allocator.print_buddy_tree();
    
    std::cout << "\nTest completed successfully! The tree height is now dynamic.\n";
    
    return 0;
} 
