#pragma once

#include <cstddef>
#include <new>

// 1. 定义一个跨平台的缓存行大小常量
#ifdef __cpp_lib_hardware_interference_size
    // 如果编译器支持 C++17 特性，则使用标准库定义
    constexpr std::size_t cache_line_size = std::hardware_destructive_interference_size;
#else
    // 否则，手动指定为常见的 64 字节
    constexpr std::size_t cache_line_size = 64;
#endif

template <typename T>
struct CachePadded {
    // 2. 强制整个结构体按缓存行大小对齐
    alignas(cache_line_size) T data;
    
    // 3. 编译器会自动在 data 之后添加填充(padding)，
    //    确保 sizeof(CachePadded<T>) == cache_line_size
    //    （前提是 sizeof(T) <= cache_line_size）

    // 4. 提供一个构造函数，方便从 T 类型直接构造
    template <typename... Args>
    explicit CachePadded(Args&&... args) : data(std::forward<Args>(args)...) {}
    
    // 5. (可选) 提供隐式类型转换，方便使用
    operator T& () { return data; }
    operator const T& () const { return data; }
};