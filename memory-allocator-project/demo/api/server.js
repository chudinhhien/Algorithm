const express = require('express');
const { spawn, exec } = require('child_process');
const path = require('path');
const cors = require('cors');

const app = express();
const PORT = 3000;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, '..')));

// Serve main page
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '..', 'index.html'));
});

// Path to the C++ executable
const EXECUTABLE_PATH = path.join(__dirname, '..', '..', 'bin', 'memory_allocator_demo.exe');

// Store active allocator instances
let allocatorInstances = new Map();

// API Routes

// Initialize allocator
app.post('/api/allocator/init', (req, res) => {
    const { type, size } = req.body;
    
    try {
        // For now, we'll simulate initialization
        // In a real implementation, you'd spawn the C++ process
        const instanceId = Date.now().toString();
        
        allocatorInstances.set(instanceId, {
            type: type || 'buddy',
            size: size || 1024,
            allocations: [],
            stats: {
                totalAllocated: 0,
                totalFree: size || 1024,
                fragmentationRatio: 0,
                allocCount: 0,
                deallocCount: 0,
                avgAllocTime: 0,
                peakUsage: 0
            }
        });
        
        res.json({
            success: true,
            instanceId: instanceId,
            message: `${type} allocator initialized with ${size} bytes`
        });
    } catch (error) {
        res.status(500).json({
            success: false,
            error: error.message
        });
    }
});

// Allocate memory
app.post('/api/allocator/:instanceId/allocate', (req, res) => {
    const { instanceId } = req.params;
    const { size } = req.body;
    
    const instance = allocatorInstances.get(instanceId);
    if (!instance) {
        return res.status(404).json({
            success: false,
            error: 'Allocator instance not found'
        });
    }
    
    try {
        // Simulate allocation
        const startTime = Date.now();
        
        if (instance.stats.totalFree >= size) {
            const allocationId = instance.allocations.length;
            const address = 0x1000 + instance.stats.totalAllocated;
            
            instance.allocations.push({
                id: allocationId,
                size: size,
                address: address,
                allocated: true
            });
            
            instance.stats.totalAllocated += size;
            instance.stats.totalFree -= size;
            instance.stats.allocCount++;
            instance.stats.peakUsage = Math.max(instance.stats.peakUsage, instance.stats.totalAllocated);
            
            const endTime = Date.now();
            const allocTime = endTime - startTime;
            instance.stats.avgAllocTime = ((instance.stats.avgAllocTime * (instance.stats.allocCount - 1)) + allocTime) / instance.stats.allocCount;
            
            // Calculate fragmentation (simplified)
            instance.stats.fragmentationRatio = instance.allocations.filter(a => !a.allocated).length / instance.allocations.length * 100;
              res.json({
                success: true,
                id: allocationId,
                address: `0x${address.toString(16)}`,
                size: size,
                stats: instance.stats,
                memoryLayout: instance.allocations
            });
        } else {
            res.json({
                success: false,
                error: 'Insufficient memory',
                stats: instance.stats
            });
        }
    } catch (error) {
        res.status(500).json({
            success: false,
            error: error.message
        });
    }
});

// Deallocate memory
app.post('/api/allocator/:instanceId/deallocate', (req, res) => {
    const { instanceId } = req.params;
    const { id } = req.body;
    
    const instance = allocatorInstances.get(instanceId);
    if (!instance) {
        return res.status(404).json({
            success: false,
            error: 'Allocator instance not found'
        });
    }
    
    try {
        const allocation = instance.allocations[id];
        if (!allocation || !allocation.allocated) {
            return res.json({
                success: false,
                error: 'Invalid allocation ID or already deallocated'
            });
        }
        
        allocation.allocated = false;
        instance.stats.totalAllocated -= allocation.size;
        instance.stats.totalFree += allocation.size;
        instance.stats.deallocCount++;
        
        // Recalculate fragmentation
        instance.stats.fragmentationRatio = instance.allocations.filter(a => !a.allocated).length / instance.allocations.length * 100;
        
        res.json({
            success: true,
            message: `Deallocated ${allocation.size} bytes`,
            stats: instance.stats,
            memoryLayout: instance.allocations
        });
    } catch (error) {
        res.status(500).json({
            success: false,
            error: error.message
        });
    }
});

// Get allocator stats
app.get('/api/allocator/:instanceId/stats', (req, res) => {
    const { instanceId } = req.params;
    
    const instance = allocatorInstances.get(instanceId);
    if (!instance) {
        return res.status(404).json({
            success: false,
            error: 'Allocator instance not found'
        });
    }
      res.json({
        success: true,
        data: {
            fragmentation: instance.stats.fragmentationRatio / 100,
            totalAllocated: instance.stats.totalAllocated,
            totalFree: instance.stats.totalFree,
            allocCount: instance.stats.allocCount,
            avgAllocTime: instance.stats.avgAllocTime,
            peakUsage: instance.stats.peakUsage
        },
        type: instance.type,
        totalSize: instance.size
    });
});

// Reset allocator
app.post('/api/allocator/:instanceId/reset', (req, res) => {
    const { instanceId } = req.params;
    
    const instance = allocatorInstances.get(instanceId);
    if (!instance) {
        return res.status(404).json({
            success: false,
            error: 'Allocator instance not found'
        });
    }
    
    instance.allocations = [];
    instance.stats = {
        totalAllocated: 0,
        totalFree: instance.size,
        fragmentationRatio: 0,
        allocCount: 0,
        deallocCount: 0,
        avgAllocTime: 0,
        peakUsage: 0
    };
    
    res.json({
        success: true,
        message: 'Allocator reset successfully',
        stats: instance.stats
    });
});

