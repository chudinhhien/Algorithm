#include "src/includes/memory_allocator.h"
#include <iostream>
#include <sstream>
#include <iomanip>

MemoryAllocator::MemoryAllocator(size_t total_memory) 
    : total_memory_(total_memory), allocated_size_(0), allocation_count_(0), deallocation_count_(0) {
}

MemoryAllocator::~MemoryAllocator() {
    std::cout << "Destroyed memory allocator\n";
    std::cout << "  Total allocations: " << allocation_count_ << "\n";
    std::cout << "  Total deallocations: " << deallocation_count_ << "\n";
    std::cout << "  Allocated size: " << allocated_size_ << " bytes\n";
}

std::string MemoryAllocator::getStats() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Memory Allocator Statistics:\n";
    oss << "  Total Memory: " << total_memory_ << " bytes\n";
    oss << "  Total Allocations: " << allocation_count_ << "\n";
    oss << "  Total Deallocations: " << deallocation_count_ << "\n";
    oss << "  Current Allocated: " << allocated_size_ << " bytes\n";
    oss << "  Utilization: " << (100.0 * allocated_size_ / total_memory_) << "%\n";
    oss << "  Current Fragmentation: " << getFragmentation() << " bytes\n";
    
    return oss.str();
}

bool MemoryAllocator::isValidPointer(void* ptr) const {
    // Basic pointer validation - can be enhanced by derived classes
    return ptr != nullptr;
} 
