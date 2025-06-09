#include "../includes/slab_allocator.h"
#include <cstring>
#include <algorithm>

SlabAllocator::SlabAllocator(size_t object_size, size_t objects_per_slab, size_t total_memory) 
    : MemoryAllocator(total_memory), object_size_(object_size), objects_per_slab_(objects_per_slab) {
    // Calculate slab size (object size * objects per slab + metadata)
    slab_size_ = object_size * objects_per_slab + sizeof(SlabHeader);
    
    // Calculate how many slabs we can fit in total memory
    max_slabs_ = total_memory / slab_size_;
    if (max_slabs_ == 0) max_slabs_ = 1;
    
    // Initialize memory
    memory_pool_ = new char[total_memory];
    std::memset(memory_pool_, 0, total_memory);
    
    // Create initial slab
    createSlab();
}

SlabAllocator::~SlabAllocator() {
    delete[] memory_pool_;
}

void* SlabAllocator::allocate(size_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (size > object_size_) {
        return nullptr; // Size too large for this slab allocator
    }
    
    // Try to allocate from existing slabs
    for (auto& slab : slabs_) {
        if (slab.free_objects > 0) {
            void* ptr = allocateFromSlab(slab);
            if (ptr) {
                allocated_size_ += object_size_;
                allocation_count_++;
                return ptr;
            }
        }
    }
    
    // No free objects in existing slabs, create new slab
    if (slabs_.size() < max_slabs_) {
        createSlab();
        if (!slabs_.empty()) {
            void* ptr = allocateFromSlab(slabs_.back());
            if (ptr) {
                allocated_size_ += object_size_;
                allocation_count_++;
                return ptr;
            }
        }
    }
    
    return nullptr; // Out of memory
}

void SlabAllocator::deallocate(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Find which slab this pointer belongs to
    for (auto& slab : slabs_) {
        char* slab_start = memory_pool_ + slab.offset + sizeof(SlabHeader);
        char* slab_end = slab_start + (object_size_ * objects_per_slab_);
        
        if (ptr >= slab_start && ptr < slab_end) {
            deallocateFromSlab(slab, ptr);
            allocated_size_ -= object_size_;
            deallocation_count_++;
            return;
        }
    }
}

size_t SlabAllocator::getFragmentation() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (slabs_.empty()) return 0;
    
    size_t total_free_objects = 0;
    size_t total_objects = slabs_.size() * objects_per_slab_;
    
    for (const auto& slab : slabs_) {
        total_free_objects += slab.free_objects;
    }
    
    // Calculate fragmentation as percentage of free objects in allocated slabs
    size_t allocated_objects = total_objects - total_free_objects;
    if (total_objects == 0) return 0;
    
    return (total_free_objects * 100) / total_objects;
}

std::string SlabAllocator::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string stats = MemoryAllocator::getStats();
    stats += "Slab Allocator Stats:\n";
    stats += "  Object Size: " + std::to_string(object_size_) + " bytes\n";
    stats += "  Objects per Slab: " + std::to_string(objects_per_slab_) + "\n";
    stats += "  Total Slabs: " + std::to_string(slabs_.size()) + "\n";
    stats += "  Max Slabs: " + std::to_string(max_slabs_) + "\n";
    stats += "  Slab Size: " + std::to_string(slab_size_) + " bytes\n";
    
    size_t total_free_objects = 0;
    for (const auto& slab : slabs_) {
        total_free_objects += slab.free_objects;
    }
    stats += "  Free Objects: " + std::to_string(total_free_objects) + "\n";
    
    return stats;
}

void SlabAllocator::createSlab() {
    if (slabs_.size() >= max_slabs_) return;
    
    SlabInfo slab;
    slab.offset = slabs_.size() * slab_size_;
    slab.free_objects = objects_per_slab_;
    
    // Initialize slab header
    SlabHeader* header = reinterpret_cast<SlabHeader*>(memory_pool_ + slab.offset);
    header->free_count = objects_per_slab_;
    header->first_free = 0;
    
    // Initialize free list - each object contains index to next free object
    char* objects_start = memory_pool_ + slab.offset + sizeof(SlabHeader);
    for (size_t i = 0; i < objects_per_slab_ - 1; ++i) {
        size_t* next_ptr = reinterpret_cast<size_t*>(objects_start + i * object_size_);
        *next_ptr = i + 1;
    }
    
    // Last object points to invalid index
    size_t* last_ptr = reinterpret_cast<size_t*>(objects_start + (objects_per_slab_ - 1) * object_size_);
    *last_ptr = static_cast<size_t>(-1);
    
    slabs_.push_back(slab);
}

void* SlabAllocator::allocateFromSlab(SlabInfo& slab) {
    if (slab.free_objects == 0) return nullptr;
    
    SlabHeader* header = reinterpret_cast<SlabHeader*>(memory_pool_ + slab.offset);
    if (header->free_count == 0) return nullptr;
    
    // Get first free object
    size_t free_index = header->first_free;
    char* objects_start = memory_pool_ + slab.offset + sizeof(SlabHeader);
    void* ptr = objects_start + free_index * object_size_;
    
    // Update free list
    size_t* next_ptr = reinterpret_cast<size_t*>(ptr);
    header->first_free = *next_ptr;
    header->free_count--;
    slab.free_objects--;
    
    // Clear the allocated memory
    std::memset(ptr, 0, object_size_);
    
    return ptr;
}

void SlabAllocator::deallocateFromSlab(SlabInfo& slab, void* ptr) {
    SlabHeader* header = reinterpret_cast<SlabHeader*>(memory_pool_ + slab.offset);
    char* objects_start = memory_pool_ + slab.offset + sizeof(SlabHeader);
    
    // Calculate object index
    size_t index = (static_cast<char*>(ptr) - objects_start) / object_size_;
    
    // Add to free list
    size_t* next_ptr = reinterpret_cast<size_t*>(ptr);
    *next_ptr = header->first_free;
    header->first_free = index;
    header->free_count++;
    slab.free_objects++;
}

std::vector<MemoryAllocator::MemoryBlock> SlabAllocator::getMemoryLayout() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<MemoryAllocator::MemoryBlock> layout;
    
    for (size_t i = 0; i < slabs_.size(); ++i) {
        const auto& slab = slabs_[i];
        
        // Add slab header
        MemoryBlock header_block;
        header_block.address = slab.offset;
        header_block.size = sizeof(SlabHeader);
        header_block.is_free = false;
        header_block.type = "Slab Header";
        layout.push_back(header_block);
        
        // Add objects
        char* objects_start = memory_pool_ + slab.offset + sizeof(SlabHeader);
        SlabHeader* header = reinterpret_cast<SlabHeader*>(memory_pool_ + slab.offset);
        
        // Build free object set for quick lookup
        std::set<size_t> free_indices;
        size_t current_free = header->first_free;
        while (current_free != static_cast<size_t>(-1) && free_indices.size() < header->free_count) {
            free_indices.insert(current_free);
            size_t* next_ptr = reinterpret_cast<size_t*>(objects_start + current_free * object_size_);
            current_free = *next_ptr;
        }
        
        for (size_t j = 0; j < objects_per_slab_; ++j) {
            MemoryBlock object_block;
            object_block.address = slab.offset + sizeof(SlabHeader) + j * object_size_;
            object_block.size = object_size_;
            object_block.is_free = free_indices.count(j) > 0;
            object_block.type = object_block.is_free ? "Free Object" : "Allocated Object";
            layout.push_back(object_block);
        }
    }
    
    return layout;
}
