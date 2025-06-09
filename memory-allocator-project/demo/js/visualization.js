/**
 * Advanced Memory Layout Visualization
 * Enhanced graphics for memory allocator demo
 */

class MemoryVisualizer {
    constructor(canvasId, options = {}) {
        this.canvas = document.getElementById(canvasId);
        this.ctx = this.canvas.getContext('2d');
        this.options = {
            blockHeight: 20,
            blockSpacing: 2,
            animationDuration: 300,
            colors: {
                free: '#e8f5e8',
                allocated: '#ffcccb',
                buddy: '#add8e6',
                slab: '#f0e68c',
                pool: '#dda0dd',
                header: '#d3d3d3',
                fragmented: '#ff6b6b'
            },
            ...options
        };
        this.memoryBlocks = [];
        this.animationQueue = [];
        this.isAnimating = false;
        
        this.setupCanvas();
        this.bindEvents();
    }
    
    setupCanvas() {
        // Set canvas size
        this.canvas.width = this.canvas.offsetWidth;
        this.canvas.height = 400;
        
        // Setup high DPI support
        const dpr = window.devicePixelRatio || 1;
        const rect = this.canvas.getBoundingClientRect();
        this.canvas.width = rect.width * dpr;
        this.canvas.height = rect.height * dpr;
        this.ctx.scale(dpr, dpr);
        this.canvas.style.width = rect.width + 'px';
        this.canvas.style.height = rect.height + 'px';
    }
    
    bindEvents() {
        // Tooltip functionality
        this.canvas.addEventListener('mousemove', (e) => {
            this.handleMouseMove(e);
        });
        
        this.canvas.addEventListener('mouseleave', () => {
            this.hideTooltip();
        });
        
        // Resize handling
        window.addEventListener('resize', () => {
            this.setupCanvas();
            this.render();
        });
    }
    
    updateMemoryLayout(blocks, allocatorType = 'buddy') {
        // Convert memory blocks to visual representation
        this.memoryBlocks = blocks.map((block, index) => ({
            ...block,
            id: index,
            x: 0,
            y: 0,
            width: 0,
            height: this.options.blockHeight,
            color: this.getBlockColor(block, allocatorType),
            alpha: 1.0
        }));
        
        this.calculateLayout();
        this.render();
    }
    
    calculateLayout() {
        const canvasWidth = this.canvas.offsetWidth;
        const totalMemory = this.memoryBlocks.reduce((sum, block) => sum + block.size, 0);
        
        let currentX = 10;
        let currentY = 10;
        const maxRowWidth = canvasWidth - 20;
        
        for (let block of this.memoryBlocks) {
            // Calculate block width based on size
            block.width = Math.max(5, (block.size / totalMemory) * (maxRowWidth * 0.8));
            
            // Check if we need to wrap to next row
            if (currentX + block.width > maxRowWidth) {
                currentX = 10;
                currentY += this.options.blockHeight + this.options.blockSpacing;
            }
            
            block.x = currentX;
            block.y = currentY;
            
            currentX += block.width + this.options.blockSpacing;
        }
    }
    
    getBlockColor(block, allocatorType) {
        if (block.is_free) {
            return this.options.colors.free;
        }
        
        // Color based on allocator type
        switch (allocatorType) {
            case 'buddy':
                return this.options.colors.buddy;
            case 'slab':
                return this.options.colors.slab;
            case 'pool':
                return this.options.colors.pool;
            case 'hybrid':
                // For hybrid, use type-specific colors
                if (block.type && block.type.includes('Buddy')) {
                    return this.options.colors.buddy;
                } else if (block.type && block.type.includes('Slab')) {
                    return this.options.colors.slab;
                } else if (block.type && block.type.includes('Pool')) {
                    return this.options.colors.pool;
                }
                return this.options.colors.allocated;
            default:
                return this.options.colors.allocated;
        }
    }
    
    render() {
        const ctx = this.ctx;
        const canvasWidth = this.canvas.offsetWidth;
        const canvasHeight = this.canvas.offsetHeight;
        
        // Clear canvas
        ctx.clearRect(0, 0, canvasWidth, canvasHeight);
        
        // Draw background
        ctx.fillStyle = '#f8f9fa';
        ctx.fillRect(0, 0, canvasWidth, canvasHeight);
        
        // Draw memory blocks
        for (let block of this.memoryBlocks) {
            this.drawBlock(block);
        }
        
        // Draw legend
        this.drawLegend();
        
        // Draw statistics
        this.drawStatistics();
    }
    
