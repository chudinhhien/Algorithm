#include "../src/includes/buddy_allocator.h"
#include "../src/includes/slab_allocator.h"
#include "../src/includes/pool_allocator.h"
#include "../src/includes/hybrid_allocator.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <chrono>

class TestRunner {
public:
    static void runAllTests() {
        std::cout << "Starting Memory Allocator Unit Tests...\n\n";
        
        testBuddyAllocator();
        testSlabAllocator();
        testPoolAllocator();
        testHybridAllocator();
        
        std::cout << "\nAll tests completed successfully!\n";
    }

private:
    static void testBuddyAllocator() {
        std::cout << "Testing Buddy Allocator...\n";
        
        BuddyAllocator allocator(1024);
        
        // Test basic allocation
        void* ptr1 = allocator.allocate(64);
        assert(ptr1 != nullptr);
        
        void* ptr2 = allocator.allocate(128);
        assert(ptr2 != nullptr);
        
        void* ptr3 = allocator.allocate(256);
        assert(ptr3 != nullptr);
        
        // Test deallocation
        allocator.deallocate(ptr1);
        allocator.deallocate(ptr2);
        
        // Test coalescing by reallocating
        void* ptr4 = allocator.allocate(64);
        assert(ptr4 != nullptr);
        
        allocator.deallocate(ptr3);
        allocator.deallocate(ptr4);
        
        // Test statistics
        std::string stats = allocator.getStats();
        assert(!stats.empty());
        
        std::cout << "  ✓ Buddy Allocator tests passed\n";
    }
    
    static void testSlabAllocator() {
        std::cout << "Testing Slab Allocator...\n";
        
        SlabAllocator allocator(64, 16, 2048); // 64-byte objects, 16 per slab
        
        // Test allocation of correct size
        void* ptr1 = allocator.allocate(64);
        assert(ptr1 != nullptr);
        
        void* ptr2 = allocator.allocate(32); // Should still work (fits in 64-byte object)
        assert(ptr2 != nullptr);
        
        // Test allocation too large
        void* ptr3 = allocator.allocate(128);
        assert(ptr3 == nullptr); // Should fail
        
        // Test multiple allocations
        std::vector<void*> ptrs;
        for (int i = 0; i < 10; ++i) {
            void* ptr = allocator.allocate(64);
            if (ptr) ptrs.push_back(ptr);
        }
        assert(ptrs.size() >= 8); // Should allocate most of them
        
        // Test deallocation
        for (void* ptr : ptrs) {
            allocator.deallocate(ptr);
        }
        allocator.deallocate(ptr1);
        allocator.deallocate(ptr2);
        
        std::cout << "  ✓ Slab Allocator tests passed\n";
    }
    
    static void testPoolAllocator() {
        std::cout << "Testing Pool Allocator...\n";
        
        PoolAllocator allocator(128, 10, 2048); // 128-byte blocks, 10 blocks
        
        // Test basic allocation
        void* ptr1 = allocator.allocate(100);
        assert(ptr1 != nullptr);
        
        void* ptr2 = allocator.allocate(128);
        assert(ptr2 != nullptr);
        
        // Test allocation too large
        void* ptr3 = allocator.allocate(200);
        assert(ptr3 == nullptr);
        
        // Test multiple allocations
        std::vector<void*> ptrs;
        for (int i = 0; i < 8; ++i) {
            void* ptr = allocator.allocate(64);
            if (ptr) ptrs.push_back(ptr);
        }
        
        // Test deallocation
        allocator.deallocate(ptr1);
        allocator.deallocate(ptr2);
        for (void* ptr : ptrs) {
            allocator.deallocate(ptr);
        }
        
        // Test pool utilization
        assert(allocator.getAvailableBlocks() == 10);
        
        std::cout << "  ✓ Pool Allocator tests passed\n";
    }
    
    static void testHybridAllocator() {
        std::cout << "Testing Hybrid Allocator...\n";
        
        HybridAllocator allocator(4096);
        
        // Test small allocation (should use pool)
        void* ptr1 = allocator.allocate(64);
        assert(ptr1 != nullptr);
        
        // Test medium allocation (should use slab)
        void* ptr2 = allocator.allocate(512);
        assert(ptr2 != nullptr);
        
        // Test large allocation (should use buddy)
        void* ptr3 = allocator.allocate(2048);
        assert(ptr3 != nullptr);
        
        // Test mixed deallocations
        allocator.deallocate(ptr1);
        allocator.deallocate(ptr2);
        allocator.deallocate(ptr3);
        
        // Test statistics
        std::string stats = allocator.getStats();
        assert(!stats.empty());
        
        // Test efficiency score
        double efficiency = allocator.getEfficiencyScore();
        assert(efficiency >= 0.0 && efficiency <= 1.0);
        
        std::cout << "  ✓ Hybrid Allocator tests passed\n";
    }
};

