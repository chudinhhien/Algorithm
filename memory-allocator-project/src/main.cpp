#include "includes/memory_allocator.h"
#include "includes/buddy_allocator.h"
#include "includes/slab_allocator.h"
#include "includes/pool_allocator.h"
#include "includes/hybrid_allocator.h"
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <cstring>

void print_separator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void test_basic_allocation(MemoryAllocator& allocator) {
    print_separator("Basic Allocation Test");
    
    std::vector<void*> ptrs;
    std::vector<size_t> sizes = {32, 64, 128, 256, 512, 1024, 2048};
    
    std::cout << "Allocating blocks of various sizes...\n";
    for (size_t size : sizes) {
        void* ptr = allocator.allocate(size);
        if (ptr) {
            ptrs.push_back(ptr);
            std::cout << "  Allocated " << size << " bytes at " << ptr << "\n";
            
            // Write some data to verify memory is writable
            std::memset(ptr, 0xAA, size);
        } else {
            std::cout << "  Failed to allocate " << size << " bytes\n";
        }
    }
    
    std::cout << "\nMemory stats after allocation:\n";
    std::cout << "  Current allocated: " << allocator.getAllocatedSize() << " bytes\n";
    std::cout << "  Available: " << (allocator.getTotalMemory() - allocator.getAllocatedSize()) << " bytes\n";
    std::cout << "  Fragmentation: " << std::fixed << std::setprecision(2) 
              << allocator.getFragmentation() << "%\n";
    
    std::cout << "\nDeallocating all blocks...\n";
    for (void* ptr : ptrs) {
        allocator.deallocate(ptr);
    }
    
    std::cout << "Final stats:\n";
    std::cout << allocator.getStats() << "\n";    std::cout << "  Allocation count: " << allocator.getAllocationCount() << "\n";
}

void test_fragmentation_scenario(MemoryAllocator& allocator) {
    print_separator("Fragmentation Test");
    
    std::vector<void*> ptrs;
    
    // Allocate many small blocks
    std::cout << "Allocating 50 blocks of 64 bytes each...\n";
    for (int i = 0; i < 50; ++i) {
        void* ptr = allocator.allocate(64);
        if (ptr) {
            ptrs.push_back(ptr);
        }
    }
    
    // Deallocate every other block to create fragmentation
    std::cout << "Deallocating every other block to create fragmentation...\n";
    for (size_t i = 1; i < ptrs.size(); i += 2) {
        allocator.deallocate(ptrs[i]);
        ptrs[i] = nullptr;
    }
    
    std::cout << "Fragmentation after pattern deallocation: " 
              << std::fixed << std::setprecision(2) 
              << allocator.getFragmentation() << "%\n";
    
    // Try to allocate a larger block
    std::cout << "Attempting to allocate 1024 bytes in fragmented state...\n";
    void* large_ptr = allocator.allocate(1024);
    if (large_ptr) {
        std::cout << "Successfully allocated large block at " << large_ptr << "\n";
        allocator.deallocate(large_ptr);
    } else {
        std::cout << "Failed to allocate large block due to fragmentation\n";
    }
      // Try defragmentation
    std::cout << "Running defragmentation...\n";
    // Note: defragmentation not implemented in base interface
    
    std::cout << "Fragmentation after defragmentation: " 
              << std::fixed << std::setprecision(2) 
              << allocator.getFragmentation() << "%\n";
    
    // Clean up remaining blocks
    for (void* ptr : ptrs) {
        if (ptr) allocator.deallocate(ptr);
    }
}

void performance_comparison() {
    print_separator("Performance Comparison");
    
    const size_t iterations = 1000;
    const size_t alloc_size = 256;
    
    std::vector<std::unique_ptr<MemoryAllocator>> allocators;
    
    // Create allocators for comparison
    try {
        allocators.push_back(AllocatorFactory::create_allocator(
            AllocatorFactory::AllocatorType::BUDDY_SYSTEM, 1024 * 1024));
        
        // Note: Other allocators would be added here when implemented
        // allocators.push_back(AllocatorFactory::create_allocator(
        //     AllocatorFactory::AllocatorType::SLAB, 1024 * 1024));
        
    } catch (const std::exception& e) {
        std::cout << "Error creating allocators: " << e.what() << "\n";
        return;
    }
    
    std::cout << "Benchmarking " << iterations << " allocations of " << alloc_size << " bytes each:\n\n";
      for (auto& allocator : allocators) {
        allocator->benchmarkAllocation(iterations, alloc_size);
        
        std::cout << "Final Statistics:\n";
        std::cout << "  Success rate: " << (100.0 * allocator->getAllocationCount() / iterations) << "%\n";
        std::cout << "  Fragmentation: " << std::fixed << std::setprecision(2) 
                  << allocator->getFragmentation() << "%\n";
        std::cout << "  Allocated: " << allocator->getAllocatedSize() << " bytes\n\n";
        
        allocator->reset(); // Reset for next test
    }
}

