# Memory Allocator Design Document

## Table of Contents
1. [Project Overview](#project-overview)
2. [Architecture](#architecture)
3. [Allocator Implementations](#allocator-implementations)
4. [Design Decisions](#design-decisions)
5. [Performance Considerations](#performance-considerations)
6. [Testing Strategy](#testing-strategy)

## Project Overview

This project implements a comprehensive memory allocator system designed for an Operating Systems course. The system provides multiple allocation strategies optimized for different use cases, including a hybrid approach that adaptively selects the best allocator based on request characteristics.

### Goals
- **Speed**: Optimized allocation and deallocation operations
- **Fragmentation Control**: Minimize internal and external fragmentation
- **Scalability**: Efficient handling of varying allocation patterns
- **Educational Value**: Clear implementation demonstrating OS memory management concepts

### Features
- Four distinct allocator implementations
- Comprehensive performance analysis
- Real-time visualization interface
- Thread-safe operations with mutex protection
- Detailed statistics and memory layout tracking

## Architecture

### Class Hierarchy

```
MemoryAllocator (Abstract Base Class)
├── BuddyAllocator
├── SlabAllocator
├── PoolAllocator
└── HybridAllocator
```

### Base Class Design

The `MemoryAllocator` abstract base class provides:
- Common interface for all allocators
- Statistics tracking (allocations, deallocations, memory usage)
- Memory layout visualization support
- Virtual methods for polymorphic behavior

### Memory Layout Representation

```cpp
struct MemoryBlock {
    size_t offset;      // Position in memory
    size_t size;        // Block size
    bool is_free;       // Allocation status
    std::string info;   // Additional metadata
};
```

## Allocator Implementations

### 1. Buddy System Allocator

**Algorithm**: Binary buddy system with power-of-2 allocation sizes

**Data Structure**: Binary tree representation using arrays
- `tree_[i]` stores the size of the largest free block in subtree rooted at node i
- Left child: `2*i + 1`, Right child: `2*i + 2`

**Key Features**:
- Automatic coalescing of adjacent free blocks
- O(log n) allocation and deallocation
- Minimal external fragmentation
- Power-of-2 size alignment reduces internal fragmentation

**Implementation Details**:
```cpp
// Tree navigation
size_t left_child = 2 * node + 1;
size_t right_child = 2 * node + 2;
size_t parent = (node - 1) / 2;

// Size calculation
size_t node_size = memory_size_ / (1 << depth);
```

### 2. Slab Allocator

**Algorithm**: Object-oriented allocation with pre-allocated slabs

**Data Structure**: 
- Multiple slabs per object size
- Free lists within each slab
- Slab descriptors tracking usage

**Key Features**:
- Excellent cache locality for same-sized objects
- Minimal fragmentation for fixed-size allocations
- Fast allocation/deallocation via free lists
- Memory recycling reduces system calls

**Implementation Details**:
```cpp
struct Slab {
    void* memory;           // Slab memory region
    std::vector<bool> used; // Object usage bitmap
    size_t free_count;      // Available objects
    size_t next_free;       // Next free object index
};
```

### 3. Pool Allocator

**Algorithm**: Pre-allocated fixed-size block pools

**Data Structure**: Linked list of free blocks

**Key Features**:
- O(1) allocation and deallocation
- Predictable memory usage patterns
- No fragmentation within pools
- Excellent for fixed-size frequent allocations

**Implementation Details**:
```cpp
// Free list node structure
struct FreeBlock {
    FreeBlock* next;
};

// Pool structure
struct Pool {
    void* memory;
    FreeBlock* free_list;
    size_t block_size;
    size_t total_blocks;
    size_t free_blocks;
};
```

### 4. Hybrid Allocator

**Algorithm**: Adaptive allocation strategy selection

**Selection Criteria**:
- Small allocations (≤256 bytes): Pool Allocator
- Medium allocations (≤1024 bytes): Slab Allocator  
- Large allocations (>1024 bytes): Buddy Allocator

**Key Features**:
- Combines benefits of all allocator types
- Intelligent size-based selection
- Configurable thresholds
- Per-allocator statistics tracking

**Configuration**:
```cpp
struct HybridConfig {
    double pool_memory_ratio = 0.3;   // 30% for pools
    double slab_memory_ratio = 0.3;   // 30% for slabs
    size_t pool_max_size = 256;       // Pool threshold
    size_t slab_max_size = 1024;      // Slab threshold
};
```

## Design Decisions

### 1. Memory Layout Abstraction

**Decision**: Use `MemoryBlock` structure for visualization
**Rationale**: Provides consistent interface across all allocators while allowing implementation-specific optimizations

### 2. Thread Safety

**Decision**: Mutex protection for all allocators
**Rationale**: Enables safe multi-threaded usage without compromising single-threaded performance significantly

### 3. Statistics Tracking

**Decision**: Built-in performance monitoring
**Rationale**: Essential for educational purposes and performance analysis

### 4. Template-Free Design

**Decision**: Avoid templates for simplicity
**Rationale**: Focuses on algorithmic concepts rather than C++ template complexity

### 5. Error Handling

**Decision**: Return nullptr on allocation failure
**Rationale**: Follows standard C library conventions and allows graceful error handling

## Performance Considerations

### Time Complexity

| Allocator | Allocation | Deallocation | Memory Overhead |
|-----------|------------|--------------|-----------------|
| Buddy     | O(log n)   | O(log n)     | ~12% (tree)     |
| Slab      | O(1)       | O(1)         | ~8% (metadata)  |
| Pool      | O(1)       | O(1)         | ~4% (headers)   |
| Hybrid    | O(1)-O(log n) | O(1)-O(log n) | ~15% (combined) |

### Space Efficiency

**Buddy System**: 
- Internal fragmentation: Up to 50% for non-power-of-2 sizes
- External fragmentation: Minimal due to coalescing

**Slab Allocator**:
- Internal fragmentation: None for exact-size objects
- External fragmentation: Low within slabs

**Pool Allocator**:
- Internal fragmentation: None for matching sizes
- External fragmentation: None within pools

**Hybrid Allocator**:
- Balanced approach optimizing for common allocation patterns
- Configurable memory distribution

### Cache Performance

**Slab Allocator**: Excellent cache locality for object reuse
**Pool Allocator**: Good cache locality for similar-sized allocations
**Buddy System**: Moderate cache performance due to coalescing
**Hybrid**: Optimized cache usage through intelligent selection

## Testing Strategy

### 1. Unit Tests
- Basic allocation/deallocation functionality
- Boundary condition testing
- Error handling validation
- Memory corruption detection

### 2. Performance Tests
- Allocation speed benchmarks
- Memory efficiency measurements
- Fragmentation analysis
- Scalability testing

### 3. Stress Tests
- High-frequency allocation patterns
- Mixed allocation sizes
- Memory pressure scenarios
- Multi-threaded access patterns

### 4. Real-World Simulations
- Server workload patterns
- Gaming engine scenarios
- Database buffer management
- Operating system kernel simulation

### 5. Comparative Analysis
- Cross-allocator performance comparison
- Memory usage efficiency
- Fragmentation behavior analysis
- Thread contention measurement

## Future Enhancements

### Potential Improvements
1. **NUMA Awareness**: Optimize for Non-Uniform Memory Access
2. **Lock-Free Operations**: Reduce thread contention
3. **Adaptive Thresholds**: Dynamic threshold adjustment in hybrid allocator
4. **Memory Compaction**: Reduce fragmentation through compaction
5. **Hardware Prefetching**: Optimize cache line usage

### Advanced Features
1. **Memory Debugging**: Built-in leak detection and corruption checking
2. **Performance Profiling**: Real-time allocation pattern analysis
3. **Custom Allocators**: Plugin architecture for specialized allocators
4. **Memory Encryption**: Security-focused memory allocation
5. **Power Management**: Energy-efficient allocation strategies

This design document provides a comprehensive overview of the memory allocator project, focusing on educational value while maintaining practical performance characteristics suitable for real-world applications.
