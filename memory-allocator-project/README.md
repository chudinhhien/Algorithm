# Bài Tập Lớn: Thiết Kế Bộ Cấp Phát Bộ Nhớ (Memory Allocator Design)

## Mô tả Dự án

Dự án này triển khai một bộ cấp phát bộ nhớ tùy chỉnh được tối ưu hóa cho:
- **Tốc độ (Speed)**: Thời gian cấp phát và giải phóng bộ nhớ nhanh
- **Phân mảnh (Fragmentation)**: Giảm thiểu phân mảnh nội bộ và ngoại bộ
- **Khả năng mở rộng (Scalability)**: Hiệu quả với các ứng dụng đa luồng

## Cấu trúc Dự án

```
memory-allocator-project/
├── src/                          # Mã nguồn C++
│   ├── core/                     # Core allocator implementations
│   │   ├── buddy_allocator.cpp   # Buddy System Allocator
│   │   ├── slab_allocator.cpp    # Slab Allocator
│   │   ├── pool_allocator.cpp    # Memory Pool Allocator
│   │   └── hybrid_allocator.cpp  # Hybrid Allocator (tùy chỉnh)
│   ├── includes/                 # Header files
│   │   ├── memory_allocator.h    # Base allocator interface
│   │   ├── buddy_allocator.h     
│   │   ├── slab_allocator.h      
│   │   ├── pool_allocator.h      
│   │   └── hybrid_allocator.h    
│   ├── utils/                    # Utilities
│   │   ├── benchmark.cpp         # Performance benchmarking
│   │   ├── statistics.cpp        # Memory usage statistics
│   │   └── visualization.cpp     # Memory layout visualization
│   └── main.cpp                  # Main testing program
├── demo/                         # Web demo interface
│   ├── index.html               # Main demo page
│   ├── css/
│   │   └── style.css            # Styling
│   ├── js/
│   │   ├── allocator-demo.js    # Demo functionality
│   │   └── visualization.js     # Memory visualization
│   └── api/
│       └── allocator-api.cpp    # C++ backend for web interface
├── tests/                       # Test cases
│   ├── unit_tests.cpp          # Unit tests for each allocator
│   ├── performance_tests.cpp   # Performance benchmarks
│   └── stress_tests.cpp        # Stress testing
├── docs/                       # Documentation
│   ├── design_document.md      # Thiết kế chi tiết
│   ├── algorithms.md           # Giải thích các thuật toán
│   ├── performance_analysis.md # Phân tích hiệu suất
│   └── user_guide.md          # Hướng dẫn sử dụng
└── Makefile                    # Build configuration
```

## Các Thuật Toán Triển Khai

### 1. Buddy System Allocator
- Chia bộ nhớ thành các khối có kích thước là lũy thừa của 2
- Giảm phân mảnh ngoại bộ
- Hỗ trợ coalescing khi giải phóng bộ nhớ

### 2. Slab Allocator
- Tối ưu cho việc cấp phát các object có kích thước cố định
- Giảm phân mảnh nội bộ
- Cache-friendly allocation

### 3. Memory Pool Allocator
- Pre-allocate large chunks of memory
- Fast allocation/deallocation
- Predictable memory usage

### 4. Hybrid Allocator (Tùy chỉnh)
- Kết hợp ưu điểm của các thuật toán trên
- Tự động chọn thuật toán phù hợp dựa trên kích thước allocation
- Tối ưu hóa cho hiệu suất tổng thể

## Tính năng Demo

1. **Visualization Interface**: Hiển thị trực quan cách các allocator hoạt động
2. **Performance Comparison**: So sánh hiệu suất giữa các thuật toán
3. **Interactive Testing**: Cho phép người dùng test với các pattern allocation khác nhau
4. **Real-time Statistics**: Hiển thị thống kê fragmentation, allocation time, etc.

## Cách Build và Chạy

```bash
# Build the project
make all

# Run tests
make test

# Run benchmarks
make benchmark

# Start web demo
make demo
```

## Yêu cầu Hệ thống

- C++17 or later
- GCC/Clang compiler
- Web browser (for demo)
- Python 3.x (for web server)

## Đánh giá và Tiêu chí

1. **Correctness (25%)**: Tính đúng đắn của các thuật toán
2. **Performance (25%)**: Hiệu suất allocation/deallocation
3. **Fragmentation Control (20%)**: Khả năng kiểm soát phân mảnh
4. **Code Quality (15%)**: Chất lượng code, documentation
5. **Innovation (15%)**: Tính sáng tạo trong thiết kế hybrid allocator

## Tác giả

[Tên sinh viên]
[MSSV]
[Lớp]