// Performance benchmarks
class PerformanceTest {
public:
    static void runBenchmarks() {
        std::cout << "\nRunning Performance Benchmarks...\n\n";
        
        const size_t iterations = 10000;
        const size_t allocation_size = 64;
        
        benchmarkAllocator<BuddyAllocator>("Buddy", 8192, iterations, allocation_size);
        benchmarkSlabAllocator("Slab", iterations, allocation_size);
        benchmarkPoolAllocator("Pool", iterations, allocation_size);
        benchmarkHybridAllocator("Hybrid", iterations, allocation_size);
    }

private:
    template<typename AllocatorType>
    static void benchmarkAllocator(const std::string& name, size_t memory_size, 
                                 size_t iterations, size_t alloc_size) {
        AllocatorType allocator(memory_size);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<void*> ptrs;
        ptrs.reserve(iterations);
        
        // Allocation phase
        for (size_t i = 0; i < iterations; ++i) {
            void* ptr = allocator.allocate(alloc_size);
            if (ptr) ptrs.push_back(ptr);
        }
        
        // Deallocation phase
        for (void* ptr : ptrs) {
            allocator.deallocate(ptr);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << name << " Allocator: " << duration.count() << " μs for " 
                  << iterations << " operations\n";
        std::cout << "  Successful allocations: " << ptrs.size() << "/" << iterations << "\n";
        std::cout << "  Average time per operation: " 
                  << (double)duration.count() / iterations << " μs\n\n";
    }
    
    static void benchmarkSlabAllocator(const std::string& name, size_t iterations, size_t alloc_size) {
        SlabAllocator allocator(alloc_size, 32, 8192);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<void*> ptrs;
        ptrs.reserve(iterations);
        
        // Allocation phase
        for (size_t i = 0; i < iterations; ++i) {
            void* ptr = allocator.allocate(alloc_size);
            if (ptr) ptrs.push_back(ptr);
        }
        
        // Deallocation phase
        for (void* ptr : ptrs) {
            allocator.deallocate(ptr);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << name << " Allocator: " << duration.count() << " μs for " 
                  << iterations << " operations\n";
        std::cout << "  Successful allocations: " << ptrs.size() << "/" << iterations << "\n";
        std::cout << "  Average time per operation: " 
                  << (double)duration.count() / iterations << " μs\n\n";
    }
    
    static void benchmarkPoolAllocator(const std::string& name, size_t iterations, size_t alloc_size) {
        PoolAllocator allocator(alloc_size, iterations, alloc_size * iterations);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<void*> ptrs;
        ptrs.reserve(iterations);
        
        // Allocation phase
        for (size_t i = 0; i < iterations; ++i) {
            void* ptr = allocator.allocate(alloc_size);
            if (ptr) ptrs.push_back(ptr);
        }
        
        // Deallocation phase
        for (void* ptr : ptrs) {
            allocator.deallocate(ptr);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << name << " Allocator: " << duration.count() << " μs for " 
                  << iterations << " operations\n";
        std::cout << "  Successful allocations: " << ptrs.size() << "/" << iterations << "\n";
        std::cout << "  Average time per operation: " 
                  << (double)duration.count() / iterations << " μs\n\n";
    }
    
    static void benchmarkHybridAllocator(const std::string& name, size_t iterations, size_t alloc_size) {
        HybridAllocator allocator(16384);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<void*> ptrs;
        ptrs.reserve(iterations);
        
        // Allocation phase
        for (size_t i = 0; i < iterations; ++i) {
            void* ptr = allocator.allocate(alloc_size);
            if (ptr) ptrs.push_back(ptr);
        }
        
        // Deallocation phase
        for (void* ptr : ptrs) {
            allocator.deallocate(ptr);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << name << " Allocator: " << duration.count() << " μs for " 
                  << iterations << " operations\n";
        std::cout << "  Successful allocations: " << ptrs.size() << "/" << iterations << "\n";
        std::cout << "  Average time per operation: " 
                  << (double)duration.count() / iterations << " μs\n\n";
    }
};

int main() {
    try {
        TestRunner::runAllTests();
        PerformanceTest::runBenchmarks();
    }
    catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
