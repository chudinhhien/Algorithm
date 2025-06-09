// Memory Allocator Benchmark Module

class MemoryAllocatorBenchmark {
    constructor() {
        this.algorithms = ['buddy', 'slab', 'pool', 'hybrid'];
        this.testSizes = [16, 32, 64, 128, 256, 512, 1024, 2048];
        this.testCounts = [100, 500, 1000, 2000, 5000];
        this.results = new Map();
    }

    // Benchmark một thuật toán cụ thể
    async benchmarkAlgorithm(algorithm, memorySize = 8192, testCount = 1000) {
        console.log(`🔄 Benchmarking ${algorithm} allocator...`);
        
        const metrics = {
            algorithm: algorithm,
            memorySize: memorySize,
            testCount: testCount,
            allocations: [],
            avgAllocTime: 0,
            avgDeallocTime: 0,
            fragmentationRatio: 0,
            peakUsage: 0,
            successRate: 0,
            throughput: 0,
            timeline: []
        };

        // Khởi tạo allocator
        const startTime = performance.now();
        let allocatorId = null;
        
        try {
            const response = await fetch('http://localhost:3000/api/allocator/init', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ type: algorithm, size: memorySize })
            });
            
            if (!response.ok) throw new Error('Failed to initialize allocator');
            const data = await response.json();
            allocatorId = data.allocatorId;
            
        } catch (error) {
            console.error('Benchmark failed:', error);
            return null;
        }

        // Test allocation/deallocation
        let successfulAllocations = 0;
        let totalAllocTime = 0;
        let totalDeallocTime = 0;
        const allocatedBlocks = [];

        for (let i = 0; i < testCount; i++) {
            // Random allocation size
            const size = this.testSizes[Math.floor(Math.random() * this.testSizes.length)];
            
            // Allocation
            const allocStart = performance.now();
            try {
                const allocResponse = await fetch(`http://localhost:3000/api/allocator/${allocatorId}/allocate`, {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ size: size })
                });
                
                const allocEnd = performance.now();
                const allocTime = allocEnd - allocStart;
                totalAllocTime += allocTime;
                
                if (allocResponse.ok) {
                    const allocData = await allocResponse.json();
                    successfulAllocations++;
                    allocatedBlocks.push({
                        blockId: allocData.blockId,
                        size: size,
                        allocTime: allocTime
                    });
                    
                    metrics.timeline.push({
                        time: i,
                        operation: 'alloc',
                        size: size,
                        duration: allocTime,
                        success: true
                    });
                } else {
                    metrics.timeline.push({
                        time: i,
                        operation: 'alloc',
                        size: size,
                        duration: allocTime,
                        success: false
                    });
                }
            } catch (error) {
                metrics.timeline.push({
                    time: i,
                    operation: 'alloc',
                    size: size,
                    duration: 0,
                    success: false
                });
            }

            // Deallocation (50% chance)
            if (allocatedBlocks.length > 0 && Math.random() < 0.5) {
                const randomIndex = Math.floor(Math.random() * allocatedBlocks.length);
                const blockToFree = allocatedBlocks[randomIndex];
                
                const deallocStart = performance.now();
                try {
                    const deallocResponse = await fetch(`http://localhost:3000/api/allocator/${allocatorId}/deallocate`, {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ blockId: blockToFree.blockId })
                    });
                    
                    const deallocEnd = performance.now();
                    const deallocTime = deallocEnd - deallocStart;
                    totalDeallocTime += deallocTime;
                    
                    if (deallocResponse.ok) {
                        allocatedBlocks.splice(randomIndex, 1);
                        metrics.timeline.push({
                            time: i,
                            operation: 'dealloc',
                            size: blockToFree.size,
                            duration: deallocTime,
                            success: true
                        });
                    }
                } catch (error) {
                    metrics.timeline.push({
                        time: i,
                        operation: 'dealloc',
                        size: blockToFree.size,
                        duration: 0,
                        success: false
                    });
                }
            }
        }

        // Lấy thống kê cuối cùng
        try {
            const statsResponse = await fetch(`http://localhost:3000/api/allocator/${allocatorId}/stats`);
            if (statsResponse.ok) {
                const stats = await statsResponse.json();
                metrics.fragmentationRatio = stats.fragmentationRatio;
                metrics.peakUsage = stats.peakUsage;
            }
        } catch (error) {
            console.error('Failed to get final stats:', error);
        }

        const endTime = performance.now();
        const totalTime = endTime - startTime;

        // Tính toán metrics
        metrics.avgAllocTime = totalAllocTime / testCount;
        metrics.avgDeallocTime = totalDeallocTime / (testCount * 0.5); // Approximate
        metrics.successRate = (successfulAllocations / testCount) * 100;
        metrics.throughput = (successfulAllocations / totalTime) * 1000; // ops/second

        console.log(`✅ ${algorithm} benchmark completed:`, {
            'Success Rate': `${metrics.successRate.toFixed(1)}%`,
            'Avg Alloc Time': `${metrics.avgAllocTime.toFixed(2)}ms`,
            'Throughput': `${metrics.throughput.toFixed(0)} ops/sec`,
            'Fragmentation': `${(metrics.fragmentationRatio * 100).toFixed(1)}%`
        });

        this.results.set(algorithm, metrics);
        return metrics;
    }

    // Benchmark tất cả thuật toán
    async benchmarkAll(memorySize = 8192, testCount = 1000) {
        const results = [];
        
        for (const algorithm of this.algorithms) {
            const result = await this.benchmarkAlgorithm(algorithm, memorySize, testCount);
            if (result) {
                results.push(result);
            }
            // Delay nhỏ giữa các test
            await new Promise(resolve => setTimeout(resolve, 500));
        }
        
        return results;
    }

    // So sánh hiệu suất
    compareAlgorithms() {
        if (this.results.size === 0) {
            console.log('No benchmark results available');
            return null;
        }

        const comparison = {
            fastest: { algorithm: '', avgAllocTime: Infinity },
            mostReliable: { algorithm: '', successRate: 0 },
            leastFragmented: { algorithm: '', fragmentationRatio: Infinity },
            highestThroughput: { algorithm: '', throughput: 0 }
        };

        for (const [algorithm, metrics] of this.results) {
            if (metrics.avgAllocTime < comparison.fastest.avgAllocTime) {
                comparison.fastest = { algorithm, avgAllocTime: metrics.avgAllocTime };
            }
            
            if (metrics.successRate > comparison.mostReliable.successRate) {
                comparison.mostReliable = { algorithm, successRate: metrics.successRate };
            }
            
            if (metrics.fragmentationRatio < comparison.leastFragmented.fragmentationRatio) {
                comparison.leastFragmented = { algorithm, fragmentationRatio: metrics.fragmentationRatio };
            }
            
            if (metrics.throughput > comparison.highestThroughput.throughput) {
                comparison.highestThroughput = { algorithm, throughput: metrics.throughput };
            }
        }

        return comparison;
    }

    // Tạo biểu đồ so sánh
    generateComparisonChart(containerId) {
        const container = document.getElementById(containerId);
        if (!container || this.results.size === 0) return;

        const algorithms = Array.from(this.results.keys());
        const metrics = Array.from(this.results.values());

        // Tạo chart HTML
        container.innerHTML = `
            <div class="benchmark-charts">
                <div class="chart-section">
                    <h3><i class="fas fa-clock"></i> Allocation Time (ms)</h3>
                    <div class="chart-bars" id="allocTimeChart"></div>
                </div>
                
                <div class="chart-section">
                    <h3><i class="fas fa-check-circle"></i> Success Rate (%)</h3>
                    <div class="chart-bars" id="successRateChart"></div>
                </div>
                
                <div class="chart-section">
                    <h3><i class="fas fa-tachometer-alt"></i> Throughput (ops/sec)</h3>
                    <div class="chart-bars" id="throughputChart"></div>
                </div>
                
                <div class="chart-section">
                    <h3><i class="fas fa-puzzle-piece"></i> Fragmentation (%)</h3>
                    <div class="chart-bars" id="fragmentationChart"></div>
                </div>
            </div>
        `;

        // Render các biểu đồ
        this.renderBarChart('allocTimeChart', algorithms, 
            metrics.map(m => m.avgAllocTime), 'ms');
        this.renderBarChart('successRateChart', algorithms, 
            metrics.map(m => m.successRate), '%');
        this.renderBarChart('throughputChart', algorithms, 
            metrics.map(m => m.throughput), 'ops/sec');
        this.renderBarChart('fragmentationChart', algorithms, 
            metrics.map(m => m.fragmentationRatio * 100), '%');
    }

    renderBarChart(containerId, labels, values, unit) {
        const container = document.getElementById(containerId);
        if (!container) return;

        const maxValue = Math.max(...values);
        const colors = ['#2196F3', '#9c27b0', '#ff5722', '#673ab7'];

        container.innerHTML = labels.map((label, index) => {
            const value = values[index];
            const percentage = (value / maxValue) * 100;
            const color = colors[index % colors.length];

            return `
                <div class="chart-bar">
                    <div class="bar-label">${label}</div>
                    <div class="bar-container">
                        <div class="bar-fill" style="width: ${percentage}%; background-color: ${color}"></div>
                        <span class="bar-value">${value.toFixed(2)} ${unit}</span>
                    </div>
                </div>
            `;
        }).join('');
    }

    // Export results
    exportResults() {
        const data = {
            timestamp: new Date().toISOString(),
            results: Object.fromEntries(this.results),
            comparison: this.compareAlgorithms()
        };
        
        const blob = new Blob([JSON.stringify(data, null, 2)], 
            { type: 'application/json' });
        const url = URL.createObjectURL(blob);
        
        const a = document.createElement('a');
        a.href = url;
        a.download = `memory-allocator-benchmark-${Date.now()}.json`;
        a.click();
        
        URL.revokeObjectURL(url);
    }
}

// Export for use in other modules
window.MemoryAllocatorBenchmark = MemoryAllocatorBenchmark;