    drawBlock(block) {
        const ctx = this.ctx;
        
        // Set alpha for animations
        ctx.globalAlpha = block.alpha;
        
        // Draw block
        ctx.fillStyle = block.color;
        ctx.fillRect(block.x, block.y, block.width, block.height);
        
        // Draw border
        ctx.strokeStyle = '#333';
        ctx.lineWidth = 1;
        ctx.strokeRect(block.x, block.y, block.width, block.height);
        
        // Draw label if block is large enough
        if (block.width > 40) {
            ctx.fillStyle = '#333';
            ctx.font = '10px Arial';
            ctx.textAlign = 'center';
            
            const text = block.size < 1024 ? 
                `${block.size}B` : 
                `${(block.size / 1024).toFixed(1)}KB`;
                
            ctx.fillText(text, block.x + block.width / 2, block.y + block.height / 2 + 3);
        }
        
        ctx.globalAlpha = 1.0;
    }
    
    drawLegend() {
        const ctx = this.ctx;
        const legendX = 10;
        const legendY = this.canvas.offsetHeight - 80;
        
        const legendItems = [
            { color: this.options.colors.free, label: 'Free' },
            { color: this.options.colors.buddy, label: 'Buddy' },
            { color: this.options.colors.slab, label: 'Slab' },
            { color: this.options.colors.pool, label: 'Pool' },
            { color: this.options.colors.allocated, label: 'Allocated' }
        ];
        
        ctx.font = '12px Arial';
        ctx.textAlign = 'left';
        
        legendItems.forEach((item, index) => {
            const x = legendX + (index * 80);
            const y = legendY;
            
            // Draw color box
            ctx.fillStyle = item.color;
            ctx.fillRect(x, y, 15, 15);
            ctx.strokeStyle = '#333';
            ctx.strokeRect(x, y, 15, 15);
            
            // Draw label
            ctx.fillStyle = '#333';
            ctx.fillText(item.label, x + 20, y + 12);
        });
    }
    
    drawStatistics() {
        const ctx = this.ctx;
        const statsX = this.canvas.offsetWidth - 200;
        const statsY = 20;
        
        // Calculate statistics
        const totalBlocks = this.memoryBlocks.length;
        const freeBlocks = this.memoryBlocks.filter(b => b.is_free).length;
        const allocatedBlocks = totalBlocks - freeBlocks;
        const totalSize = this.memoryBlocks.reduce((sum, b) => sum + b.size, 0);
        const freeSize = this.memoryBlocks.filter(b => b.is_free).reduce((sum, b) => sum + b.size, 0);
        
        const stats = [
            `Total Blocks: ${totalBlocks}`,
            `Allocated: ${allocatedBlocks}`,
            `Free: ${freeBlocks}`,
            `Memory Used: ${this.formatSize(totalSize - freeSize)}`,
            `Memory Free: ${this.formatSize(freeSize)}`
        ];
        
        // Draw statistics background
        ctx.fillStyle = 'rgba(255, 255, 255, 0.9)';
        ctx.fillRect(statsX - 10, statsY - 10, 190, stats.length * 20 + 20);
        ctx.strokeStyle = '#ddd';
        ctx.strokeRect(statsX - 10, statsY - 10, 190, stats.length * 20 + 20);
        
        // Draw statistics text
        ctx.fillStyle = '#333';
        ctx.font = '12px Arial';
        ctx.textAlign = 'left';
        
        stats.forEach((stat, index) => {
            ctx.fillText(stat, statsX, statsY + (index * 18));
        });
    }
    
