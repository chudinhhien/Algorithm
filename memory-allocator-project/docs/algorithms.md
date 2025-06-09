# Memory Allocation Algorithms

## Table of Contents
1. [Buddy System Algorithm](#buddy-system-algorithm)
2. [Slab Allocation Algorithm](#slab-allocation-algorithm)
3. [Pool Allocation Algorithm](#pool-allocation-algorithm)
4. [Hybrid Allocation Strategy](#hybrid-allocation-strategy)
5. [Algorithm Comparison](#algorithm-comparison)

## Buddy System Algorithm

### Overview
The buddy system is a memory allocation algorithm that divides memory into blocks of sizes that are powers of 2. It maintains a binary tree structure to track free and allocated blocks, enabling efficient coalescing of adjacent free blocks.

### Core Concepts

#### Binary Tree Representation
```
Memory Size: 1024 bytes (2^10)
Tree Structure:
         [1024]
        /      \
    [512]      [512]
   /    \      /    \
[256] [256] [256] [256]
```

#### Node Indexing
- Root node: index 0
- Left child of node i: 2*i + 1
- Right child of node i: 2*i + 2
- Parent of node i: (i-1)/2

### Algorithm Details

#### Allocation Process
```cpp
void* allocate(size_t size) {
    // 1. Round up to next power of 2
    size_t actual_size = nextPowerOf2(size);
    
    // 2. Find suitable block in tree
    size_t node = 0;
    size_t node_size = memory_size_;
    
    // 3. Traverse down to find free block
    while (node_size > actual_size) {
        if (tree_[node] < actual_size) {
            return nullptr; // No suitable block
        }
        
        // Choose left or right child
        size_t left = 2 * node + 1;
        size_t right = 2 * node + 2;
        
        if (tree_[left] >= actual_size) {
            node = left;
        } else {
            node = right;
        }
        
        node_size /= 2;
    }
    
    // 4. Mark block as allocated
    tree_[node] = 0;
    
    // 5. Update parent nodes
    updateParents(node);
    
    return memory_ + (node - (tree_size_ - memory_size_ / min_block_size_)) * node_size;
}
```

#### Deallocation Process
```cpp
void deallocate(void* ptr) {
    // 1. Calculate node index from pointer
    size_t offset = (char*)ptr - (char*)memory_;
    size_t node = findNode(offset);
    size_t node_size = calculateNodeSize(node);
    
    // 2. Mark block as free
    tree_[node] = node_size;
    
    // 3. Coalesce with buddy if possible
    while (node > 0) {
        size_t parent = (node - 1) / 2;
        size_t left = 2 * parent + 1;
        size_t right = 2 * parent + 2;
        
        // Check if both children are free
        if (tree_[left] == node_size && tree_[right] == node_size) {
            // Coalesce
            tree_[parent] = node_size * 2;
            tree_[left] = 0;
            tree_[right] = 0;
            
            node = parent;
            node_size *= 2;
        } else {
            // Update parent's maximum free block size
            tree_[parent] = std::max(tree_[left], tree_[right]);
            break;
        }
    }
}
```

#### Complexity Analysis
- **Time Complexity**: O(log n) for both allocation and deallocation
- **Space Complexity**: O(n) for the tree structure
- **Internal Fragmentation**: Up to 50% for non-power-of-2 sizes
- **External Fragmentation**: Minimal due to automatic coalescing

### Advantages
1. **Automatic Coalescing**: Eliminates external fragmentation
2. **Fast Operations**: Logarithmic time complexity
3. **Simple Implementation**: Clear tree-based structure
4. **Predictable Behavior**: Deterministic allocation patterns

### Disadvantages
1. **Internal Fragmentation**: Significant for non-power-of-2 sizes
2. **Memory Overhead**: Tree structure requires additional space
3. **Size Restrictions**: Only power-of-2 allocation sizes
4. **Cache Performance**: May have poor locality for small allocations

## Slab Allocation Algorithm

### Overview
Slab allocation is designed for efficient allocation of objects of the same size. It pre-allocates large chunks of memory (slabs) and divides them into equally-sized objects, maintaining free lists for fast allocation.

### Core Concepts

#### Slab Structure
```cpp
struct Slab {
    void* memory;           // Pointer to slab memory
    std::vector<bool> used; // Bitmap of used objects
    size_t free_count;      // Number of free objects
    size_t next_free;       // Index of next free object
};
```

#### Object Layout
```
Slab Memory Layout:
[Object 0][Object 1][Object 2]...[Object N-1]
     |         |         |            |
    used      free      used         free
```

### Algorithm Details

#### Slab Creation
```cpp
void createSlab(size_t object_size) {
    size_t objects_per_slab = slab_size_ / object_size;
    
    Slab slab;
    slab.memory = std::aligned_alloc(sizeof(void*), slab_size_);
    slab.used.resize(objects_per_slab, false);
    slab.free_count = objects_per_slab;
    slab.next_free = 0;
    
    slabs_[object_size].push_back(std::move(slab));
}
```

#### Allocation Process
```cpp
void* allocate(size_t size) {
    // 1. Find or create slab for this size
    auto& slab_list = slabs_[size];
    Slab* available_slab = nullptr;
    
    // 2. Find slab with free objects
    for (auto& slab : slab_list) {
        if (slab.free_count > 0) {
            available_slab = &slab;
            break;
        }
    }
    
    // 3. Create new slab if needed
    if (!available_slab) {
        createSlab(size);
        available_slab = &slab_list.back();
    }
    
    // 4. Find free object in slab
    size_t index = available_slab->next_free;
    while (index < available_slab->used.size() && 
           available_slab->used[index]) {
        index++;
    }
    
    // 5. Mark object as used
    available_slab->used[index] = true;
    available_slab->free_count--;
    
    // 6. Update next_free pointer
    available_slab->next_free = index + 1;
    
    return (char*)available_slab->memory + index * size;
}
```

#### Deallocation Process
```cpp
void deallocate(void* ptr) {
    // 1. Find which slab contains this pointer
    for (auto& [size, slab_list] : slabs_) {
        for (auto& slab : slab_list) {
            char* slab_start = (char*)slab.memory;
            char* slab_end = slab_start + slab_size_;
            
            if (ptr >= slab_start && ptr < slab_end) {
                // 2. Calculate object index
                size_t index = ((char*)ptr - slab_start) / size;
                
                // 3. Mark object as free
                slab.used[index] = false;
                slab.free_count++;
                
                // 4. Update next_free if this is earlier
                if (index < slab.next_free) {
                    slab.next_free = index;
                }
                
                return;
            }
        }
    }
}
```

#### Complexity Analysis
- **Time Complexity**: O(1) average case, O(n) worst case for allocation
- **Space Complexity**: O(1) per object plus slab overhead
- **Internal Fragmentation**: None for exact-size objects
- **External Fragmentation**: Low within slabs

### Advantages
1. **Cache Efficiency**: Excellent locality for same-sized objects
2. **Fast Allocation**: O(1) average time for allocation/deallocation
3. **Memory Recycling**: Reuses freed objects efficiently
4. **No Fragmentation**: Within slabs for fixed-size objects

### Disadvantages
1. **Memory Overhead**: Metadata for each slab
2. **Size Restrictions**: Optimized only for specific object sizes
3. **Slab Waste**: Unused space in partially filled slabs
4. **Complex Management**: Multiple slabs per object size

## Pool Allocation Algorithm

### Overview
Pool allocation pre-allocates fixed-size blocks and maintains them in a free list. It's optimized for scenarios with frequent allocation and deallocation of same-sized objects.

### Core Concepts

#### Free List Structure
```cpp
struct FreeBlock {
    FreeBlock* next;
};

struct Pool {
    void* memory;
    FreeBlock* free_list;
    size_t block_size;
    size_t total_blocks;
    size_t free_blocks;
};
```

#### Memory Layout
```
Pool Memory Layout:
[Block 0][Block 1][Block 2]...[Block N-1]
    |        |        |           |
   used    free     used        free
           |                     |
           v                     v
        free_list ----------> next_free
```

### Algorithm Details

#### Pool Initialization
```cpp
void initializePool(size_t block_size, size_t num_blocks) {
    Pool pool;
    pool.block_size = block_size;
    pool.total_blocks = num_blocks;
    pool.free_blocks = num_blocks;
    
    // Allocate memory for all blocks
    pool.memory = std::aligned_alloc(sizeof(void*), 
                                    block_size * num_blocks);
    
    // Initialize free list
    pool.free_list = nullptr;
    char* current = (char*)pool.memory;
    
    for (size_t i = 0; i < num_blocks; i++) {
        FreeBlock* block = (FreeBlock*)current;
        block->next = pool.free_list;
        pool.free_list = block;
        current += block_size;
    }
    
    pools_.push_back(std::move(pool));
}
```

#### Allocation Process
```cpp
void* allocate(size_t size) {
    // 1. Find pool with matching block size
    Pool* target_pool = nullptr;
    for (auto& pool : pools_) {
        if (pool.block_size >= size) {
            target_pool = &pool;
            break;
        }
    }
    
    // 2. Check if pool has free blocks
    if (!target_pool || !target_pool->free_list) {
        return nullptr;
    }
    
    // 3. Remove block from free list
    FreeBlock* allocated_block = target_pool->free_list;
    target_pool->free_list = allocated_block->next;
    target_pool->free_blocks--;
    
    return allocated_block;
}
```

#### Deallocation Process
```cpp
void deallocate(void* ptr) {
    // 1. Find which pool contains this pointer
    for (auto& pool : pools_) {
        char* pool_start = (char*)pool.memory;
        char* pool_end = pool_start + 
                        pool.block_size * pool.total_blocks;
        
        if (ptr >= pool_start && ptr < pool_end) {
            // 2. Add block back to free list
            FreeBlock* block = (FreeBlock*)ptr;
            block->next = pool.free_list;
            pool.free_list = block;
            pool.free_blocks++;
            
            return;
        }
    }
}
```

#### Complexity Analysis
- **Time Complexity**: O(1) for allocation and deallocation
- **Space Complexity**: O(1) per block plus pool overhead
- **Internal Fragmentation**: Depends on size matching
- **External Fragmentation**: None within pools

### Advantages
1. **Constant Time**: O(1) allocation and deallocation
2. **Predictable Performance**: No search or coalescing required
3. **Simple Implementation**: Straightforward free list management
4. **Memory Locality**: Blocks allocated from same pool are nearby

### Disadvantages
1. **Fixed Sizes**: Not suitable for variable-size allocations
2. **Memory Waste**: Unused pools waste space
3. **Pre-allocation**: Requires knowing allocation patterns
4. **Limited Flexibility**: Cannot adapt to changing requirements

## Hybrid Allocation Strategy

### Overview
The hybrid allocator combines multiple allocation algorithms to optimize for different allocation patterns. It intelligently selects the best allocator based on allocation size and frequency patterns.

### Selection Strategy

#### Size-Based Selection
```cpp
AllocatorType selectAllocator(size_t size) {
    if (size <= config_.pool_max_size) {
        return AllocatorType::POOL;
    } else if (size <= config_.slab_max_size) {
        return AllocatorType::SLAB;
    } else {
        return AllocatorType::BUDDY;
    }
}
```

#### Allocation Routing
```cpp
void* allocate(size_t size) {
    AllocatorType type = selectAllocator(size);
    void* ptr = nullptr;
    
    switch (type) {
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
        allocation_map_[ptr] = type;
        updateStatistics(type, size, true);
    }
    
    return ptr;
}
```

#### Memory Distribution
```cpp
struct HybridConfig {
    double pool_memory_ratio = 0.3;   // 30% for pools
    double slab_memory_ratio = 0.3;   // 30% for slabs  
    size_t pool_max_size = 256;       // Pool threshold
    size_t slab_max_size = 1024;      // Slab threshold
};

// Memory allocation:
// - Pool allocators: 30% of total memory
// - Slab allocators: 30% of total memory  
// - Buddy allocator: 40% of total memory
```

### Adaptive Features

#### Performance Monitoring
```cpp
struct AllocatorStats {
    size_t allocations = 0;
    size_t deallocations = 0;
    size_t total_allocated = 0;
    double average_allocation_time = 0.0;
    double fragmentation_ratio = 0.0;
};
```

#### Efficiency Scoring
```cpp
double getEfficiencyScore() const {
    double pool_efficiency = calculateEfficiency(pool_stats_);
    double slab_efficiency = calculateEfficiency(slab_stats_);
    double buddy_efficiency = calculateEfficiency(buddy_stats_);
    
    return (pool_efficiency * config_.pool_memory_ratio +
            slab_efficiency * config_.slab_memory_ratio +
            buddy_efficiency * (1.0 - config_.pool_memory_ratio - 
                               config_.slab_memory_ratio));
}
```

### Advantages
1. **Optimal Performance**: Best algorithm for each allocation size
2. **Balanced Trade-offs**: Combines strengths of different approaches
3. **Configurable**: Adjustable thresholds and memory distribution
4. **Comprehensive**: Handles all allocation patterns effectively

### Disadvantages
1. **Complexity**: More complex implementation and debugging
2. **Memory Overhead**: Combined overhead of all allocators
3. **Selection Overhead**: Additional logic for allocator selection
4. **Configuration Dependency**: Performance depends on proper tuning

## Algorithm Comparison

### Performance Matrix

| Algorithm | Allocation | Deallocation | Memory Overhead | Fragmentation |
|-----------|------------|--------------|-----------------|---------------|
| Buddy     | O(log n)   | O(log n)     | ~12%           | Internal: High, External: Low |
| Slab      | O(1) avg   | O(1)         | ~8%            | Internal: None, External: Low |
| Pool      | O(1)       | O(1)         | ~4%            | Internal: Variable, External: None |
| Hybrid    | O(1)-O(log n) | O(1)-O(log n) | ~15%        | Balanced |

### Use Case Recommendations

#### Buddy System
- **Best for**: Variable-size allocations, general-purpose allocation
- **Avoid for**: Small, frequent allocations

#### Slab Allocator  
- **Best for**: Fixed-size objects, object caching, kernel data structures
- **Avoid for**: Highly variable allocation sizes

#### Pool Allocator
- **Best for**: Real-time systems, game engines, high-frequency trading
- **Avoid for**: Unknown allocation patterns

#### Hybrid Allocator
- **Best for**: General applications, mixed allocation patterns
- **Avoid for**: Highly specialized or constrained environments

### Memory Efficiency Comparison

```
Small Allocations (≤256 bytes):
Pool > Slab > Hybrid > Buddy

Medium Allocations (257-1024 bytes):
Slab > Pool > Hybrid > Buddy

Large Allocations (>1024 bytes):
Buddy > Hybrid > Slab > Pool

Mixed Workloads:
Hybrid > Buddy > Slab > Pool
```

### Conclusion

Each algorithm excels in specific scenarios:
- **Buddy System**: Provides balanced performance for general-purpose allocation
- **Slab Allocator**: Optimizes cache performance for object allocation
- **Pool Allocator**: Delivers predictable real-time performance
- **Hybrid Allocator**: Adapts to diverse workloads automatically

The choice depends on specific application requirements, allocation patterns, and performance constraints.
