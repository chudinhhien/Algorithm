#include "memory_allocator.h"
#include <iostream>
#include <sstream>
#include <iomanip>

MemoryAllocator::MemoryAllocator(size_t total_memory) 
    : total_memory_(total_memory) {
}

MemoryAllocator::~MemoryAllocator() {
    std::cout << "Destroyed memory allocator\n";
    std::cout << "  Total allocations: " << stats_.total_allocations << "\n";
    std::cout << "  Total deallocations: " << stats_.total_deallocations << "\n";
    std::cout << "  Peak allocated: " << stats_.peak_allocated << " bytes\n";
}

std::string MemoryAllocator::getStats() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "Memory Allocator Statistics:\n";
    oss << "  Total Memory: " << total_memory_ << " bytes\n";
    oss << "  Total Allocations: " << stats_.total_allocations << "\n";
    oss << "  Total Deallocations: " << stats_.total_deallocations << "\n";
    oss << "  Failed Allocations: " << stats_.failed_allocations << "\n";
    oss << "  Current Allocated: " << stats_.current_allocated << " bytes\n";
    oss << "  Peak Allocated: " << stats_.peak_allocated << " bytes\n";
    oss << "  Current Fragmentation: " << getFragmentation() << " bytes\n";
    
    if (stats_.total_allocations > 0) {
        oss << "  Average Allocation Time: " 
            << (stats_.total_allocation_time / stats_.total_allocations) << " ns\n";
    }
    
    if (stats_.total_deallocations > 0) {
        oss << "  Average Deallocation Time: " 
            << (stats_.total_deallocation_time / stats_.total_deallocations) << " ns\n";
    }
    
    return oss.str();
}
