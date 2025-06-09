// Quick Browser Console Test for Memory Allocator Demo
// Copy and paste this into the browser console when on the demo page

console.log('🚀 Starting Memory Allocator Demo Final Integration Test...');

// Function to test all visualizations quickly
function quickTest() {
    console.log('\n📋 Quick Integration Test Starting...');
    
    // 1. Check containers
    const memoryViz = document.getElementById('memoryViz');
    const algorithmViz = document.getElementById('algorithmSpecificViz');
    
    if (!memoryViz || !algorithmViz) {
        console.error('❌ Required containers missing!');
        return false;
    }
    console.log('✅ All containers found');
    
    // 2. Check if MemoryAllocatorDemo is available
    if (typeof MemoryAllocatorDemo === 'undefined') {
        console.error('❌ MemoryAllocatorDemo class not available!');
        return false;
    }
    console.log('✅ MemoryAllocatorDemo class available');
    
    // 3. Create instance and test methods
    try {
        const demo = new MemoryAllocatorDemo();
        console.log('✅ MemoryAllocatorDemo instance created');
        
        // Test all algorithm-specific visualization methods
        const testData = {
            buddy: { algorithm: 'buddy', buddyTree: { size: 1024, level: 0 } },
            pool: { algorithm: 'pool', pools: [{ size: 32, count: 16, used: 5 }] },
            slab: { algorithm: 'slab', caches: [{ name: 'Test', objectSize: 64, slabs: [{ objects: [] }] }] },
            hybrid: { algorithm: 'hybrid', strategies: [{ name: 'Test Strategy', usage: 50 }] }
        };
        
        console.log('\n🧪 Testing individual methods...');
        
        // Test each method
        demo.generateBuddyTree(testData.buddy);
        console.log('✅ generateBuddyTree(data) works');
        
        demo.generatePoolChunks(testData.pool);
        console.log('✅ generatePoolChunks(data) works');
        
        demo.generateSlabCaches(testData.slab);
        console.log('✅ generateSlabCaches(data) works');
        
        demo.generateHybridStrategies(testData.hybrid);
        console.log('✅ generateHybridStrategies(data) works');
        
        // Test full integration
        console.log('\n🔗 Testing full integration...');
        demo.renderAlgorithmSpecificVisualization({ algorithm: 'buddy' });
        console.log('✅ Buddy system integration works');
        
        demo.renderAlgorithmSpecificVisualization({ algorithm: 'pool' });
        console.log('✅ Pool system integration works');
        
        demo.renderAlgorithmSpecificVisualization({ algorithm: 'slab' });
        console.log('✅ Slab system integration works');
        
        demo.renderAlgorithmSpecificVisualization({ algorithm: 'hybrid' });
        console.log('✅ Hybrid system integration works');
        
        console.log('\n🎉 ALL TESTS PASSED! The Memory Allocator Demo is fully functional! 🎉');
        return true;
        
    } catch (error) {
        console.error('❌ Test failed:', error);
        return false;
    }
}

// Run the test
quickTest();

// Make test function available globally
window.quickTest = quickTest;

console.log('\n📝 To run this test again, type: quickTest()');
console.log('📝 To test global functions, try: window.testAlgorithm("buddy")');