// Run benchmark
app.post('/api/allocator/:instanceId/benchmark', (req, res) => {
    const { instanceId } = req.params;
    const { iterations, allocSize } = req.body;
    
    const instance = allocatorInstances.get(instanceId);
    if (!instance) {
        return res.status(404).json({
            success: false,
            error: 'Allocator instance not found'
        });
    }
    
    try {
        // Simulate benchmark
        const startTime = Date.now();
        let successfulAllocs = 0;
        
        for (let i = 0; i < iterations && instance.stats.totalFree >= allocSize; i++) {
            // Simulate allocation
            const allocationId = instance.allocations.length;
            const address = 0x1000 + instance.stats.totalAllocated;
            
            instance.allocations.push({
                id: allocationId,
                size: allocSize,
                address: address,
                allocated: true
            });
            
            instance.stats.totalAllocated += allocSize;
            instance.stats.totalFree -= allocSize;
            instance.stats.allocCount++;
            successfulAllocs++;
        }
        
        const endTime = Date.now();
        const totalTime = endTime - startTime;
        
        instance.stats.peakUsage = Math.max(instance.stats.peakUsage, instance.stats.totalAllocated);
        instance.stats.avgAllocTime = totalTime / successfulAllocs;
          res.json({
            success: true,
            data: {
                type: req.body.type || 'allocation',
                iterations: iterations,
                totalTime: totalTime,
                avgTime: totalTime / successfulAllocs,
                minTime: 0.1, // Simulated
                maxTime: totalTime / 10, // Simulated
                successRate: (successfulAllocs / iterations) * 100,
                fragmentation: instance.stats.fragmentationRatio / 100,
                successful: successfulAllocs,
                throughput: successfulAllocs / (totalTime / 1000)
            }
        });
    } catch (error) {
        res.status(500).json({
            success: false,
            error: error.message
        });
    }
});

// Run real C++ executable for comparison
app.post('/api/run-cpp-demo', (req, res) => {
    const { command } = req.body;
    
    // Run the actual C++ executable
    exec(`echo "${command}" | "${EXECUTABLE_PATH}"`, {
        cwd: path.dirname(EXECUTABLE_PATH),
        timeout: 10000
    }, (error, stdout, stderr) => {
        if (error) {
            return res.status(500).json({
                success: false,
                error: error.message,
                stderr: stderr
            });
        }
        
        res.json({
            success: true,
            output: stdout,
            stderr: stderr
        });
    });
});

// Defragment memory
app.post('/api/allocator/:instanceId/defragment', (req, res) => {
    const { instanceId } = req.params;
    
    const instance = allocatorInstances.get(instanceId);
    if (!instance) {
        return res.status(404).json({
            success: false,
            error: 'Allocator instance not found'
        });
    }
    
    try {
        // Simulate defragmentation by compacting allocations
        const allocatedBlocks = instance.allocations.filter(a => a.allocated);
        let currentAddress = 0x1000;
        
        allocatedBlocks.forEach(block => {
            block.address = currentAddress;
            currentAddress += block.size;
        });
        
        // Update fragmentation
        instance.stats.fragmentationRatio = instance.allocations.filter(a => !a.allocated).length / instance.allocations.length * 100;
        
        res.json({
            success: true,
            message: 'Defragmentation completed',
            stats: instance.stats,
            memoryLayout: instance.allocations
        });
    } catch (error) {
        res.status(500).json({
            success: false,
            error: error.message
        });
    }
});

// Get memory layout
app.get('/api/allocator/:instanceId/layout', (req, res) => {
    const { instanceId } = req.params;
    
    const instance = allocatorInstances.get(instanceId);
    if (!instance) {
        return res.status(404).json({
            success: false,
            error: 'Allocator instance not found'
        });
    }
      try {
        // Generate memory layout for visualization
        const blocks = [];
        let currentPos = 0;
        
        // Sort allocations by address
        const sortedAllocs = instance.allocations
            .filter(a => a.allocated)
            .sort((a, b) => a.address - b.address);
        
        // If no allocations, just show one big free block
        if (sortedAllocs.length === 0) {
            blocks.push({
                size: instance.size,
                type: 'free'
            });
        } else {
            // Add allocated blocks and gaps
            sortedAllocs.forEach(alloc => {
                // Add gap if needed
                if (currentPos < alloc.address - 0x1000) {
                    blocks.push({
                        size: (alloc.address - 0x1000) - currentPos,
                        type: 'free'
                    });
                }
                
                // Add allocated block
                blocks.push({
                    size: alloc.size,
                    type: 'allocated'
                });
                
                currentPos = (alloc.address - 0x1000) + alloc.size;
            });
            
            // Add remaining free space
            if (currentPos < instance.size) {
                blocks.push({
                    size: instance.size - currentPos,
                    type: 'free'
                });
            }
        }
        
        res.json({
            success: true,
            data: {
                blocks: blocks,
                totalSize: instance.size,
                allocatedSize: instance.stats.totalAllocated,
                freeSize: instance.stats.totalFree
            }
        });
    } catch (error) {
        res.status(500).json({
            success: false,
            error: error.message
        });
    }
});

// Start server
app.listen(PORT, () => {
    console.log(`Memory Allocator Demo Server running at http://localhost:${PORT}`);
    console.log(`C++ Executable path: ${EXECUTABLE_PATH}`);
});

module.exports = app;