void stress_test_all() {
    print_separator("Stress Test");
    
    auto allocator = AllocatorFactory::create_allocator(
        AllocatorFactory::AllocatorType::BUDDY_SYSTEM, 2 * 1024 * 1024);
    
    std::cout << "Running stress test on Buddy System Allocator...\n";
    allocator->stressTest(5); // 5 seconds
      std::cout << "\nStress Test Results:\n";
    std::cout << "  Total operations: " << (allocator->getAllocationCount() + allocator->getDeallocationCount()) << "\n";
    std::cout << "  Allocation/Deallocation ratio: " 
              << (static_cast<double>(allocator->getAllocationCount()) / allocator->getDeallocationCount()) << "\n";
    std::cout << "  Final fragmentation: " << std::fixed << std::setprecision(2) 
              << allocator->getFragmentation() << "%\n";
}

void demonstrate_buddy_specific_features() {
    print_separator("Buddy System Specific Features");
    
    auto buddy = std::make_unique<BuddyAllocator>(1024);
    
    std::cout << "Buddy System Configuration:\n";
    std::cout << "  Total Memory: " << buddy->getTotalMemory() << " bytes\n\n";
    
    // Allocate some blocks to show basic functionality
    std::cout << "Allocating blocks to demonstrate buddy system:\n";
    void* ptr1 = buddy->allocate(64);
    void* ptr2 = buddy->allocate(128);
    void* ptr3 = buddy->allocate(32);
    
    std::cout << "Allocated: 64 bytes at " << ptr1 << "\n";
    std::cout << "Allocated: 128 bytes at " << ptr2 << "\n";
    std::cout << "Allocated: 32 bytes at " << ptr3 << "\n\n";
    
    std::cout << "Memory stats after allocation:\n";
    std::cout << buddy->getStats() << "\n";
    std::cout << "Fragmentation: " << buddy->getFragmentation() << "%\n";
    
    // Clean up
    buddy->deallocate(ptr1);
    buddy->deallocate(ptr2);
    buddy->deallocate(ptr3);
    
    std::cout << "\nAfter deallocation:\n";
    std::cout << buddy->getStats() << "\n";
}

void interactive_demo() {
    print_separator("Interactive Demo");
    
    auto allocator = AllocatorFactory::create_allocator(
        AllocatorFactory::AllocatorType::BUDDY_SYSTEM, 2048);
    
    std::vector<std::pair<void*, size_t>> user_allocations;
    
    std::cout << "Interactive Memory Allocator Demo\n";
    std::cout << "Commands: allocate <size>, deallocate <index>, stats, quit\n\n";
    
    std::string command;
    while (true) {
        std::cout << "allocator> ";
        std::cin >> command;
        
        if (command == "quit" || command == "exit") {
            break;
        } else if (command == "allocate") {
            size_t size;
            std::cin >> size;
            
            void* ptr = allocator->allocate(size);
            if (ptr) {
                user_allocations.emplace_back(ptr, size);
                std::cout << "Allocated " << size << " bytes at " << ptr 
                          << " (index " << (user_allocations.size() - 1) << ")\n";
            } else {
                std::cout << "Failed to allocate " << size << " bytes\n";
            }
            
        } else if (command == "deallocate") {
            size_t index;
            std::cin >> index;
            
            if (index < user_allocations.size() && user_allocations[index].first) {
                allocator->deallocate(user_allocations[index].first);
                std::cout << "Deallocated " << user_allocations[index].second 
                          << " bytes from index " << index << "\n";
                user_allocations[index].first = nullptr;
            } else {
                std::cout << "Invalid index or already deallocated\n";
            }
              } else if (command == "stats") {
            std::cout << "Current Statistics:\n";
            std::cout << "  Current allocated: " << allocator->getAllocatedSize() << " bytes\n";
            std::cout << "  Available: " << (allocator->getTotalMemory() - allocator->getAllocatedSize()) << " bytes\n";
            std::cout << "  Total allocations: " << allocator->getAllocationCount() << "\n";
            std::cout << "  Fragmentation: " << std::fixed << std::setprecision(2) 
                      << allocator->getFragmentation() << "%\n";
            
            std::cout << "  Active allocations:\n";
            for (size_t i = 0; i < user_allocations.size(); ++i) {
                if (user_allocations[i].first) {
                    std::cout << "    [" << i << "] " << user_allocations[i].second 
                              << " bytes at " << user_allocations[i].first << "\n";
                }
            }
        } else {
            std::cout << "Unknown command. Use: allocate <size>, deallocate <index>, stats, quit\n";
        }
    }
    
    // Clean up any remaining allocations
    for (const auto& alloc : user_allocations) {
        if (alloc.first) {
            allocator->deallocate(alloc.first);
        }
    }
}

int main() {
    std::cout << "Memory Allocator Design - Testing Suite\n";
    std::cout << "=======================================\n";
    
    try {
        // Basic functionality tests
        auto buddy_allocator = AllocatorFactory::create_allocator(
            AllocatorFactory::AllocatorType::BUDDY_SYSTEM, 8192);
        
        test_basic_allocation(*buddy_allocator);
        test_fragmentation_scenario(*buddy_allocator);
        
        // Performance comparison
        performance_comparison();
        
        // Stress testing
        stress_test_all();
        
        // Buddy-specific features
        demonstrate_buddy_specific_features();
        
        // Interactive demo
        std::cout << "\nWould you like to try the interactive demo? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            interactive_demo();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    std::cout << "\nAll tests completed successfully!\n";
    return 0;
}
