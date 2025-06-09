// Memory Allocator Demo JavaScript

class MemoryAllocatorDemo {
    constructor() {
        this.currentAllocatorId = null;
        this.allocatorType = 'buddy';
        this.memorySize = 1024;
        this.allocations = [];
        this.stats = {
            totalAllocated: 0,
            totalFree: 1024,
            fragmentationRatio: 0,
            allocCount: 0,
            avgAllocTime: 0,
            peakUsage: 0
        };
        this.apiBaseUrl = 'http://localhost:3000/api';
        
        this.initializeEventListeners();
        this.initializeTabs();
        this.setupAlgorithmDetails();
    }

    initializeEventListeners() {
        // Tab switching
        document.querySelectorAll('.tab-button').forEach(button => {
            button.addEventListener('click', (e) => {
                this.switchTab(e.target.dataset.tab);
            });
        });

        // Demo controls
        document.getElementById('initAllocator').addEventListener('click', () => {
            this.initializeAllocator();
        });

        document.getElementById('allocateBtn').addEventListener('click', () => {
            this.performAllocation();
        });

        document.getElementById('deallocateBtn').addEventListener('click', () => {
            this.performDeallocation();
        });

        document.getElementById('defragmentBtn').addEventListener('click', () => {
            this.performDefragmentation();
        });        document.getElementById('resetBtn').addEventListener('click', () => {
            this.resetAllocator();
        });        // Test visualization button
        document.getElementById('testVisualizationBtn').addEventListener('click', () => {
            this.testVisualization();
        });

        // Test algorithm visualization button
        document.getElementById('testAlgorithmVisualizationBtn').addEventListener('click', () => {
            this.testAlgorithmVisualization();
        });

        // Algorithm selector
        document.getElementById('algorithmSelect').addEventListener('change', (e) => {
            this.updateAlgorithmDetails(e.target.value);
        });

        // Benchmark controls
        document.getElementById('startBenchmark').addEventListener('click', () => {
            this.startBenchmark();
        });

        document.getElementById('exportResults').addEventListener('click', () => {
            this.exportBenchmarkResults();
        });

        // Real-time updates
        document.getElementById('allocatorType').addEventListener('change', (e) => {
            this.allocatorType = e.target.value;
        });

        document.getElementById('memorySize').addEventListener('change', (e) => {
            this.memorySize = parseInt(e.target.value);
        });
    }

    initializeTabs() {
        this.switchTab('overview');
    }

    switchTab(tabName) {
        // Update tab buttons
        document.querySelectorAll('.tab-button').forEach(btn => {
            btn.classList.remove('active');
        });
        document.querySelector(`[data-tab="${tabName}"]`).classList.add('active');

        // Update tab content
        document.querySelectorAll('.tab-content').forEach(content => {
            content.classList.remove('active');
        });
        document.getElementById(tabName).classList.add('active');
    }

    setupAlgorithmDetails() {
        this.algorithmDetails = {
            buddy: {
                title: "Buddy System Allocator",
                description: "Thuật toán Buddy System chia bộ nhớ thành các khối có kích thước là lũy thừa của 2.",
                features: [
                    "Chia bộ nhớ theo cây nhị phân",
                    "Kích thước block luôn là lũy thừa của 2",
                    "Tự động coalescing khi deallocate",
                    "Giảm fragmentation ngoại bộ hiệu quả"
                ],
                complexity: {
                    allocation: "O(log n)",
                    deallocation: "O(log n)",
                    space: "O(n)"
                },
                advantages: [
                    "Coalescing tự động và hiệu quả",
                    "Giảm fragmentation ngoại bộ",
                    "Thuật toán đơn giản và ổn định"
                ],
                disadvantages: [
                    "Có thể có fragmentation nội bộ cao",
                    "Chỉ cấp phát kích thước lũy thừa của 2",
                    "Overhead cho metadata"
                ]
            },
            slab: {
                title: "Slab Allocator",
                description: "Slab Allocator tối ưu cho việc cấp phát các object có kích thước cố định.",
                features: [
                    "Cache các object có kích thước cố định",
                    "Allocation và deallocation O(1)",
                    "Cache-friendly memory layout",
                    "Giảm fragmentation nội bộ"
                ],
                complexity: {
                    allocation: "O(1)",
                    deallocation: "O(1)",
                    space: "O(n + k)"
                },
                advantages: [
                    "Allocation/deallocation rất nhanh",
                    "Cache locality tốt",
                    "Giảm fragmentation nội bộ",
                    "Phù hợp cho object pooling"
                ],
                disadvantages: [
                    "Chỉ hiệu quả với fixed-size allocations",
                    "Memory overhead cho metadata",
                    "Cần biết trước allocation patterns"
                ]
            },
            pool: {
                title: "Memory Pool Allocator",
                description: "Memory Pool pre-allocate các chunk lớn và chia nhỏ cho allocation requests.",
                features: [
                    "Pre-allocation memory chunks",
                    "Predictable memory usage",
                    "Fast allocation/deallocation",
                    "Bulk operations support"
                ],
                complexity: {
                    allocation: "O(1)",
                    deallocation: "O(1)",
                    space: "O(n)"
                },
                advantages: [
                    "Allocation/deallocation rất nhanh",
                    "Memory usage có thể dự đoán",
                    "Phù hợp cho real-time systems",
                    "Hỗ trợ bulk operations"
                ],
                disadvantages: [
                    "Cần biết trước memory requirements",
                    "Có thể waste memory nếu không sử dụng hết",
                    "Ít linh hoạt với dynamic workloads"
                ]
            },
            hybrid: {
                title: "Hybrid Allocator",
                description: "Hybrid Allocator kết hợp ưu điểm của nhiều thuật toán và tự động chọn strategy phù hợp.",
                features: [
                    "Adaptive algorithm selection",
                    "Pattern learning và optimization",
                    "Multiple allocation strategies",
                    "Performance monitoring và tuning"
                ],
                complexity: {
                    allocation: "O(1) - Adaptive",
                    deallocation: "O(1) - Adaptive",
                    space: "O(n + overhead)"
                },
                advantages: [
                    "Tối ưu cho mọi allocation pattern",
                    "Tự động học và cải thiện performance",
                    "Kết hợp ưu điểm của nhiều thuật toán",
                    "Phù hợp cho production systems"
                ],
                disadvantages: [
                    "Phức tạp trong implementation",
                    "Overhead cho monitoring và switching",
                    "Cần thời gian để học patterns"
                ]
            }
        };

        this.updateAlgorithmDetails('buddy');
    }

