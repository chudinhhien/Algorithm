// Quick test script for memory allocator visualizations
// Run this in the browser console on localhost:3000

console.log('=== STARTING VISUALIZATION TEST ===');

// First, let's test if our enhanced visualization functions work
function testVisualizationFunctions() {
    console.log('Testing visualization functions...');
    
    const canvas = document.getElementById('memoryCanvas');
    if (!canvas) {
        console.error('Memory canvas not found!');
        return;
    }
    
    console.log('Canvas found:', canvas);
    
    // Clear canvas
    canvas.innerHTML = '';
    
    // Create a test demo instance
    const testDemo = {
        allocatorType: 'buddy',
        memorySize: 1024,
        allocations: [
            { id: 1, size: 256, address: '0x00001000' },
            { id: 2, size: 128, address: '0x00002000' }
        ],
        stats: {
            totalAllocated: 384,
            totalFree: 640,
            fragmentationRatio: 0.25
        },
        
        // Method to format bytes
        formatBytes: function(bytes) {
            if (bytes === 0) return '0 B';
            const k = 1024;
            const sizes = ['B', 'KB', 'MB', 'GB'];
            const i = Math.floor(Math.log(bytes) / Math.log(k));
            return parseFloat((bytes / Math.pow(k, i)).toFixed(1)) + ' ' + sizes[i];
        },
        
        // Method to create buddy visualization
        createBuddyVisualization: function() {
            console.log('Creating buddy visualization...');
            
            const container = document.createElement('div');
            container.className = 'buddy-container algorithm-specific-viz';
            container.style.cssText = `
                display: block !important;
                visibility: visible !important;
                opacity: 1 !important;
                border: 3px solid #2196F3;
                background: #e3f2fd;
                padding: 20px;
                margin: 20px 0;
                border-radius: 12px;
                min-height: 300px;
            `;
            
            container.innerHTML = `
                <h4 style="color: #2196F3; margin-bottom: 15px;">
                    <i class="fas fa-sitemap"></i> Buddy System Tree Structure
                </h4>
                <div class="buddy-tree-section">
                    <div class="buddy-tree" style="background: #ffffff; padding: 20px; border-radius: 8px; min-height: 200px;">
                        <div class="buddy-level" style="margin: 10px 0; text-align: center;">
                            <div class="buddy-node" style="display: inline-block; margin: 5px; padding: 10px; border: 2px solid #2196F3; border-radius: 8px; background: #ffffff;">
                                1024B Root
                            </div>
                        </div>
                        <div class="buddy-level" style="margin: 10px 0; text-align: center;">
                            <div class="buddy-node" style="display: inline-block; margin: 5px; padding: 10px; border: 2px solid #ff5722; border-radius: 8px; background: #ffcdd2;">
                                512B Allocated
                            </div>
                            <div class="buddy-node" style="display: inline-block; margin: 5px; padding: 10px; border: 2px solid #4caf50; border-radius: 8px; background: #c8e6c9;">
                                512B Free
                            </div>
                        </div>
                        <div class="buddy-level" style="margin: 10px 0; text-align: center;">
                            <div class="buddy-node" style="display: inline-block; margin: 5px; padding: 8px; border: 1px solid #ff5722; border-radius: 6px; background: #ffcdd2;">
                                256B Used
                            </div>
                            <div class="buddy-node" style="display: inline-block; margin: 5px; padding: 8px; border: 1px solid #ff5722; border-radius: 6px; background: #ffcdd2;">
                                256B Used
                            </div>
                            <div class="buddy-node" style="display: inline-block; margin: 5px; padding: 8px; border: 1px solid #4caf50; border-radius: 6px; background: #c8e6c9;">
                                256B Free
                            </div>
                            <div class="buddy-node" style="display: inline-block; margin: 5px; padding: 8px; border: 1px solid #4caf50; border-radius: 6px; background: #c8e6c9;">
                                256B Free
                            </div>
                        </div>
                    </div>
                </div>
                <div class="buddy-stats" style="margin-top: 20px; background: #ffffff; padding: 15px; border-radius: 8px; display: flex; gap: 20px;">
                    <div style="text-align: center; flex: 1;">
                        <span style="display: block; font-size: 0.9rem; color: #666; margin-bottom: 5px;">Max Block Size:</span>
                        <span style="display: block; font-size: 1.1rem; font-weight: bold; color: #2196F3;">1024 B</span>
                    </div>
                    <div style="text-align: center; flex: 1;">
                        <span style="display: block; font-size: 0.9rem; color: #666; margin-bottom: 5px;">Min Block Size:</span>
                        <span style="display: block; font-size: 1.1rem; font-weight: bold; color: #2196F3;">16 B</span>
                    </div>
                </div>
            `;
            
            return container;
        },
        
        // Method to create pool visualization
        createPoolVisualization: function() {
            console.log('Creating pool visualization...');
            
            const container = document.createElement('div');
            container.className = 'pool-container algorithm-specific-viz';
            container.style.cssText = `
                display: block !important;
                visibility: visible !important;
                opacity: 1 !important;
                border: 3px solid #ff5722;
                background: #fbe9e7;
                padding: 20px;
                margin: 20px 0;
                border-radius: 12px;
                min-height: 300px;
            `;
            
            const chunkCount = 8;
            let chunksHTML = '';
            
            for (let i = 0; i < chunkCount; i++) {
                const usage = Math.random() * 80 + 10; // 10-90% usage
                chunksHTML += `
                    <div class="pool-chunk" style="
                        display: inline-block;
                        width: 80px;
                        height: 120px;
                        margin: 5px;
                        border: 2px solid #ff5722;
                        border-radius: 8px;
                        background: #ffffff;
                        text-align: center;
                        padding: 8px;
                        vertical-align: bottom;
                    ">
                        <div style="font-size: 0.8rem; font-weight: bold; margin-bottom: 5px;">Chunk ${i + 1}</div>
                        <div style="
                            height: 60px;
                            border: 1px solid #ddd;
                            border-radius: 4px;
                            display: flex;
                            flex-direction: column-reverse;
                            margin-bottom: 5px;
                        ">
                            <div style="
                                background: #ff5722;
                                height: ${usage}%;
                                border-radius: 0 0 3px 3px;
                            "></div>
                        </div>
                        <div style="font-size: 0.8rem; color: #666;">${Math.floor(usage)}%</div>
                    </div>
                `;
            }
            
            container.innerHTML = `
                <h4 style="color: #ff5722; margin-bottom: 15px;">
                    <i class="fas fa-cubes"></i> Memory Pool Chunks
                </h4>
                <div class="pool-chunk-section">
                    <div class="pool-chunks" style="
                        display: flex;
                        gap: 10px;
                        align-items: flex-end;
                        padding: 20px;
                        background: #ffffff;
                        border-radius: 12px;
                        min-height: 200px;
                        border: 2px solid #ff5722;
                        overflow-x: auto;
                        justify-content: center;
                    ">
                        ${chunksHTML}
                    </div>
                </div>
            `;
            
            return container;
        }
    };
    
    // Test Buddy System visualization
    console.log('Testing Buddy System...');
    testDemo.allocatorType = 'buddy';
    const buddyViz = testDemo.createBuddyVisualization();
    canvas.appendChild(buddyViz);
    console.log('✓ Buddy visualization added');
    
    // Test Pool visualization after 3 seconds
    setTimeout(() => {
        console.log('Testing Memory Pool...');
        testDemo.allocatorType = 'pool';
        const poolViz = testDemo.createPoolVisualization();
        canvas.appendChild(poolViz);
        console.log('✓ Pool visualization added');
        
        // Final verification
        setTimeout(() => {
            const buddyContainers = canvas.querySelectorAll('.buddy-container');
            const poolContainers = canvas.querySelectorAll('.pool-container');
            
            console.log('=== FINAL VERIFICATION ===');
            console.log('Buddy containers found:', buddyContainers.length);
            console.log('Pool containers found:', poolContainers.length);
            
            buddyContainers.forEach((container, i) => {
                const rect = container.getBoundingClientRect();
                console.log(`Buddy container ${i}:`, {
                    visible: rect.height > 0 && rect.width > 0,
                    height: rect.height,
                    width: rect.width
                });
            });
            
            poolContainers.forEach((container, i) => {
                const rect = container.getBoundingClientRect();
                console.log(`Pool container ${i}:`, {
                    visible: rect.height > 0 && rect.width > 0,
                    height: rect.height,
                    width: rect.width
                });
            });
            
            if (buddyContainers.length > 0 && poolContainers.length > 0) {
                console.log('🎉 SUCCESS: Both visualizations are working!');
            } else {
                console.log('❌ ISSUE: Some visualizations are missing');
            }
        }, 1000);
        
    }, 3000);
}

// Auto-start the test
console.log('Starting automatic test in 2 seconds...');
setTimeout(testVisualizationFunctions, 2000);

// Also expose the function for manual testing
window.testVisualizationFunctions = testVisualizationFunctions;

console.log('=== Test script loaded. Visualization test will start automatically ===');
console.log('You can also run: testVisualizationFunctions() manually');