    formatSize(bytes) {
        if (bytes < 1024) return `${bytes}B`;
        if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)}KB`;
        return `${(bytes / (1024 * 1024)).toFixed(1)}MB`;
    }
    
    // Animation methods
    animateAllocation(blockIndex, callback) {
        if (blockIndex >= 0 && blockIndex < this.memoryBlocks.length) {
            const block = this.memoryBlocks[blockIndex];
            this.animateBlockHighlight(block, '#90EE90', callback);
        }
    }
    
    animateDeallocation(blockIndex, callback) {
        if (blockIndex >= 0 && blockIndex < this.memoryBlocks.length) {
            const block = this.memoryBlocks[blockIndex];
            this.animateBlockHighlight(block, '#FFB6C1', callback);
        }
    }
    
    animateBlockHighlight(block, highlightColor, callback) {
        const originalColor = block.color;
        const startTime = Date.now();
        
        const animate = () => {
            const elapsed = Date.now() - startTime;
            const progress = Math.min(elapsed / this.options.animationDuration, 1);
            
            if (progress < 0.5) {
                // Fade to highlight color
                block.color = this.interpolateColor(originalColor, highlightColor, progress * 2);
            } else {
                // Fade back to original color
                block.color = this.interpolateColor(highlightColor, originalColor, (progress - 0.5) * 2);
            }
            
            this.render();
            
            if (progress < 1) {
                requestAnimationFrame(animate);
            } else {
                block.color = originalColor;
                this.render();
                if (callback) callback();
            }
        };
        
        requestAnimationFrame(animate);
    }
    
    interpolateColor(color1, color2, factor) {
        // Simple color interpolation (assumes hex colors)
        const c1 = this.hexToRgb(color1);
        const c2 = this.hexToRgb(color2);
        
        if (!c1 || !c2) return color1;
        
        const r = Math.round(c1.r + factor * (c2.r - c1.r));
        const g = Math.round(c1.g + factor * (c2.g - c1.g));
        const b = Math.round(c1.b + factor * (c2.b - c1.b));
        
        return `rgb(${r}, ${g}, ${b})`;
    }
    
    hexToRgb(hex) {
        const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? {
            r: parseInt(result[1], 16),
            g: parseInt(result[2], 16),
            b: parseInt(result[3], 16)
        } : null;
    }
    
    // Mouse interaction
    handleMouseMove(e) {
        const rect = this.canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        
        // Find block under mouse
        const hoveredBlock = this.memoryBlocks.find(block => 
            x >= block.x && x <= block.x + block.width &&
            y >= block.y && y <= block.y + block.height
        );
        
        if (hoveredBlock) {
            this.showTooltip(hoveredBlock, e.clientX, e.clientY);
        } else {
            this.hideTooltip();
        }
    }
    
    showTooltip(block, x, y) {
        // Remove existing tooltip
        this.hideTooltip();
        
        // Create tooltip element
        const tooltip = document.createElement('div');
        tooltip.id = 'memory-tooltip';
        tooltip.style.cssText = `
            position: fixed;
            left: ${x + 10}px;
            top: ${y - 30}px;
            background: rgba(0, 0, 0, 0.8);
            color: white;
            padding: 8px 12px;
            border-radius: 4px;
            font-size: 12px;
            pointer-events: none;
            z-index: 1000;
            white-space: nowrap;
        `;
        
        const content = `
            <div>Address: 0x${block.address.toString(16).toUpperCase()}</div>
            <div>Size: ${this.formatSize(block.size)}</div>
            <div>Status: ${block.is_free ? 'Free' : 'Allocated'}</div>
            ${block.type ? `<div>Type: ${block.type}</div>` : ''}
        `;
        
        tooltip.innerHTML = content;
        document.body.appendChild(tooltip);
    }
    
    hideTooltip() {
        const tooltip = document.getElementById('memory-tooltip');
        if (tooltip) {
            tooltip.remove();
        }
    }
    
    // Utility methods
    clear() {
        this.memoryBlocks = [];
        this.render();
    }
    
    resize() {
        this.setupCanvas();
        this.calculateLayout();
        this.render();
    }
    
    exportAsImage() {
        // Create a new canvas with white background
        const exportCanvas = document.createElement('canvas');
        exportCanvas.width = this.canvas.width;
        exportCanvas.height = this.canvas.height;
        const exportCtx = exportCanvas.getContext('2d');
        
        // Fill with white background
        exportCtx.fillStyle = 'white';
        exportCtx.fillRect(0, 0, exportCanvas.width, exportCanvas.height);
        
        // Draw the current canvas content
        exportCtx.drawImage(this.canvas, 0, 0);
        
        // Create download link
        const link = document.createElement('a');
        link.download = 'memory-layout.png';
        link.href = exportCanvas.toDataURL();
        link.click();
    }
}

// Performance Chart Visualization
class PerformanceChart {
    constructor(canvasId, options = {}) {
        this.canvas = document.getElementById(canvasId);
        this.ctx = this.canvas.getContext('2d');
        this.options = {
            padding: 40,
            gridColor: '#e0e0e0',
            axisColor: '#333',
            colors: ['#ff6b6b', '#4ecdc4', '#45b7d1', '#96ceb4', '#feca57'],
            ...options
        };
        this.data = [];
        
        this.setupCanvas();
    }
    
    setupCanvas() {
        this.canvas.width = this.canvas.offsetWidth;
        this.canvas.height = 300;
        
        const dpr = window.devicePixelRatio || 1;
        const rect = this.canvas.getBoundingClientRect();
        this.canvas.width = rect.width * dpr;
        this.canvas.height = rect.height * dpr;
        this.ctx.scale(dpr, dpr);
        this.canvas.style.width = rect.width + 'px';
        this.canvas.style.height = rect.height + 'px';
    }
    
    updateData(benchmarkResults) {
        this.data = benchmarkResults;
        this.render();
    }
    
    render() {
        const ctx = this.ctx;
        const width = this.canvas.offsetWidth;
        const height = this.canvas.offsetHeight;
        
        // Clear canvas
        ctx.clearRect(0, 0, width, height);
        
        if (this.data.length === 0) {
            this.drawNoData();
            return;
        }
        
        // Calculate chart area
        const chartArea = {
            x: this.options.padding,
            y: this.options.padding,
            width: width - this.options.padding * 2,
            height: height - this.options.padding * 2
        };
        
        // Draw chart background
        ctx.fillStyle = '#f8f9fa';
        ctx.fillRect(chartArea.x, chartArea.y, chartArea.width, chartArea.height);
        
        // Draw grid
        this.drawGrid(chartArea);
        
        // Draw bars
        this.drawBars(chartArea);
        
        // Draw axes
        this.drawAxes(chartArea);
        
        // Draw legend
        this.drawChartLegend(chartArea);
    }
    
    drawGrid(chartArea) {
        const ctx = this.ctx;
        ctx.strokeStyle = this.options.gridColor;
        ctx.lineWidth = 1;
        
        // Vertical grid lines
        const numVerticalLines = this.data.length + 1;
        for (let i = 0; i <= numVerticalLines; i++) {
            const x = chartArea.x + (i * chartArea.width) / numVerticalLines;
            ctx.beginPath();
            ctx.moveTo(x, chartArea.y);
            ctx.lineTo(x, chartArea.y + chartArea.height);
            ctx.stroke();
        }
        
        // Horizontal grid lines
        const numHorizontalLines = 5;
        for (let i = 0; i <= numHorizontalLines; i++) {
            const y = chartArea.y + (i * chartArea.height) / numHorizontalLines;
            ctx.beginPath();
            ctx.moveTo(chartArea.x, y);
            ctx.lineTo(chartArea.x + chartArea.width, y);
            ctx.stroke();
        }
    }
    
    drawBars(chartArea) {
        const ctx = this.ctx;
        const barWidth = chartArea.width / this.data.length * 0.8;
        const maxValue = Math.max(...this.data.map(d => d.totalTime));
        
        this.data.forEach((item, index) => {
            const barHeight = (item.totalTime / maxValue) * chartArea.height * 0.8;
            const x = chartArea.x + (index * chartArea.width) / this.data.length + 
                     (chartArea.width / this.data.length - barWidth) / 2;
            const y = chartArea.y + chartArea.height - barHeight;
            
            // Draw bar
            ctx.fillStyle = this.options.colors[index % this.options.colors.length];
            ctx.fillRect(x, y, barWidth, barHeight);
            
            // Draw bar border
            ctx.strokeStyle = '#333';
            ctx.lineWidth = 1;
            ctx.strokeRect(x, y, barWidth, barHeight);
            
            // Draw value label
            ctx.fillStyle = '#333';
            ctx.font = '10px Arial';
            ctx.textAlign = 'center';
            ctx.fillText(
                `${item.totalTime.toFixed(1)}ms`,
                x + barWidth / 2,
                y - 5
            );
            
            // Draw allocator name
            ctx.fillText(
                item.allocatorName,
                x + barWidth / 2,
                chartArea.y + chartArea.height + 15
            );
        });
    }
    
    drawAxes(chartArea) {
        const ctx = this.ctx;
        ctx.strokeStyle = this.options.axisColor;
        ctx.lineWidth = 2;
        
        // X-axis
        ctx.beginPath();
        ctx.moveTo(chartArea.x, chartArea.y + chartArea.height);
        ctx.lineTo(chartArea.x + chartArea.width, chartArea.y + chartArea.height);
        ctx.stroke();
        
        // Y-axis
        ctx.beginPath();
        ctx.moveTo(chartArea.x, chartArea.y);
        ctx.lineTo(chartArea.x, chartArea.y + chartArea.height);
        ctx.stroke();
        
        // Y-axis labels
        ctx.fillStyle = this.options.axisColor;
        ctx.font = '10px Arial';
        ctx.textAlign = 'right';
        
        const maxValue = Math.max(...this.data.map(d => d.totalTime));
        for (let i = 0; i <= 5; i++) {
            const value = (maxValue / 5) * i;
            const y = chartArea.y + chartArea.height - (i * chartArea.height) / 5;
            ctx.fillText(
                `${value.toFixed(1)}ms`,
                chartArea.x - 5,
                y + 3
            );
        }
    }
    
    drawChartLegend(chartArea) {
        // This would show additional information about the chart
        const ctx = this.ctx;
        ctx.fillStyle = '#333';
        ctx.font = '12px Arial';
        ctx.textAlign = 'center';
        ctx.fillText(
            'Allocation Performance Comparison',
            chartArea.x + chartArea.width / 2,
            20
        );
    }
    
    drawNoData() {
        const ctx = this.ctx;
        ctx.fillStyle = '#999';
        ctx.font = '16px Arial';
        ctx.textAlign = 'center';
        ctx.fillText(
            'No performance data available',
            this.canvas.offsetWidth / 2,
            this.canvas.offsetHeight / 2
        );
    }
}

// Export for use in other files
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { MemoryVisualizer, PerformanceChart };
}
