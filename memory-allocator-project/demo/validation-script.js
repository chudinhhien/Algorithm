// Comprehensive validation script for Memory Allocator Demo
// Run this in the browser console to validate all functionality

console.log('🔍 Starting Memory Allocator Demo Validation...');

// 1. Check if required containers exist
function validateContainers() {
    console.log('\n📦 Validating HTML containers...');
    
    const memoryViz = document.getElementById('memoryViz');
    const algorithmViz = document.getElementById('algorithmSpecificViz');
    
    if (!memoryViz) {
        console.error('❌ Missing memoryViz container');
        return false;
    } else {
        console.log('✅ memoryViz container found');
    }
    
    if (!algorithmViz) {
        console.error('❌ Missing algorithmSpecificViz container');
        return false;
    } else {
        console.log('✅ algorithmSpecificViz container found');
    }
    
    return true;
}

// 2. Check if MemoryAllocatorDemo class is available
function validateClass() {
    console.log('\n🏗️ Validating MemoryAllocatorDemo class...');
    
    if (typeof MemoryAllocatorDemo === 'undefined') {
        console.error('❌ MemoryAllocatorDemo class not found');
        return false;
    }
    
    console.log('✅ MemoryAllocatorDemo class available');
    
    // Check if we can create an instance
    try {
        const demo = new MemoryAllocatorDemo();
        console.log('✅ MemoryAllocatorDemo instance created successfully');
        return demo;
    } catch (error) {
        console.error('❌ Failed to create MemoryAllocatorDemo instance:', error);
        return false;
    }
}

// 3. Test algorithm-specific visualization methods
function validateMethods(demo) {
    console.log('\n⚙️ Validating visualization methods...');
    
    const methods = [
        'generateBuddyTree',
        'generatePoolChunks', 
        'generateSlabCaches',
        'generateHybridStrategies',
        'renderAlgorithmSpecificVisualization'
    ];
    
    for (const method of methods) {
        if (typeof demo[method] === 'function') {
            console.log(`✅ ${method} method available`);
        } else {
            console.error(`❌ ${method} method missing`);
            return false;
        }
    }
    
    return true;
}

// 4. Test each algorithm visualization
function testAlgorithmVisualizations(demo) {
    console.log('\n🎨 Testing algorithm visualizations...');
    
    // Test data for each algorithm
    const testData = {
        buddy: {
            algorithm: 'buddy',
            totalSize: 1024,
            allocations: [{ id: 1, size: 64, address: 0 }],
            buddyTree: { size: 1024, level: 0, allocated: false, left: null, right: null }
        },
        pool: {
            algorithm: 'pool',
            totalSize: 1024,
            allocations: [{ id: 1, size: 32, address: 0 }],
            pools: [{ size: 32, count: 16, used: 1, address: 0, chunks: [{ allocated: true, id: 1 }] }]
        },
        slab: {
            algorithm: 'slab',
            totalSize: 1024,
            allocations: [{ id: 1, size: 64, address: 0, type: 'TestStruct' }],
            caches: [{ name: 'TestStruct', objectSize: 64, slabs: [{ objects: [{ allocated: true, id: 1 }] }] }]
        },
        hybrid: {
            algorithm: 'hybrid',
            totalSize: 1024,
            allocations: [{ id: 1, size: 32, address: 0, strategy: 'small' }],
            strategies: [{ name: 'Small Objects', type: 'pool', usage: 50, totalSize: 256, usedSize: 128 }]
        }
    };
    
    const algorithms = ['buddy', 'pool', 'slab', 'hybrid'];
    let success = true;
    
    for (const algorithm of algorithms) {
        try {
            console.log(`\n🧪 Testing ${algorithm} visualization...`);
            
            // Clear previous content
            document.getElementById('algorithmSpecificViz').innerHTML = '';
            
            // Test the specific method
            switch (algorithm) {
                case 'buddy':
                    demo.generateBuddyTree(testData.buddy);
                    break;
                case 'pool':
                    demo.generatePoolChunks(testData.pool);
                    break;
                case 'slab':
                    demo.generateSlabCaches(testData.slab);
                    break;
                case 'hybrid':
                    demo.generateHybridStrategies(testData.hybrid);
                    break;
            }
            
            // Check if content was generated
            const content = document.getElementById('algorithmSpecificViz').innerHTML;
            if (content.trim().length > 0) {
                console.log(`✅ ${algorithm} visualization generated content`);
            } else {
                console.error(`❌ ${algorithm} visualization generated no content`);
                success = false;
            }
            
        } catch (error) {
            console.error(`❌ ${algorithm} visualization failed:`, error);
            success = false;
        }
    }
    
    return success;
}

// 5. Test full integration with renderAlgorithmSpecificVisualization
function testFullIntegration(demo) {
    console.log('\n🔗 Testing full integration...');
    
    const testLayouts = [
        { algorithm: 'buddy', totalSize: 1024, allocations: [] },
        { algorithm: 'pool', totalSize: 1024, allocations: [] },
        { algorithm: 'slab', totalSize: 1024, allocations: [] },
        { algorithm: 'hybrid', totalSize: 1024, allocations: [] }
    ];
    
    for (const layout of testLayouts) {
        try {
            console.log(`\n🧪 Testing full integration for ${layout.algorithm}...`);
            demo.renderAlgorithmSpecificVisualization(layout);
            console.log(`✅ ${layout.algorithm} full integration successful`);
        } catch (error) {
            console.error(`❌ ${layout.algorithm} full integration failed:`, error);
            return false;
        }
    }
    
    return true;
}

// Main validation function
async function runFullValidation() {
    console.log('🚀 Running complete validation suite...\n');
    
    // Step 1: Validate containers
    if (!validateContainers()) {
        console.log('\n❌ VALIDATION FAILED: Missing required containers');
        return false;
    }
    
    // Step 2: Validate class
    const demo = validateClass();
    if (!demo) {
        console.log('\n❌ VALIDATION FAILED: MemoryAllocatorDemo class issues');
        return false;
    }
    
    // Step 3: Validate methods
    if (!validateMethods(demo)) {
        console.log('\n❌ VALIDATION FAILED: Missing required methods');
        return false;
    }
    
    // Step 4: Test algorithm visualizations
    if (!testAlgorithmVisualizations(demo)) {
        console.log('\n❌ VALIDATION FAILED: Algorithm visualization issues');
        return false;
    }
    
    // Step 5: Test full integration
    if (!testFullIntegration(demo)) {
        console.log('\n❌ VALIDATION FAILED: Full integration issues');
        return false;
    }
    
    console.log('\n🎉 ALL VALIDATIONS PASSED! 🎉');
    console.log('The Memory Allocator Demo is fully functional and ready for use.');
    return true;
}

// Auto-run validation if this script is executed
runFullValidation();

// Export functions for manual testing
window.validateMemoryAllocatorDemo = runFullValidation;
window.validateContainers = validateContainers;
window.validateClass = validateClass;