    updateAlgorithmDetails(algorithm) {
        const details = this.algorithmDetails[algorithm];
        const detailsContainer = document.getElementById('algorithmDetails');
        
        detailsContainer.innerHTML = `
            <div class="algorithm-detail">
                <h3>${details.title}</h3>
                <p class="description">${details.description}</p>
                
                <div class="detail-sections">
                    <div class="detail-section">
                        <h4><i class="fas fa-list"></i> Đặc điểm chính</h4>
                        <ul>
                            ${details.features.map(feature => `<li>${feature}</li>`).join('')}
                        </ul>
                    </div>
                    
                    <div class="detail-section">
                        <h4><i class="fas fa-clock"></i> Độ phức tạp</h4>
                        <div class="complexity-grid">
                            <div class="complexity-item">
                                <span class="complexity-label">Allocation:</span>
                                <span class="complexity-value">${details.complexity.allocation}</span>
                            </div>
                            <div class="complexity-item">
                                <span class="complexity-label">Deallocation:</span>
                                <span class="complexity-value">${details.complexity.deallocation}</span>
                            </div>
                            <div class="complexity-item">
                                <span class="complexity-label">Space:</span>
                                <span class="complexity-value">${details.complexity.space}</span>
                            </div>
                        </div>
                    </div>
                    
                    <div class="pros-cons">
                        <div class="detail-section pros">
                            <h4><i class="fas fa-thumbs-up"></i> Ưu điểm</h4>
                            <ul>
                                ${details.advantages.map(advantage => `<li>${advantage}</li>`).join('')}
                            </ul>
                        </div>
                        
                        <div class="detail-section cons">
                            <h4><i class="fas fa-thumbs-down"></i> Nhược điểm</h4>
                            <ul>
                                ${details.disadvantages.map(disadvantage => `<li>${disadvantage}</li>`).join('')}
                            </ul>
                        </div>
                    </div>
                </div>
            </div>
        `;
    }    async initializeAllocator() {
        this.allocatorType = document.getElementById('allocatorType').value;
        this.memorySize = parseInt(document.getElementById('memorySize').value);
        
        try {
            this.showMessage('Đang khởi tạo allocator...', 'info');
            
            const response = await fetch(`${this.apiBaseUrl}/allocator/init`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    type: this.allocatorType,
                    size: this.memorySize
                })
            });
            
            const result = await response.json();            if (result.success) {
                this.currentAllocatorId = result.instanceId || result.allocatorId;
                this.allocations = [];
                console.log('Allocator initialized successfully:', result);
                console.log('Current allocator ID set to:', this.currentAllocatorId);
                
                // Reset stats
                this.stats = {
                    totalAllocated: 0,
                    totalFree: this.memorySize,
                    fragmentationRatio: 0,
                    allocCount: 0,
                    avgAllocTime: 0,
                    peakUsage: 0
                };
                
                this.updateStats();
                console.log('About to update memory visualization...');
                this.updateMemoryVisualization();
                
                this.showMessage(`${this.allocatorType.toUpperCase()} Allocator khởi tạo thành công với ${this.memorySize} bytes`, 'success');
            } else {
                this.showMessage(`Lỗi khởi tạo allocator: ${result.error}`, 'error');
            }
        } catch (error) {
            console.error('Error initializing allocator:', error);
            this.showMessage('Không thể kết nối đến server', 'error');
              // Fallback to simulated allocator
            this.currentAllocatorId = 'simulated';
            this.currentAllocator = new SimulatedAllocator(this.allocatorType, this.memorySize);
            this.allocations = [];
            
            console.log('Created simulated allocator:', this.currentAllocator);
            
            this.stats = {
                totalAllocated: 0,
                totalFree: this.memorySize,
                fragmentationRatio: 0,
                allocCount: 0,
                avgAllocTime: 0,
                peakUsage: 0
            };
            
            this.updateStats();
            this.updateMemoryVisualization();
            
            this.showMessage(`${this.allocatorType.toUpperCase()} Allocator khởi tạo (simulation mode) với ${this.memorySize} bytes`, 'info');
        }
    }    async performAllocation() {
        if (!this.currentAllocatorId) {
            this.showMessage('Vui lòng khởi tạo allocator trước', 'error');
            return;
        }

        const size = parseInt(document.getElementById('allocSize').value);
        if (size <= 0) {
            this.showMessage('Kích thước allocation phải lớn hơn 0', 'error');
            return;
        }

        const startTime = performance.now();
        
        try {
            let result;
            
            if (this.currentAllocatorId === 'simulated') {
                // Use simulated allocator
                result = this.currentAllocator.allocate(size);
            } else {
                // Use API
                const response = await fetch(`${this.apiBaseUrl}/allocator/${this.currentAllocatorId}/allocate`, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({ size: size })
                });
                
                result = await response.json();
            }
            
            const allocTime = performance.now() - startTime;

            if (result.success) {
                this.allocations.push({
                    id: result.id,
                    size: size,
                    address: result.address,
                    time: allocTime
                });

                // Update stats
                this.stats.totalAllocated += size;
                this.stats.totalFree -= size;
                this.stats.allocCount++;
                this.stats.avgAllocTime = ((this.stats.avgAllocTime * (this.stats.allocCount - 1)) + allocTime) / this.stats.allocCount;
                
                if (this.stats.totalAllocated > this.stats.peakUsage) {
                    this.stats.peakUsage = this.stats.totalAllocated;
                }                this.updateStats();
                this.updateMemoryVisualization();
                this.refreshBuddyTreeVisualization(); // Add this line to refresh buddy tree
                this.refreshSlabVisualization(); // Add this line to refresh slab visualization
                this.showMessage(`Allocated ${size} bytes tại địa chỉ ${result.address}`, 'success');
            } else {
                this.showMessage(`Allocation thất bại: ${result.error}`, 'error');
            }
        } catch (error) {
            console.error('Error performing allocation:', error);
            this.showMessage('Lỗi kết nối server trong quá trình allocation', 'error');
        }
    }    async performDeallocation() {
        if (!this.currentAllocatorId || this.allocations.length === 0) {
            this.showMessage('Không có allocation nào để deallocate', 'error');
            return;
        }

        // Deallocate the last allocation
        const lastAlloc = this.allocations.pop();
        
        try {
            let result;
            
            if (this.currentAllocatorId === 'simulated') {
                // Use simulated allocator
                result = this.currentAllocator.deallocate(lastAlloc.id);
            } else {
                // Use API
                const response = await fetch(`${this.apiBaseUrl}/allocator/${this.currentAllocatorId}/deallocate`, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({ id: lastAlloc.id })
                });
                
                result = await response.json();
            }

            if (result.success) {
                this.stats.totalAllocated -= lastAlloc.size;
                this.stats.totalFree += lastAlloc.size;                this.updateStats();
                this.updateMemoryVisualization();
                this.refreshBuddyTreeVisualization(); // Add this line to refresh buddy tree
                this.refreshSlabVisualization(); // Add this line to refresh slab visualization
                this.showMessage(`Deallocated ${lastAlloc.size} bytes từ địa chỉ ${lastAlloc.address}`, 'success');
            } else {
                // Put allocation back if deallocation failed
                this.allocations.push(lastAlloc);
                this.showMessage(`Deallocation thất bại: ${result.error}`, 'error');
            }
        } catch (error) {
            // Put allocation back if there was an error
            this.allocations.push(lastAlloc);
            console.error('Error performing deallocation:', error);
            this.showMessage('Lỗi kết nối server trong quá trình deallocation', 'error');
        }
    }    async performDefragmentation() {
        if (!this.currentAllocatorId) {
            this.showMessage('Vui lòng khởi tạo allocator trước', 'error');
            return;
        }

        const startTime = performance.now();
        
        try {
            if (this.currentAllocatorId === 'simulated') {
                // Use simulated allocator
                this.currentAllocator.defragment();
            } else {
                // Use API
                const response = await fetch(`${this.apiBaseUrl}/allocator/${this.currentAllocatorId}/defragment`, {
                    method: 'POST'
                });
                
                const result = await response.json();
                if (!result.success) {
                    this.showMessage(`Defragmentation thất bại: ${result.error}`, 'error');
                    return;
                }
            }
            
            const defragTime = performance.now() - startTime;

            this.updateStats();
            this.updateMemoryVisualization();
            this.showMessage(`Defragmentation hoàn thành trong ${defragTime.toFixed(2)}ms`, 'success');
        } catch (error) {
            console.error('Error performing defragmentation:', error);
            this.showMessage('Lỗi kết nối server trong quá trình defragmentation', 'error');
        }
    }

    async resetAllocator() {
        if (!this.currentAllocatorId) {
            this.showMessage('Không có allocator nào để reset', 'error');
            return;
        }
        
        try {
            if (this.currentAllocatorId !== 'simulated') {
                // Use API to reset
                const response = await fetch(`${this.apiBaseUrl}/allocator/${this.currentAllocatorId}/reset`, {
                    method: 'POST'
                });
                
                const result = await response.json();
                if (!result.success) {
                    this.showMessage(`Reset thất bại: ${result.error}`, 'error');
                    return;
                }
            }
            
            this.currentAllocatorId = null;
            this.currentAllocator = null;
            this.allocations = [];
            this.stats = {
                totalAllocated: 0,
                totalFree: this.memorySize,
                fragmentationRatio: 0,
                allocCount: 0,
                avgAllocTime: 0,
                peakUsage: 0
            };

            this.updateStats();
            this.updateMemoryVisualization();
            this.showMessage('Allocator đã được reset', 'info');
        } catch (error) {
            console.error('Error resetting allocator:', error);
            this.showMessage('Lỗi kết nối server trong quá trình reset', 'error');
        }
    }    async updateStats() {
        // Calculate fragmentation
        try {
            if (this.currentAllocatorId && this.currentAllocatorId !== 'simulated') {
                const response = await fetch(`${this.apiBaseUrl}/allocator/${this.currentAllocatorId}/stats`);
                const stats = await response.json();
                
                if (stats.success) {
                    this.stats.fragmentationRatio = stats.data.fragmentation;
                    // Update other stats from server if available
                    if (stats.data.totalAllocated !== undefined) {
                        this.stats.totalAllocated = stats.data.totalAllocated;
                    }
                    if (stats.data.totalFree !== undefined) {
                        this.stats.totalFree = stats.data.totalFree;
                    }
                }
            } else if (this.currentAllocator) {
                this.stats.fragmentationRatio = this.currentAllocator.getFragmentation();
            }
        } catch (error) {
            console.error('Error getting stats:', error);
            // Continue with local stats
        }

        // Update UI
        document.getElementById('totalAllocated').textContent = this.stats.totalAllocated;
        document.getElementById('totalFree').textContent = this.stats.totalFree;
        document.getElementById('fragmentationRatio').textContent = (this.stats.fragmentationRatio * 100).toFixed(1) + '%';
        document.getElementById('allocCount').textContent = this.stats.allocCount;
        document.getElementById('avgAllocTime').textContent = this.stats.avgAllocTime.toFixed(2);
        document.getElementById('peakUsage').textContent = this.stats.peakUsage;    }    async updateMemoryVisualization() {
        if (!this.currentAllocatorId) {
            console.log('No allocator ID available for visualization');
            return;
        }

        console.log('Fetching layout for allocator:', this.currentAllocatorId);
        
        try {
            let layout;
            if (this.currentAllocatorId === 'simulated') {
                layout = {
                    blocks: this.currentAllocator.getMemoryLayout(),
                    totalSize: this.currentAllocator.totalSize,
                    allocatedBlocks: Array.from(this.currentAllocator.allocatedBlocks.values()),
                    freeBlocks: this.currentAllocator.freeBlocks
                };
            } else {
                const response = await fetch(`${this.apiBaseUrl}/allocator/${this.currentAllocatorId}/layout`);
                const result = await response.json();
                
                if (result.success) {
                    layout = result.layout || result.data;
                } else {
                    console.error('Layout fetch failed:', result.error);
                    this.showMessage(`Lỗi cập nhật visualization: ${result.error}`, 'error');
                    return;
                }
            }
            
            console.log('Rendering visualization with layout:', layout);
            this.renderAlgorithmSpecificVisualization(layout);        } catch (error) {
            console.error('Error fetching layout:', error);
            this.showMessage('Không thể cập nhật visualization', 'error');
        }
    }

    testVisualization() {
        console.log('=== Testing Visualization ===');
        const canvas = document.getElementById('memoryCanvas');
        canvas.innerHTML = '';

        // Test data
        const testBlocks = [
            { size: 256, type: 'free' },
            { size: 128, type: 'allocated' },
            { size: 64, type: 'allocated' },
            { size: 512, type: 'free' },
            { size: 32, type: 'allocated' },
            { size: 32, type: 'fragmented' }
        ];

        const totalSize = 1024;
        const availableWidth = canvas.clientWidth - 40;
        const minBlockWidth = 10;

        console.log('Canvas width:', canvas.clientWidth);
        console.log('Available width:', availableWidth);
        console.log('Test blocks:', testBlocks);

        testBlocks.forEach((block, index) => {
            const proportionalWidth = Math.max(minBlockWidth, (block.size / totalSize) * availableWidth);
            
            const blockElement = document.createElement('div');
            blockElement.className = `memory-block ${block.type}`;
            blockElement.style.width = `${proportionalWidth}px`;
            blockElement.style.minWidth = `${minBlockWidth}px`;
            
            if (proportionalWidth > 30) {
                blockElement.textContent = block.size;
            } else {
                blockElement.textContent = '';
            }
            
            blockElement.title = `Test Block ${index}: ${block.size} bytes - ${block.type}`;
            
            canvas.appendChild(blockElement);
            
            console.log(`Added test block ${index}: size=${block.size}, width=${proportionalWidth}px, type=${block.type}`);
        });

        this.showMessage('Test visualization completed - check console for details', 'info');
    }

    // Test function to force show algorithm-specific visualizations
    testAlgorithmVisualization() {
        console.log('=== Testing Algorithm Visualization ===');
        const canvas = document.getElementById('memoryCanvas');
        canvas.innerHTML = '';

        // Create mock layout data
        const mockLayout = {
            blocks: [
                { size: 256, type: 'free' },
                { size: 128, type: 'allocated' },
                { size: 64, type: 'allocated' },
                { size: 512, type: 'free' },
                { size: 32, type: 'allocated' },
                { size: 32, type: 'fragmented' }
            ],
            totalSize: this.memorySize,
            allocatedBlocks: [],
            freeBlocks: []
        };

        // Set some mock allocations for testing
        this.allocations = [
            { id: 1, size: 128, address: '0x00001000' },
            { id: 2, size: 64, address: '0x00002000' },
            { id: 3, size: 32, address: '0x00003000' }
        ];

        console.log('Current allocator type:', this.allocatorType);
        console.log('Mock layout:', mockLayout);
        console.log('Mock allocations:', this.allocations);

        // Force call the visualization
        this.renderAlgorithmSpecificVisualization(mockLayout);

        this.showMessage(`Testing ${this.allocatorType} visualization with mock data`, 'info');
    }

    showMessage(message, type = 'info') {
        // Create message element
        const messageEl = document.createElement('div');
        messageEl.className = `message message-${type}`;
        messageEl.innerHTML = `
            <i class="fas fa-${type === 'success' ? 'check-circle' : type === 'error' ? 'exclamation-circle' : 'info-circle'}"></i>
            ${message}
        `;

        // Add to page
        document.body.appendChild(messageEl);

        // Auto remove after 3 seconds
        setTimeout(() => {
            messageEl.remove();
        }, 3000);
    }    async runBenchmark() {
        const benchmarkType = document.getElementById('benchmarkType').value;
        const iterations = parseInt(document.getElementById('iterations').value);
        
        this.showMessage(`Đang chạy benchmark ${benchmarkType} với ${iterations} iterations...`, 'info');
        
        try {
            if (this.currentAllocatorId && this.currentAllocatorId !== 'simulated') {
                // Use API benchmark
                const response = await fetch(`${this.apiBaseUrl}/allocator/${this.currentAllocatorId}/benchmark`, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                    },
                    body: JSON.stringify({
                        type: benchmarkType,
                        iterations: iterations
                    })
                });
                
                const result = await response.json();
                
                if (result.success) {
                    this.displayBenchmarkResults(result.data);
                } else {
                    this.showMessage(`Benchmark thất bại: ${result.error}`, 'error');
                }
            } else {
                // Simulate benchmark for fallback
                setTimeout(() => {
                    const results = this.simulateBenchmark(benchmarkType, iterations);
                    this.displayBenchmarkResults(results);
                }, 1000);
            }
        } catch (error) {
            console.error('Error running benchmark:', error);
            this.showMessage('Lỗi kết nối server trong quá trình benchmark', 'error');
            
            // Fallback to simulation
            setTimeout(() => {
                const results = this.simulateBenchmark(benchmarkType, iterations);
                this.displayBenchmarkResults(results);
            }, 1000);
        }
    }

    simulateBenchmark(type, iterations) {
        // Simulate different benchmark scenarios
        const results = {
            type: type,
            iterations: iterations,
            totalTime: Math.random() * 1000 + 100,
            avgTime: 0,
            minTime: Math.random() * 10 + 1,
            maxTime: Math.random() * 50 + 20,
            successRate: 95 + Math.random() * 5,
            fragmentation: Math.random() * 0.3
        };

        results.avgTime = results.totalTime / iterations;

        return results;
    }

    displayBenchmarkResults(results) {
        const resultsContainer = document.getElementById('benchmarkResults');
        resultsContainer.innerHTML = `
            <h3><i class="fas fa-chart-bar"></i> Kết quả Benchmark: ${results.type}</h3>
            <div class="benchmark-result">
                <span class="benchmark-label">Tổng thời gian:</span>
                <span class="benchmark-value">${results.totalTime.toFixed(2)} ms</span>
            </div>
            <div class="benchmark-result">
                <span class="benchmark-label">Thời gian trung bình:</span>
                <span class="benchmark-value">${results.avgTime.toFixed(4)} ms</span>
            </div>
            <div class="benchmark-result">
                <span class="benchmark-label">Thời gian tối thiểu:</span>
                <span class="benchmark-value">${results.minTime.toFixed(4)} ms</span>
            </div>
            <div class="benchmark-result">
                <span class="benchmark-label">Thời gian tối đa:</span>
                <span class="benchmark-value">${results.maxTime.toFixed(4)} ms</span>
            </div>
            <div class="benchmark-result">
                <span class="benchmark-label">Tỷ lệ thành công:</span>
                <span class="benchmark-value">${results.successRate.toFixed(1)}%</span>
            </div>
            <div class="benchmark-result">
                <span class="benchmark-label">Fragmentation cuối:</span>
                <span class="benchmark-value">${(results.fragmentation * 100).toFixed(1)}%</span>
            </div>
        `;

        this.showMessage('Benchmark hoàn thành!', 'success');
    }

    async startBenchmark() {
        const memorySize = parseInt(document.getElementById('benchmarkMemorySize').value) || 8192;
        const testCount = parseInt(document.getElementById('benchmarkTestCount').value) || 1000;
        
        if (!window.MemoryAllocatorBenchmark) {
            console.error('Benchmark module not loaded');
            return;
        }

        const benchmark = new MemoryAllocatorBenchmark();
        const startBtn = document.getElementById('startBenchmark');
        const progressContainer = document.querySelector('.benchmark-progress');
        const resultsContainer = document.getElementById('benchmarkResults');
        
        // Show progress and disable button
        startBtn.disabled = true;
        startBtn.innerHTML = '<i class="fas fa-spinner fa-spin"></i> Running Benchmark...';
        progressContainer.style.display = 'block';
        
        try {
            const results = await benchmark.benchmarkAll(memorySize, testCount);
            
            // Generate comparison chart
            benchmark.generateComparisonChart('benchmarkResults');
            
            // Show summary
            this.displayBenchmarkSummary(benchmark.compareAlgorithms());
            
            // Store benchmark instance for export
            this.lastBenchmark = benchmark;
            
            console.log('Benchmark completed successfully!');
            
        } catch (error) {
            console.error('Benchmark failed:', error);
            resultsContainer.innerHTML = `
                <div class="error-message">
                    <i class="fas fa-exclamation-triangle"></i>
                    Benchmark failed: ${error.message}
                </div>
            `;
        } finally {
            // Reset UI
            startBtn.disabled = false;
            startBtn.innerHTML = '<i class="fas fa-play"></i> Start Benchmark';
            progressContainer.style.display = 'none';
        }
    }

    displayBenchmarkSummary(comparison) {
        if (!comparison) return;
        
        const summaryContainer = document.getElementById('benchmarkSummary');
        if (!summaryContainer) return;
        
        summaryContainer.innerHTML = `
            <div class="summary-grid">
                <div class="summary-card">
                    <div class="icon"><i class="fas fa-rocket"></i></div>
                    <div class="title">Fastest Allocation</div>
                    <div class="algorithm">${comparison.fastest.algorithm}</div>
                    <div class="value">${comparison.fastest.avgAllocTime.toFixed(2)}ms</div>
                </div>
                
                <div class="summary-card">
                    <div class="icon"><i class="fas fa-shield-alt"></i></div>
                    <div class="title">Most Reliable</div>
                    <div class="algorithm">${comparison.mostReliable.algorithm}</div>
                    <div class="value">${comparison.mostReliable.successRate.toFixed(1)}%</div>
                </div>
                
                <div class="summary-card">
                    <div class="icon"><i class="fas fa-puzzle-piece"></i></div>
                    <div class="title">Least Fragmented</div>
                    <div class="algorithm">${comparison.leastFragmented.algorithm}</div>
                    <div class="value">${(comparison.leastFragmented.fragmentationRatio * 100).toFixed(1)}%</div>
                </div>
                
                <div class="summary-card">
                    <div class="icon"><i class="fas fa-tachometer-alt"></i></div>
                    <div class="title">Highest Throughput</div>
                    <div class="algorithm">${comparison.highestThroughput.algorithm}</div>
                    <div class="value">${comparison.highestThroughput.throughput.toFixed(0)} ops/sec</div>
                </div>
            </div>
        `;
    }

    exportBenchmarkResults() {
        if (this.lastBenchmark) {
            this.lastBenchmark.exportResults();
        } else {
            alert('No benchmark results to export. Please run a benchmark first.');
        }
    }

    renderAlgorithmSpecificVisualization(layout) {
        console.log('=== renderAlgorithmSpecificVisualization START ===');
        console.log('Allocator type:', this.allocatorType);
        console.log('Layout received:', layout);
        console.log('Current allocations:', this.allocations);        console.log('Memory size:', this.memorySize);
        
        const canvas = document.getElementById('memoryCanvas');
        if (!canvas) {
            console.error('Canvas element not found!');
            return;
        }
        
        // Clear canvas completely
        canvas.innerHTML = '';

        if (!layout || !layout.blocks) {
            console.log('No layout data available, creating demo layout...');
            // Create demo layout for visualization
            layout = {
                blocks: [
                    { size: this.memorySize * 0.4, type: 'allocated' },
                    { size: this.memorySize * 0.3, type: 'free' },
                    { size: this.memorySize * 0.2, type: 'allocated' },
                    { size: this.memorySize * 0.1, type: 'free' }
                ]
            };
        }

        // Create visualization container
        const vizContainer = document.createElement('div');
        vizContainer.className = `visualization-container ${this.allocatorType}-visualization`;
        
        // Apply forced styling for visibility
        vizContainer.style.cssText = `
            display: block !important;
            visibility: visible !important;
            opacity: 1 !important;
            width: 100%;
            margin: 20px 0;
            background: #ffffff;
            border-radius: 12px;
            box-shadow: 0 8px 32px rgba(0,0,0,0.1);
            overflow: visible;
        `;

        // Add header with memory info
        const header = document.createElement('div');
        header.className = 'visualization-header';
        header.style.cssText = `
            background: linear-gradient(135deg, #2196F3, #21CBF3);
            color: white;
            padding: 20px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            border-radius: 12px 12px 0 0;
        `;
        header.innerHTML = `
            <h3 style="font-size: 1.4rem; font-weight: 600; margin: 0;">
                <i class="fas fa-microchip" style="margin-right: 10px;"></i> 
                ${this.getAlgorithmTitle()} Memory Layout
            </h3>
            <div class="memory-info" style="display: flex; gap: 20px; font-size: 0.9rem;">
                <span style="background: rgba(255,255,255,0.2); padding: 5px 12px; border-radius: 20px; font-weight: 500;">Total: ${this.formatBytes(this.memorySize)}</span>
                <span style="background: rgba(255,255,255,0.2); padding: 5px 12px; border-radius: 20px; font-weight: 500;">Allocated: ${this.formatBytes(this.stats.totalAllocated)}</span>
                <span style="background: rgba(255,255,255,0.2); padding: 5px 12px; border-radius: 20px; font-weight: 500;">Free: ${this.formatBytes(this.stats.totalFree)}</span>
                <span style="background: rgba(255,255,255,0.2); padding: 5px 12px; border-radius: 20px; font-weight: 500;">Fragmentation: ${(this.stats.fragmentationRatio * 100).toFixed(1)}%</span>
            </div>
        `;
        vizContainer.appendChild(header);

        // Add linear memory layout
        console.log('Creating linear memory visualization...');
        const linearSection = this.createLinearMemoryVisualization(layout);
        vizContainer.appendChild(linearSection);        // Add algorithm-specific visualization
        console.log('Creating algorithm-specific visualization...');
        const algorithmSection = this.createAlgorithmSpecificSection(layout);
        if (algorithmSection) {
            console.log('Algorithm section created, appending to container');
            console.log('Algorithm section HTML:', algorithmSection.outerHTML.substring(0, 300) + '...');
            vizContainer.appendChild(algorithmSection);
            
            // Double-check that the section was added
            setTimeout(() => {
                const addedSection = vizContainer.querySelector('.algorithm-specific-viz');
                if (addedSection) {
                    console.log('✓ Algorithm section successfully added to DOM');
                    console.log('Section rect:', addedSection.getBoundingClientRect());
                    console.log('Section style display:', window.getComputedStyle(addedSection).display);
                    console.log('Section style visibility:', window.getComputedStyle(addedSection).visibility);
                } else {
                    console.error('✗ Algorithm section NOT found in DOM after appendChild');
                }
            }, 50);
        } else {
            console.warn('Algorithm section is null! Creating fallback section...');
            // Create fallback visualization
            const fallbackSection = document.createElement('div');
            fallbackSection.className = 'algorithm-specific-viz fallback-viz';
            fallbackSection.style.cssText = `
                display: block !important;
                visibility: visible !important;
                opacity: 1 !important;
                padding: 20px;
                margin: 20px;
                background: #fff3e0;
                border: 3px solid #ff9800;
                border-radius: 12px;
                text-align: center;
            `;
            fallbackSection.innerHTML = `
                <h4 style="color: #f57c00; margin-bottom: 15px;">
                    <i class="fas fa-exclamation-triangle"></i> Algorithm Visualization
                </h4>
                <p style="color: #ef6c00;">
                    Visualization for ${this.allocatorType} algorithm is currently being generated...
                </p>
                <div style="margin-top: 15px; padding: 10px; background: rgba(255,152,0,0.1); border-radius: 8px;">
                    <small style="color: #e65100;">Algorithm: ${this.getAlgorithmTitle()}</small>
                </div>
            `;
            vizContainer.appendChild(fallbackSection);
        }        // Append the complete visualization container to canvas
        canvas.appendChild(vizContainer);
        console.log('=== renderAlgorithmSpecificVisualization END ===');
        console.log('Final canvas HTML:', canvas.innerHTML.substring(0, 500) + '...');
        
        // Debug DOM structure
        setTimeout(() => {
            this.debugVisualizationDOM();
        }, 100);
    }    createLinearMemoryVisualization(layout) {
        const section = document.createElement('div');
        section.className = 'linear-memory-section';
        section.style.cssText = `
            padding: 20px;
            background: #f8f9fa;
            margin: 0;
            border-radius: 0;
        `;
        
        section.innerHTML = `
            <h4 style="color: #333; font-size: 1.2rem; margin-bottom: 15px; display: flex; align-items: center;">
                <i class="fas fa-layer-group" style="margin-right: 10px;"></i> Linear Memory Layout
            </h4>
            <div class="linear-memory-container" style="background: #ffffff; border-radius: 8px; padding: 20px;">
                <div class="memory-blocks" id="memoryBlocks" style="display: flex; width: 100%; min-height: 60px; border-radius: 8px; overflow: hidden; box-shadow: 0 2px 8px rgba(0,0,0,0.1);">
                    ${this.generateLinearMemoryBlocks(layout.blocks)}
                </div>
            </div>
        `;
        return section;
    }

    generateLinearMemoryBlocks(blocks) {
        if (!blocks || blocks.length === 0) {
            return '<div class="no-blocks">No memory blocks to display</div>';
        }

        const totalSize = this.memorySize;
        return blocks.map((block, index) => {
            const width = (block.size / totalSize) * 100;
            const widthStyle = width < 2 ? 'min-width: 20px;' : '';
            
            return `
                <div class="memory-block memory-block-${block.type}" 
                     style="width: ${width}%; ${widthStyle}"
                     title="Block ${index}: ${this.formatBytes(block.size)} - ${block.type}">
                    ${width > 5 ? this.formatBytes(block.size) : ''}
                </div>
            `;
        }).join('');
    }

    createAlgorithmSpecificSection(layout) {
        console.log('createAlgorithmSpecificSection called with type:', this.allocatorType);
        
        switch(this.allocatorType) {
            case 'buddy':
                console.log('Creating buddy visualization...');
                const buddyViz = this.createBuddyVisualization(layout);
                console.log('Buddy visualization result:', buddyViz ? 'SUCCESS' : 'NULL');
                return buddyViz;
            case 'slab':
                console.log('Creating slab visualization...');
                const slabViz = this.createSlabVisualization(layout);
                console.log('Slab visualization result:', slabViz ? 'SUCCESS' : 'NULL');
                return slabViz;
            case 'pool':
                console.log('Creating pool visualization...');
                const poolViz = this.createPoolVisualization(layout);
                console.log('Pool visualization result:', poolViz ? 'SUCCESS' : 'NULL');
                return poolViz;
            case 'hybrid':
                console.log('Creating hybrid visualization...');
                const hybridViz = this.createHybridVisualization(layout);
                console.log('Hybrid visualization result:', hybridViz ? 'SUCCESS' : 'NULL');
                return hybridViz;
            default:
                console.warn('Unknown allocator type:', this.allocatorType);
                return null;
        }
    }    createBuddyVisualization(layout) {
        console.log('createBuddyVisualization called');
        
        const container = document.createElement('div');
        container.className = 'buddy-container algorithm-specific-viz';
        // Force visibility with inline styles
        container.style.display = 'block !important';
        container.style.visibility = 'visible !important';
        container.style.opacity = '1 !important';
        container.style.border = '3px solid red'; // Debug border
        container.style.minHeight = '300px'; // Ensure minimum height
        container.style.background = '#ffeeee'; // Debug background
        container.style.padding = '20px';
        container.style.margin = '20px 0';        container.style.borderRadius = '12px';
          console.log('Generating buddy tree HTML...');
        const buddyTreeHTML = this.generateBuddyTree(layout);
        console.log('Buddy tree HTML generated, length:', buddyTreeHTML.length);
        
        container.innerHTML = `
            <h4 style="color: #2196F3; margin-bottom: 15px;"><i class="fas fa-sitemap"></i> Buddy System Tree Structure</h4>
            <div class="buddy-tree-section">
                <div class="buddy-tree" id="buddyTree" style="background: #ffffff; padding: 20px; border-radius: 8px; min-height: 200px;">
                    ${buddyTreeHTML}
                </div>
            </div>
            <div class="buddy-stats" style="margin-top: 20px; background: #ffffff; padding: 15px; border-radius: 8px; display: flex; gap: 15px; flex-wrap: wrap;">
                <div class="buddy-stat" style="text-align: center; flex: 1; min-width: 120px;">
                    <span class="stat-label" style="display: block; font-size: 0.9rem; color: #666; margin-bottom: 5px;">Max Block Size:</span>
                    <span class="stat-value" style="display: block; font-size: 1.1rem; font-weight: bold; color: #2196F3;">${this.formatBytes(this.memorySize)}</span>
                </div>
                <div class="buddy-stat" style="text-align: center; flex: 1; min-width: 120px;">
                    <span class="stat-label" style="display: block; font-size: 0.9rem; color: #666; margin-bottom: 5px;">Min Block Size:</span>
                    <span class="stat-value" style="display: block; font-size: 1.1rem; font-weight: bold; color: #2196F3;">32B</span>
                </div>
                <div class="buddy-stat" style="text-align: center; flex: 1; min-width: 120px;">
                    <span class="stat-label" style="display: block; font-size: 0.9rem; color: #666; margin-bottom: 5px;">Current Tree Levels:</span>
                    <span class="stat-value" style="display: block; font-size: 1.1rem; font-weight: bold; color: #4CAF50;" id="currentTreeLevels">${this.calculateCurrentTreeLevels()}</span>
                </div>
                <div class="buddy-stat" style="text-align: center; flex: 1; min-width: 120px;">
                    <span class="stat-label" style="display: block; font-size: 0.9rem; color: #666; margin-bottom: 5px;">Active Allocations:</span>
                    <span class="stat-value" style="display: block; font-size: 1.1rem; font-weight: bold; color: #FF9800;">${this.allocations ? this.allocations.length : 0}</span>
                </div>
            </div>
        `;
        
        console.log('Buddy container HTML set, innerHTML length:', container.innerHTML.length);
        return container;
    }

    createSlabVisualization(layout) {
        console.log('createSlabVisualization called');
        
        const container = document.createElement('div');
        container.className = 'slab-container algorithm-specific-viz';
        // Force visibility with inline styles - same as other visualizations
        container.style.display = 'block !important';
        container.style.visibility = 'visible !important';
        container.style.opacity = '1 !important';
        container.style.border = '3px solid #9c27b0'; // Purple theme for slab
        container.style.minHeight = '300px'; // Ensure minimum height
        container.style.background = '#f3e5f5'; // Light purple background
        container.style.padding = '20px';
        container.style.margin = '20px 0';
        container.style.borderRadius = '12px';
        
        console.log('Generating slab caches HTML...');
        const slabCachesHTML = this.generateSlabCaches(layout);
        console.log('Slab caches HTML generated, length:', slabCachesHTML.length);
        
        container.innerHTML = `
            <h4 style="color: #9c27b0; margin-bottom: 15px;"><i class="fas fa-database"></i> Slab Cache Objects</h4>
            <div class="slab-cache-section">
                <div class="slab-caches" style="display: grid; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); gap: 20px; padding: 20px; background: #ffffff; border-radius: 12px; min-height: 200px; border: 2px solid #9c27b0;">
                    ${slabCachesHTML}
                </div>
            </div>
            <div class="slab-stats" style="margin-top: 20px; background: #ffffff; padding: 15px; border-radius: 8px; display: flex; gap: 15px; flex-wrap: wrap;">
                <div class="slab-stat" style="text-align: center; flex: 1; min-width: 120px;">
                    <span class="stat-label" style="display: block; font-size: 0.9rem; color: #666; margin-bottom: 5px;">Total Caches:</span>
                    <span class="stat-value" style="display: block; font-size: 1.1rem; font-weight: bold; color: #9c27b0;" id="totalSlabCaches">${this.calculateTotalSlabCaches()}</span>
                </div>
                <div class="slab-stat" style="text-align: center; flex: 1; min-width: 120px;">
                    <span class="stat-label" style="display: block; font-size: 0.9rem; color: #666; margin-bottom: 5px;">Active Objects:</span>
                    <span class="stat-value" style="display: block; font-size: 1.1rem; font-weight: bold; color: #9c27b0;" id="activeSlabObjects">${this.calculateActiveSlabObjects()}</span>
                </div>
                <div class="slab-stat" style="text-align: center; flex: 1; min-width: 120px;">
                    <span class="stat-label" style="display: block; font-size: 0.9rem; color: #666; margin-bottom: 5px;">Cache Efficiency:</span>
                    <span class="stat-value" style="display: block; font-size: 1.1rem; font-weight: bold; color: #4CAF50;" id="slabEfficiency">${this.calculateSlabEfficiency()}%</span>
                </div>
                <div class="slab-stat" style="text-align: center; flex: 1; min-width: 120px;">
                    <span class="stat-label" style="display: block; font-size: 0.9rem; color: #666; margin-bottom: 5px;">Memory Overhead:</span>
                    <span class="stat-value" style="display: block; font-size: 1.1rem; font-weight: bold; color: #FF9800;" id="slabOverhead">${this.calculateSlabOverhead()}%</span>
                </div>
            </div>
        `;
        
        console.log('Slab container HTML set, innerHTML length:', container.innerHTML.length);
        return container;
    }    createPoolVisualization(layout) {
        console.log('createPoolVisualization called');
        
        const container = document.createElement('div');
        container.className = 'pool-container algorithm-specific-viz';
        // Force visibility with inline styles
        container.style.display = 'block !important';
        container.style.visibility = 'visible !important';
        container.style.opacity = '1 !important';
        container.style.border = '3px solid #ff5722'; // Debug border
        container.style.minHeight = '300px'; // Ensure minimum height
        container.style.background = '#fbe9e7'; // Pool background
        container.style.padding = '20px';
        container.style.margin = '20px 0';
        container.style.borderRadius = '12px';
          console.log('Generating pool chunks HTML...');
        const poolChunksHTML = this.generatePoolChunks(layout);
        console.log('Pool chunks HTML generated, length:', poolChunksHTML.length);
        
        container.innerHTML = `
            <h4 style="color: #ff5722; margin-bottom: 15px;"><i class="fas fa-cubes"></i> Memory Pool Chunks</h4>
            <div class="pool-chunk-section">
                <div class="pool-chunks" style="display: flex; gap: 15px; align-items: flex-end; padding: 20px; background: #ffffff; border-radius: 12px; min-height: 200px; border: 2px solid #ff5722; overflow-x: auto;">
                    ${poolChunksHTML}
                </div>
            </div>
        `;
        
        console.log('Pool container HTML set, innerHTML length:', container.innerHTML.length);
        return container;
    }

    createHybridVisualization(layout) {
        const container = document.createElement('div');
        container.className = 'hybrid-container algorithm-specific-viz';
          container.innerHTML = `
            <h4><i class="fas fa-brain"></i> Hybrid Algorithm Strategies</h4>
            <div class="hybrid-strategy-section">
                <div class="hybrid-strategies">
                    ${this.generateHybridStrategies(layout)}
                </div>
            </div>
        `;
        
        return container;
    }    generateBuddyTree(data = {}) {
        let html = '';
        
        // Use data from parameter or fallback to instance data
        const blocks = data.blocks || this.allocations || [];
        const totalSize = data.totalSize || this.memorySize || 1024;
        const minBlockSize = 32; // Minimum allocation unit (từ C++ code)
        
        console.log('Generating dynamic buddy tree with data:', { blocks, totalSize, minBlockSize });
        console.log('Current allocations for tree:', this.allocations);
        
        // If we have specific blocks data with level information, use it directly
        if (blocks.length > 0 && blocks[0].level !== undefined) {
            // Group blocks by level
            const blocksByLevel = {};
            blocks.forEach(block => {
                if (!blocksByLevel[block.level]) {
                    blocksByLevel[block.level] = [];
                }
                blocksByLevel[block.level].push(block);
            });
            
            // Render levels
            const levels = Object.keys(blocksByLevel).map(Number).sort((a, b) => a - b);
            levels.forEach(level => {
                html += `<div class="buddy-level buddy-level-${level}">`;
                blocksByLevel[level].forEach(block => {
                    const status = block.allocated ? 'allocated' : 'free';
                    html += `
                        <div class="buddy-node ${status}" data-level="${level}" data-size="${block.size}">
                            <div class="node-header">Level ${level}</div>
                            <div class="node-size">${block.size}B</div>
                            <div class="node-status">${status.toUpperCase()}</div>
                            <div class="node-address">${block.address || 'N/A'}</div>
                        </div>
                    `;
                });
                html += `</div>`;
            });
        } else {
            // Generate dynamic buddy tree based on current allocations only
            // Calculate the maximum level actually needed based on allocations
            let actualMaxLevel = 0; // Start with root level only
            
            if (this.allocations && this.allocations.length > 0) {
                for (const alloc of this.allocations) {
                    if (alloc.size) {
                        // Find the level needed for this allocation size
                        const levelForAlloc = Math.ceil(Math.log2(totalSize / alloc.size));
                        actualMaxLevel = Math.max(actualMaxLevel, levelForAlloc);
                    }
                }
            }
            
            // Limit max level to reasonable bounds
            const theoreticalMaxLevel = Math.floor(Math.log2(totalSize / minBlockSize));
            actualMaxLevel = Math.min(actualMaxLevel, theoreticalMaxLevel, 6);
            
            console.log('Dynamic tree levels: actual=' + actualMaxLevel + ', theoretical=' + theoreticalMaxLevel);
            
            // Only generate levels that are actually needed
            for (let level = 0; level <= actualMaxLevel; level++) {
                const nodeSize = totalSize / Math.pow(2, level);
                const nodeCount = Math.pow(2, level);
                
                html += `<div class="buddy-level buddy-level-${level}">`;
                
                for (let i = 0; i < nodeCount; i++) {
                    const chunkStart = (totalSize / nodeCount) * i;
                    const chunkEnd = chunkStart + nodeSize;
                    
                    // Check if this chunk is allocated based on real allocations
                    let allocated = false;
                    let allocatedInfo = null;
                    
                    if (this.allocations && this.allocations.length > 0) {
                        for (const alloc of this.allocations) {
                            if (!alloc.address) continue;
                            
                            let allocAddr;
                            if (typeof alloc.address === 'string' && alloc.address.startsWith('0x')) {
                                allocAddr = parseInt(alloc.address, 16);
                            } else {
                                allocAddr = parseInt(alloc.address) || chunkStart; // Use chunkStart as fallback
                            }
                            
                            // Check if allocation overlaps with this buddy block
                            if (allocAddr >= chunkStart && allocAddr < chunkEnd) {
                                // For Buddy System, blocks should fit exactly or be smaller
                                if (alloc.size <= nodeSize) {
                                    allocated = true;
                                    allocatedInfo = alloc;
                                    break;
                                }
                            }
                        }
                    }
                      const status = allocated ? 'allocated' : 'free';
                    const allocId = allocatedInfo ? allocatedInfo.id : null;
                    
                    html += `
                        <div class="buddy-node ${status}" 
                             data-level="${level}" 
                             data-size="${nodeSize}"
                             data-alloc-id="${allocId || ''}"
                             data-address="0x${chunkStart.toString(16).padStart(8, '0')}"
                             title="${allocated ? `Allocated: ${allocatedInfo.size}B (ID: ${allocId})` : `Free: ${this.formatBytes(nodeSize)}`}">
                            <div class="node-header">Level ${level}</div>
                            <div class="node-size">${this.formatBytes(nodeSize)}</div>
                            <div class="node-status ${status}">${status.toUpperCase()}</div>
                            <div class="node-address">0x${chunkStart.toString(16).padStart(8, '0')}</div>
                            ${allocated ? `<div class="node-alloc-info">ID: ${allocId} | ${this.formatBytes(allocatedInfo.size)}</div>` : ''}
                        </div>
                    `;
                }
                html += `</div>`;
            }
        }
        
        // Add legend for the buddy tree
        html += `
            <div class="buddy-legend">
                <h5><i class="fas fa-info-circle"></i> Buddy System Legend</h5>
                <div class="legend-items">
                    <div class="legend-item">
                        <div class="legend-color allocated"></div>
                        <span>Allocated Block</span>
                    </div>
                    <div class="legend-item">
                        <div class="legend-color free"></div>
                        <span>Free Block</span>
                    </div>
                    <div class="legend-item">
                        <div class="legend-info">
                            <i class="fas fa-layer-group"></i>
                            <span>Higher levels = Larger blocks</span>
                        </div>
                    </div>
                </div>
            </div>
        `;          console.log('Generated buddy tree HTML with real allocation data:', html.substring(0, 300) + '...');
        return html;
    }

    calculateCurrentTreeLevels() {
        if (!this.allocations || this.allocations.length === 0) {
            return '0 (Root only)';
        }
        
        const totalSize = this.memorySize || 1024;
        const minBlockSize = 32;
        let maxLevel = 0;
        
        for (const alloc of this.allocations) {
            if (alloc.size) {
                const levelForAlloc = Math.ceil(Math.log2(totalSize / alloc.size));
                maxLevel = Math.max(maxLevel, levelForAlloc);
            }
        }
        
        const theoreticalMaxLevel = Math.floor(Math.log2(totalSize / minBlockSize));
        maxLevel = Math.min(maxLevel, theoreticalMaxLevel, 6);
        
        return `${maxLevel + 1} (0-${maxLevel})`;
    }

    // Slab-specific calculation methods
    calculateTotalSlabCaches() {
        return 5; // Standard cache sizes: 16B, 32B, 64B, 128B, 256B
    }

    calculateActiveSlabObjects() {
        if (!this.allocations || this.allocations.length === 0) {
            return 0;
        }
        
        // Count allocations that would use slab caches (small objects)
        return this.allocations.filter(alloc => alloc.size <= 256).length;
    }

    calculateSlabEfficiency() {
        if (!this.allocations || this.allocations.length === 0) {
            return 85; // Default efficiency when no allocations
        }
        
        // Calculate efficiency based on how well slab sizes match allocations
        const slabSizes = [16, 32, 64, 128, 256];
        let totalWaste = 0;
        let totalAllocated = 0;
        
        this.allocations.forEach(alloc => {
            if (alloc.size <= 256) {
                // Find the smallest slab that can fit this allocation
                const slabSize = slabSizes.find(size => size >= alloc.size) || 256;
                totalWaste += slabSize - alloc.size;
                totalAllocated += slabSize;
            }
        });
        
        if (totalAllocated === 0) return 85;
        
        const efficiency = ((totalAllocated - totalWaste) / totalAllocated) * 100;
        return Math.max(0, Math.min(100, efficiency)).toFixed(0);
    }

    calculateSlabOverhead() {
        if (!this.allocations || this.allocations.length === 0) {
            return 15; // Default overhead
        }
        
        // Calculate memory overhead from slab management structures
        const activeSlabs = Math.ceil(this.calculateActiveSlabObjects() / 64); // Assume 64 objects per slab avg
        const slabHeaderSize = 64; // Bytes per slab header
        const totalSlabMemory = activeSlabs * 4096; // 4KB per slab
        const overhead = (activeSlabs * slabHeaderSize) / totalSlabMemory * 100;
        
        return Math.max(5, Math.min(25, overhead)).toFixed(0);
    }

    // Method to refresh the buddy tree visualization
    refreshBuddyTreeVisualization() {
        console.log('Refreshing buddy tree visualization...');
        
        // Find the buddy tree container
        const buddyTreeContainer = document.querySelector('.buddy-tree-visualization');
        if (buddyTreeContainer) {
            // Get current algorithm to ensure we're updating the right visualization
            const currentAlgorithm = document.querySelector('.tab-content.active')?.getAttribute('data-algorithm');
            
            if (currentAlgorithm === 'buddy-system') {
                console.log('Updating buddy tree for current active algorithm');
                
                // Generate new buddy tree with current allocation data
                const newTreeHTML = this.generateBuddyTree({ 
                    blocks: this.allocations, 
                    totalSize: this.memorySize 
                });
                
                // Update the container with animation
                buddyTreeContainer.style.opacity = '0.7';
                buddyTreeContainer.classList.add('updating');
                
                setTimeout(() => {
                    buddyTreeContainer.innerHTML = newTreeHTML;
                    buddyTreeContainer.style.opacity = '1';
                    buddyTreeContainer.classList.remove('updating');
                    
                    // Update dynamic tree levels display
                    const treeLevelsElement = document.getElementById('currentTreeLevels');
                    if (treeLevelsElement) {
                        treeLevelsElement.textContent = this.calculateCurrentTreeLevels();
                    }
                    
                    console.log('Buddy tree visualization updated successfully');
                }, 150);
            }
        } else {
            console.log('Buddy tree container not found or not visible');
        }
    }

    refreshSlabVisualization() {
        console.log('Refreshing slab visualization...');
        
        const slabContainer = document.querySelector('.slab-container');
        if (!slabContainer) {
            console.warn('Slab container not found');
            return;
        }
        
        // Get current algorithm to ensure we're updating the right visualization
        const currentAlgorithm = document.querySelector('.tab-content.active')?.getAttribute('data-algorithm');
        
        if (currentAlgorithm === 'slab') {
            console.log('Updating slab visualization for current active algorithm');
            
            // Generate new slab caches with current allocation data
            const newSlabHTML = this.generateSlabCaches({ 
                allocations: this.allocations
            });
            
            // Update the slab caches container
            const slabCachesContainer = slabContainer.querySelector('.slab-caches');
            if (slabCachesContainer) {
                slabCachesContainer.style.opacity = '0.7';
                slabCachesContainer.classList.add('updating');
                
                setTimeout(() => {
                    slabCachesContainer.innerHTML = newSlabHTML;
                    slabCachesContainer.style.opacity = '1';
                    slabCachesContainer.classList.remove('updating');
                    
                    // Update statistics
                    const totalCachesEl = document.getElementById('totalSlabCaches');
                    const activeObjectsEl = document.getElementById('activeSlabObjects');
                    const efficiencyEl = document.getElementById('slabEfficiency');
                    const overheadEl = document.getElementById('slabOverhead');
                    
                    if (totalCachesEl) totalCachesEl.textContent = this.calculateTotalSlabCaches();
                    if (activeObjectsEl) activeObjectsEl.textContent = this.calculateActiveSlabObjects();
                    if (efficiencyEl) efficiencyEl.textContent = this.calculateSlabEfficiency() + '%';
                    if (overheadEl) overheadEl.textContent = this.calculateSlabOverhead() + '%';
                    
                    console.log('Slab visualization updated successfully');
                }, 150);
            }
        } else {
            console.log('Slab visualization not active, skipping update');
        }
    }

    generateSlabCaches(data = {}) {
        // Use data from parameter or create default based on real allocations
        let caches = data.caches;
        
        if (!caches) {
            // Generate caches based on actual allocations or defaults
            caches = [
                { objectSize: 16, slabSize: 4096, objectsPerSlab: 256, allocatedSlabs: 2 },
                { objectSize: 32, slabSize: 4096, objectsPerSlab: 128, allocatedSlabs: 1 },
                { objectSize: 64, slabSize: 4096, objectsPerSlab: 64, allocatedSlabs: 3 },
                { objectSize: 128, slabSize: 4096, objectsPerSlab: 32, allocatedSlabs: 1 },
                { objectSize: 256, slabSize: 4096, objectsPerSlab: 16, allocatedSlabs: 1 }
            ];
            
            // Adjust based on actual allocations if available
            if (this.allocations && this.allocations.length > 0) {
                caches.forEach(cache => {
                    const matchingAllocs = this.allocations.filter(alloc => 
                        alloc.size <= cache.objectSize && 
                        alloc.size > (cache.objectSize / 2)
                    );
                    if (matchingAllocs.length > 0) {
                        cache.allocatedSlabs = Math.max(1, Math.ceil(matchingAllocs.length / cache.objectsPerSlab));
                    }
                });
            }
        }
        
        console.log('Generating slab caches with data:', caches);
        
        return caches.map((cache, index) => {
            const totalObjects = cache.objectsPerSlab * cache.allocatedSlabs;
            
            // Calculate real usage based on allocations
            let allocatedObjects = 0;
            if (this.allocations && this.allocations.length > 0) {
                const matchingAllocs = this.allocations.filter(alloc => 
                    alloc.size <= cache.objectSize && 
                    alloc.size > (index > 0 ? caches[index-1].objectSize : 0)
                );
                allocatedObjects = Math.min(matchingAllocs.length, totalObjects);
            } else {
                // Simulate realistic allocation pattern
                allocatedObjects = Math.floor(totalObjects * (0.2 + Math.random() * 0.6));
            }
            
            const usage = totalObjects > 0 ? (allocatedObjects / totalObjects) * 100 : 0;
            const efficiency = allocatedObjects > 0 ? (allocatedObjects * cache.objectSize) / (cache.allocatedSlabs * cache.slabSize) * 100 : 0;
            
            return `
                <div class="slab-cache" style="border: 2px solid #9c27b0; border-radius: 12px; padding: 15px; background: linear-gradient(135deg, #fff 0%, #f8f4ff 100%); box-shadow: 0 4px 8px rgba(156, 39, 176, 0.1); transition: transform 0.3s ease;" onmouseover="this.style.transform='scale(1.02)'" onmouseout="this.style.transform='scale(1)'">
                    <div class="slab-cache-header" style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 15px; padding-bottom: 10px; border-bottom: 2px solid #e1bee7;">
                        <span class="slab-cache-size" style="font-size: 1.2rem; font-weight: bold; color: #9c27b0;">
                            <i class="fas fa-cube"></i> ${cache.objectSize}B Objects
                        </span>
                        <span class="slab-cache-usage" style="font-size: 1rem; color: #666; background: #e1bee7; padding: 4px 8px; border-radius: 12px;">
                            ${allocatedObjects}/${totalObjects}
                        </span>
                    </div>
                    
                    <div class="slab-cache-bar" style="width: 100%; height: 20px; background: #e0e0e0; border-radius: 10px; overflow: hidden; margin-bottom: 15px;">
                        <div class="slab-cache-used" style="width: ${usage}%; height: 100%; background: linear-gradient(90deg, #9c27b0 0%, #ba68c8 100%); transition: width 0.3s ease; position: relative;">
                            <div style="position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); color: white; font-size: 0.8rem; font-weight: bold;">
                                ${usage.toFixed(1)}%
                            </div>
                        </div>
                    </div>
                    
                    <div class="slab-objects" style="display: grid; grid-template-columns: repeat(16, 1fr); gap: 2px; margin-bottom: 15px; padding: 10px; background: #fafafa; border-radius: 8px; border: 1px solid #e1bee7;">
                        ${Array.from({length: Math.min(totalObjects, 64)}, (_, i) => {
                            const isAllocated = i < allocatedObjects;
                            return `<div class="slab-object ${isAllocated ? 'allocated' : ''}" 
                                style="width: 12px; height: 12px; border-radius: 2px; 
                                background: ${isAllocated ? '#9c27b0' : '#e0e0e0'}; 
                                transition: all 0.3s ease;
                                ${isAllocated ? 'box-shadow: 0 2px 4px rgba(156, 39, 176, 0.3);' : ''}
                                " title="${isAllocated ? 'Allocated Object' : 'Free Object'}"></div>`;
                        }).join('')}
                        ${totalObjects > 64 ? `<div style="grid-column: span 4; text-align: center; font-size: 0.8rem; color: #666; align-self: center;">+${totalObjects - 64} more</div>` : ''}
                    </div>
                    
                    <div class="slab-cache-stats" style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px; font-size: 0.9rem;">
                        <div style="padding: 8px; background: #f5f5f5; border-radius: 6px; border-left: 4px solid #9c27b0;">
                            <strong>Slabs:</strong> ${cache.allocatedSlabs}<br/>
                            <strong>Slab Size:</strong> ${this.formatBytes(cache.slabSize)}
                        </div>
                        <div style="padding: 8px; background: #f5f5f5; border-radius: 6px; border-left: 4px solid #4caf50;">
                            <strong>Usage:</strong> ${usage.toFixed(1)}%<br/>
                            <strong>Efficiency:</strong> ${efficiency.toFixed(1)}%
                        </div>
                    </div>
                    
                    <div class="slab-allocation-pattern" style="margin-top: 10px; padding: 8px; background: #f8f4ff; border-radius: 6px; border: 1px solid #e1bee7;">
                        <small style="color: #666;">
                            <i class="fas fa-chart-line"></i> 
                            ${allocatedObjects > 0 ? 
                                `Active pattern: ${Math.ceil(allocatedObjects/cache.allocatedSlabs)} objects/slab avg` : 
                                'No active allocations in this cache'
                            }
                        </small>
                    </div>
                </div>
            `;
        }).join('');
    }    generatePoolChunks(data = {}) {
        // Use data from parameter or fallback to instance data
        const pools = data.pools || [];
        const chunkCount = pools.length > 0 ? pools.length : 8;
        const memSize = data.totalSize || this.memorySize || 2048;
        const chunkSize = memSize / chunkCount;
        const allocations = this.allocations || [];
        
        console.log('Generating pool chunks with data:', { pools, chunkCount, memSize, chunkSize, allocations });
        
        // If we have specific pools data from parameter, use it
        if (pools.length > 0) {
            return pools.map((pool, i) => {
                const usage = (pool.used / pool.total) * 100;
                return `
                    <div class="pool-chunk" title="Pool ${i + 1}: ${pool.used}/${pool.total} blocks (${Math.floor(usage)}% used)">
                        <div class="pool-chunk-header">Pool ${i + 1}<br/>${pool.size}B</div>
                        <div class="pool-chunk-bar">
                            <div class="pool-chunk-used" style="height: ${Math.min(usage, 100)}%"></div>
                        </div>
                        <div class="pool-chunk-info">${Math.floor(usage)}%</div>
                    </div>
                `;
            }).join('');
        }
        
        // Fallback to chunk-based visualization
        
        return Array.from({length: chunkCount}, (_, i) => {
            // Calculate usage based on allocations in this chunk
            const chunkStart = i * chunkSize;
            const chunkEnd = chunkStart + chunkSize;
            
            let allocatedSize = 0;
            let usage = 0;
            
            if (allocations.length > 0) {
                const chunkAllocs = allocations.filter(alloc => {
                    if (!alloc.address) return false;
                    let allocAddr;
                    if (typeof alloc.address === 'string' && alloc.address.startsWith('0x')) {
                        allocAddr = parseInt(alloc.address, 16);
                    } else {
                        allocAddr = parseInt(alloc.address) || 0;
                    }
                    return allocAddr >= chunkStart && allocAddr < chunkEnd;
                });
                
                allocatedSize = chunkAllocs.reduce((sum, alloc) => sum + (alloc.size || 0), 0);
                usage = (allocatedSize / chunkSize) * 100;
            } else {
                // Simulate some usage for demonstration
                usage = Math.random() * 80 + 10; // 10-90% usage
            }
            
            return `
                <div class="pool-chunk" title="Chunk ${i + 1}: ${Math.floor(usage)}% used">
                    <div class="pool-chunk-header">Chunk ${i + 1}</div>
                    <div class="pool-chunk-bar">
                        <div class="pool-chunk-used" style="height: ${Math.min(usage, 100)}%"></div>
                    </div>
                    <div class="pool-chunk-info">${Math.floor(usage)}%</div>
                </div>
            `;
        }).join('');
    }    generateHybridStrategies(data = {}) {
        // Use data from parameter or fallback to default strategies
        const strategies = data.strategies || [
            { name: 'buddy', icon: 'fa-sitemap', active: this.allocatorType === 'buddy' },
            { name: 'slab', icon: 'fa-database', active: this.allocatorType === 'slab' },
            { name: 'pool', icon: 'fa-cubes', active: this.allocatorType === 'pool' }
        ];
        
        console.log('Generating hybrid strategies with data:', strategies);
        
        return strategies.map(strategy => {
            // Use strategy usage from data or simulate usage based on allocation patterns
            let usage = strategy.usage || (20 + Math.random() * 60);
            if (strategy.active) usage = strategy.usage || (70 + Math.random() * 30);
            
            return `
                <div class="hybrid-strategy ${strategy.active ? 'hybrid-strategy-active' : ''}">
                    <div class="hybrid-strategy-icon">
                        <i class="fas ${strategy.icon || 'fa-cog'}"></i>
                    </div>
                    <div class="hybrid-strategy-name">${(strategy.name || 'Unknown').toUpperCase()}</div>
                    <div class="hybrid-strategy-usage">${Math.floor(usage)}%</div>
                    <div class="hybrid-strategy-bar">
                        <div class="hybrid-strategy-fill" style="width: ${usage}%"></div>
                    </div>
                </div>
            `;
        }).join('');
    }

    getAlgorithmTitle() {
        const titles = {
            buddy: 'Buddy System Allocator',
            slab: 'Slab Allocator',
            pool: 'Memory Pool Allocator',
            hybrid: 'Hybrid Allocator'
        };
        return titles[this.allocatorType] || 'Unknown Algorithm';
    }    formatBytes(bytes) {
        if (bytes === 0) return '0 B';
        const k = 1024;
        const sizes = ['B', 'KB', 'MB', 'GB'];
        const i = Math.floor(Math.log(bytes) / Math.log(k));
        return parseFloat((bytes / Math.pow(k, i)).toFixed(1)) + ' ' + sizes[i];
    }

    // Enhanced debug logging for algorithm-specific visualizations
    debugVisualizationDOM() {
        console.log('=== ENHANCED VISUALIZATION DOM DEBUG ===');
        
        // Check if container exists
        const container = document.getElementById('algorithmSpecificViz');
        console.log('🔍 Algorithm container found:', !!container);
        
        if (container) {
            console.log('📐 Container dimensions:', {
                width: container.offsetWidth,
                height: container.offsetHeight,
                display: window.getComputedStyle(container).display,
                visibility: window.getComputedStyle(container).visibility,
                opacity: window.getComputedStyle(container).opacity
            });
            
            console.log('📋 Container innerHTML length:', container.innerHTML.length);
            console.log('🎨 Container styles:', {
                background: window.getComputedStyle(container).backgroundColor,
                border: window.getComputedStyle(container).border,
                padding: window.getComputedStyle(container).padding
            });
            
            // Check for child elements
            const children = container.children;
            console.log('👶 Container children count:', children.length);
            
            for (let i = 0; i < children.length; i++) {
                const child = children[i];
                console.log(`  Child ${i}:`, {
                    tagName: child.tagName,
                    className: child.className,
                    display: window.getComputedStyle(child).display,
                    visible: child.offsetHeight > 0 && child.offsetWidth > 0
                });
            }
        } else {
            console.error('❌ Algorithm-specific visualization container not found!');
            console.log('Available elements with IDs:', 
                Array.from(document.querySelectorAll('[id]')).map(el => el.id)
            );
        }
        
        console.log('=== END VISUALIZATION DOM DEBUG ===');
    }

    // Test all visualizations with enhanced feedback
    testAllVisualizations() {
        console.log('🚀 Starting comprehensive visualization test...');
        
        this.debugVisualizationDOM();
        
        const algorithms = [
            {
                type: 'buddy',
                data: {
                    blocks: [
                        { id: 1, size: 128, allocated: true, level: 3, address: '0x00000000' },
                        { id: 2, size: 128, allocated: false, level: 3, address: '0x00000080' },
                        { id: 3, size: 256, allocated: true, level: 2, address: '0x00000100' },
                        { id: 4, size: 256, allocated: false, level: 2, address: '0x00000200' }
                    ],
                    totalSize: 1024
                }
            },
            {
                type: 'pool',
                data: {
                    pools: [
                        { size: 32, used: 15, total: 32 },
                        { size: 64, used: 8, total: 16 },
                        { size: 128, used: 4, total: 8 },
                        { size: 256, used: 2, total: 4 }
                    ],
                    totalSize: 2048
                }
            },
            {
                type: 'slab',
                data: {
                    caches: [
                        { objectSize: 16, slabSize: 4096, objectsPerSlab: 256, allocatedSlabs: 2 },
                        { objectSize: 32, slabSize: 4096, objectsPerSlab: 128, allocatedSlabs: 1 },
                        { objectSize: 64, slabSize: 4096, objectsPerSlab: 64, allocatedSlabs: 3 },
                        { objectSize: 128, slabSize: 4096, objectsPerSlab: 32, allocatedSlabs: 1 }
                    ],
                    totalSize: 32768
                }
            },
            {
                type: 'hybrid',
                data: {
                    strategies: [
                        { name: 'Buddy System', usage: 45, totalAllocations: 230 },
                        { name: 'Slab Allocator', usage: 35, totalAllocations: 180 },
                        { name: 'Memory Pool', usage: 20, totalAllocations: 90 }
                    ],
                    totalSize: 8192
                }
            }
        ];
        
        algorithms.forEach((algorithm, index) => {
            setTimeout(() => {
                console.log(`🎯 Testing ${algorithm.type.toUpperCase()} visualization...`);
                this.allocatorType = algorithm.type;
                
                try {
                    this.renderAlgorithmSpecificVisualization(algorithm.data);
                    console.log(`✅ ${algorithm.type} visualization rendered successfully`);
                    
                    // Additional verification after rendering
                    setTimeout(() => {
                        const container = document.getElementById('algorithmSpecificViz');
                        if (container && container.innerHTML.length > 50) {
                            console.log(`✅ ${algorithm.type} visualization content verified (${container.innerHTML.length} chars)`);
                        } else {
                            console.warn(`⚠️ ${algorithm.type} visualization may not have rendered properly`);
                        }
                    }, 500);
                    
                } catch (error) {
                    console.error(`❌ Error rendering ${algorithm.type} visualization:`, error);
                }
            }, index * 3000); // 3 second intervals
        });
        
        return 'Comprehensive visualization test initiated. Check console for detailed results.';
    }
}

// Simulated Allocator for demo purposes
class SimulatedAllocator {
    constructor(type, size) {
        this.type = type;
        this.totalSize = size;
        this.allocatedBlocks = new Map();
        this.freeBlocks = [{ start: 0, size: size }];
        this.nextId = 1;
    }

    allocate(size) {
        // Find suitable free block
        for (let i = 0; i < this.freeBlocks.length; i++) {
            const block = this.freeBlocks[i];
            if (block.size >= size) {
                const id = this.nextId++;
                const address = `0x${block.start.toString(16).padStart(8, '0')}`;
                
                // Allocate from this block
                this.allocatedBlocks.set(id, {
                    start: block.start,
                    size: size,
                    address: address
                });

                // Update free block
                if (block.size === size) {
                    this.freeBlocks.splice(i, 1);
                } else {
                    block.start += size;
                    block.size -= size;
                }

                return { success: true, id: id, address: address };
            }
        }

        return { success: false, error: 'Out of memory' };
    }

    deallocate(id) {
        const block = this.allocatedBlocks.get(id);
        if (!block) {
            return { success: false, error: 'Invalid block ID' };
        }

        // Add back to free blocks
        this.freeBlocks.push({ start: block.start, size: block.size });
        this.freeBlocks.sort((a, b) => a.start - b.start);

        // Merge adjacent free blocks
        this.mergeFreeBlocks();

        this.allocatedBlocks.delete(id);
        return { success: true };
    }

    defragment() {
        // Simulate defragmentation
        this.mergeFreeBlocks();
    }

    mergeFreeBlocks() {
        for (let i = 0; i < this.freeBlocks.length - 1; i++) {
            const current = this.freeBlocks[i];
            const next = this.freeBlocks[i + 1];
            
            if (current.start + current.size === next.start) {
                current.size += next.size;
                this.freeBlocks.splice(i + 1, 1);
                i--; // Check this block again
            }
        }
    }

    getFragmentation() {
        if (this.freeBlocks.length <= 1) return 0;
        
        const totalFree = this.freeBlocks.reduce((sum, block) => sum + block.size, 0);
        const largestFree = Math.max(...this.freeBlocks.map(block => block.size));
        
        return totalFree > 0 ? 1 - (largestFree / totalFree) : 0;
    }    getMemoryLayout() {
        const layout = [];
        
        // If no allocations, just show one big free block
        if (this.allocatedBlocks.size === 0) {
            layout.push({ size: this.totalSize, type: 'free' });
            console.log('No allocations - showing single free block:', layout);
            return layout;
        }

        // Create sorted list of allocated blocks
        const allocatedBlocks = Array.from(this.allocatedBlocks.values())
            .sort((a, b) => a.start - b.start);
        
        console.log('Allocated blocks:', allocatedBlocks);
        
        let currentPos = 0;

        // Process each allocated block and add free space between them
        for (const block of allocatedBlocks) {
            // Add free space before this block
            if (currentPos < block.start) {
                layout.push({ 
                    size: block.start - currentPos, 
                    type: 'free' 
                });
            }
            
            // Add the allocated block
            layout.push({ 
                size: block.size, 
                type: 'allocated' 
            });
            
            currentPos = block.start + block.size;
        }

        // Add remaining free space
        if (currentPos < this.totalSize) {
            layout.push({ 
                size: this.totalSize - currentPos, 
                type: 'free' 
            });
        }        console.log('Generated layout:', layout);
        return layout;
    }

    debugVisualizationDOM() {
        console.log('=== DEBUG VISUALIZATION DOM ===');
        const canvas = document.getElementById('memoryCanvas');
        if (!canvas) {
            console.error('Canvas not found!');
            return;
        }
        
        console.log('Canvas children count:', canvas.children.length);
        console.log('Canvas class list:', canvas.classList.toString());
        console.log('Canvas innerHTML length:', canvas.innerHTML.length);
        
        // Check for algorithm-specific containers
        const buddyContainers = canvas.querySelectorAll('.buddy-container');
        const poolContainers = canvas.querySelectorAll('.pool-container');
        const slabContainers = canvas.querySelectorAll('.slab-container');
        const hybridContainers = canvas.querySelectorAll('.hybrid-container');
        const algorithmSpecific = canvas.querySelectorAll('.algorithm-specific-viz');
        
        console.log('Found containers:');
        console.log('- Buddy containers:', buddyContainers.length);
        console.log('- Pool containers:', poolContainers.length);
        console.log('- Slab containers:', slabContainers.length);
        console.log('- Hybrid containers:', hybridContainers.length);
        console.log('- Algorithm specific containers:', algorithmSpecific.length);
        
        // Check visibility of all containers
        algorithmSpecific.forEach((container, i) => {
            const style = window.getComputedStyle(container);
            const rect = container.getBoundingClientRect();
            console.log(`Algorithm container ${i}:`, {
                className: container.className,
                display: style.display,
                visibility: style.visibility,
                opacity: style.opacity,
                height: style.height,
                width: style.width,
                boundingRect: {
                    width: rect.width,
                    height: rect.height,
                    top: rect.top,
                    left: rect.left
                },
                innerHTML: container.innerHTML.substring(0, 200) + '...'
            });
        });
        
        console.log('=== END DEBUG ===');
    }

    // Test method for algorithm visualization
    testAlgorithmVisualization() {
        console.log('=== TESTING ALGORITHM VISUALIZATION ===');
        
        // Generate test data
        const testData = {
            blocks: [
                { size: 512, type: 'allocated' },
                { size: 256, type: 'free' },
                { size: 128, type: 'allocated' },
                { size: 128, type: 'free' }
            ],
            totalSize: this.memorySize
        };
        
        // Test current algorithm
        console.log('Testing current algorithm:', this.allocatorType);
        this.renderAlgorithmSpecificVisualization(testData);
        
        return 'Algorithm visualization test completed. Check console for details.';
    }
}

// Console test function for manual verification
window.testVisualizationsNow = () => {
    console.log('=== MANUAL VISUALIZATION TEST ===');
    
    // Get or create demo instance
    let demoInstance = window.memoryAllocatorDemo;
    if (!demoInstance) {
        demoInstance = new MemoryAllocatorDemo();
        window.memoryAllocatorDemo = demoInstance;
    }
    
    // Test with Buddy System
    const buddyData = {
        blocks: [
            { size: 512, type: 'allocated' },
            { size: 256, type: 'free' },
            { size: 128, type: 'allocated' },
            { size: 128, type: 'free' }
        ],
        totalSize: 1024
    };
    
    console.log('Testing Buddy System visualization...');
    demoInstance.allocatorType = 'buddy';
    demoInstance.allocations = [
        { id: 1, size: 256, address: '0x00001000' },
        { id: 2, size: 128, address: '0x00002000' }
    ];
    demoInstance.renderAlgorithmSpecificVisualization(buddyData);
    
    // Test with Memory Pool after 3 seconds
    setTimeout(() => {
        const poolData = {
            blocks: [
                { size: 128, type: 'allocated' },
                { size: 128, type: 'allocated' },
                { size: 128, type: 'free' },
                { size: 128, type: 'free' },
                { size: 128, type: 'allocated' },
                { size: 128, type: 'free' },
                { size: 128, type: 'free' },
                { size: 128, type: 'free' }
            ],
            totalSize: 1024
        };
        
        console.log('Testing Memory Pool visualization...');
        demoInstance.allocatorType = 'pool';
        demoInstance.allocations = [
            { id: 1, size: 128, address: '0x00000000' },
            { id: 2, size: 128, address: '0x00000080' },
            { id: 3, size: 128, address: '0x00000200' }
        ];
        demoInstance.renderAlgorithmSpecificVisualization(poolData);
    }, 3000);
    
    return 'Test initiated. Check the algorithm-specific visualization sections below the memory layout.';
};

// Global test function for comprehensive testing
window.runComprehensiveVisualizationTest = () => {
    console.log('🔬 COMPREHENSIVE VISUALIZATION TEST STARTING...');
    
    // Get or create demo instance
    let demoInstance = window.memoryAllocatorDemo;
    if (!demoInstance) {
        demoInstance = new MemoryAllocatorDemo();
        window.memoryAllocatorDemo = demoInstance;
    }
    
    return demoInstance.testAllVisualizations();
};

// Quick test for individual algorithms
window.testAlgorithm = (type) => {
    let demoInstance = window.memoryAllocatorDemo;
    if (!demoInstance) {
        demoInstance = new MemoryAllocatorDemo();
        window.memoryAllocatorDemo = demoInstance;
    }
    
    demoInstance.allocatorType = type;
    
    const testData = {
        buddy: {
            blocks: [
                { id: 1, size: 128, allocated: true, level: 3, address: '0x000000000' },
                { id: 2, size: 128, allocated: false, level: 3, address: '0x00000080' }
            ],
            totalSize: 1024
        },
        pool: {
            pools: [
                { size: 32, used: 15, total: 32 },
                { size: 64, used: 8, total: 16 }
            ],
            totalSize: 2048
        },
        slab: {
            caches: [
                { objectSize: 16, slabSize: 4096, objectsPerSlab: 256, allocatedSlabs: 2 },
                { objectSize: 32, slabSize: 4096, objectsPerSlab: 128, allocatedSlabs: 1 }
            ],
            totalSize: 32768
        },
        hybrid: {
            strategies: [
                { name: 'Buddy System', usage: 45, totalAllocations: 230 },
                { name: 'Slab Allocator', usage: 35, totalAllocations: 180 },
                { name: 'Memory Pool', usage: 20, totalAllocations: 90 }
            ],
            totalSize: 8192
        }
    };
    
    if (testData[type]) {
        console.log(`Testing ${type} algorithm...`);
        demoInstance.renderAlgorithmSpecificVisualization(testData[type]);
        return `${type} visualization test completed.`;
    } else {
        return `Unknown algorithm type: ${type}. Available: buddy, pool, slab, hybrid`;
    }
};

// Initialize the demo when page loads
document.addEventListener('DOMContentLoaded', () => {
    const demo = new MemoryAllocatorDemo();
    window.memoryAllocatorDemo = demo; // Store reference for testing
    
    // Add event listeners for test buttons
    const testVisualizationBtn = document.getElementById('testVisualizationBtn');
    const testAlgorithmVisualizationBtn = document.getElementById('testAlgorithmVisualizationBtn');
    
    if (testVisualizationBtn) {
        testVisualizationBtn.addEventListener('click', () => {
            console.log('Test Visualization button clicked!');
            if (window.testVisualizationsNow) {
                const result = window.testVisualizationsNow();
                console.log('Test result:', result);
            } else {
                console.error('testVisualizationsNow function not found!');
            }
        });
    }
    
    if (testAlgorithmVisualizationBtn) {
        testAlgorithmVisualizationBtn.addEventListener('click', () => {
            console.log('Test Algorithm Visualization button clicked!');
            // Test all algorithm types
            ['buddy', 'pool', 'slab', 'hybrid'].forEach((algorithmType, index) => {
                setTimeout(() => {
                    console.log(`Testing ${algorithmType} algorithm visualization...`);
                    demo.allocatorType = algorithmType;
                    
                    // Mock some data based on algorithm type
                    let mockData = {};
                    switch (algorithmType) {
                        case 'buddy':
                            mockData = {
                                blocks: [
                                    { id: 1, size: 128, allocated: true, level: 3, address: '0x00000000' },
                                    { id: 2, size: 128, allocated: false, level: 3, address: '0x00000080' },
                                    { id: 3, size: 256, allocated: true, level: 2, address: '0x00000100' }
                                ],
                                totalSize: 1024
                            };
                            break;
                        case 'pool':
                            mockData = {
                                pools: [
                                    { size: 32, used: 15, total: 32 },
                                    { size: 64, used: 8, total: 16 }
                                ],
                                totalSize: 2048
                            };
                            break;
                        case 'slab':
                            mockData = {
                                caches: [
                                    { objectSize: 16, slabSize: 4096, objectsPerSlab: 256, allocatedSlabs: 2 },
                                    { objectSize: 32, slabSize: 4096, objectsPerSlab: 128, allocatedSlabs: 1 }
                                ],
                                totalSize: 32768
                            };
                            break;
                        case 'hybrid':
                            mockData = {
                                strategies: [
                                    { name: 'Buddy System', usage: 45, totalAllocations: 230 },
                                    { name: 'Slab Allocator', usage: 35, totalAllocations: 180 },
                                    { name: 'Memory Pool', usage: 20, totalAllocations: 90 }
                                ],
                                totalSize: 8192
                            };
                            break;
                    }
                    
                    demo.renderAlgorithmSpecificVisualization(mockData);
                }, index * 2000); // Stagger the tests
            });
        });
    }
});
