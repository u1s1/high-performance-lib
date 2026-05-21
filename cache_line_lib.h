#pragma once

#include <cstddef>
#include <new>
#include <utility>
#include <type_traits>

#if defined(__GCC_DESTRUCTIVE_SIZE)
    constexpr std::size_t __hardware_cache_line_size = __GCC_DESTRUCTIVE_SIZE;
#else
    constexpr std::size_t __hardware_cache_line_size = 64;
#endif

namespace hpl {

template <typename T>
struct cache_padded {
    // 强制整个结构体按缓存行大小对齐
    alignas(__hardware_cache_line_size) T data;
    
    // ==========================================
    // 1. 构造与析构
    // ==========================================
    cache_padded() = default;
    ~cache_padded() = default;

    // 提供一个完美转发的构造函数，方便从 T 类型的参数直接构造
    template <typename... Args>
    explicit cache_padded(Args&&... args) : data(std::forward<Args>(args)...) {}

    // 允许拷贝和移动构造
    cache_padded(const cache_padded&) = default;
    cache_padded(cache_padded&&) = default;

    // ==========================================
    // 2. 赋值运算符重载 (Assignment Operators)
    // ==========================================
    
    // 默认的拷贝和移动赋值
    cache_padded& operator=(const cache_padded&) = default;
    cache_padded& operator=(cache_padded&&) = default;

    // 从 T 类型左值赋值
    cache_padded& operator=(const T& value) {
        data = value;
        return *this;
    }

    // 从 T 类型右值赋值 (支持移动语义)
    cache_padded& operator=(T&& value) noexcept(std::is_nothrow_move_assignable_v<T>) {
        data = std::move(value);
        return *this;
    }

    // ==========================================
    // 3. 隐式类型转换 (Conversion Operators)
    // 核心魔法：让 cache_padded<T> 可以直接当成 T 来用，
    // 编译器会自动处理原生类型 T 的 +, -, ++, -- 等算术操作。
    // ==========================================
    operator T&() noexcept { return data; }
    operator const T&() const noexcept { return data; }

    // ==========================================
    // 4. 指针/解引用运算符 (Access Operators)
    // 行为类似智能指针或 std::optional，方便显式访问
    // ==========================================
    T& operator*() noexcept { return data; }
    const T& operator*() const noexcept { return data; }

    T* operator->() noexcept { return &data; }
    const T* operator->() const noexcept { return &data; }

    // ==========================================
    // 5. 比较运算符重载 (Comparison Operators)
    // ==========================================
    
    // 同类型之间的比较
    bool operator==(const cache_padded& rhs) const { return data == rhs.data; }
    bool operator!=(const cache_padded& rhs) const { return data != rhs.data; }
    bool operator<(const cache_padded& rhs) const { return data < rhs.data; }
    bool operator>(const cache_padded& rhs) const { return data > rhs.data; }
    bool operator<=(const cache_padded& rhs) const { return data <= rhs.data; }
    bool operator>=(const cache_padded& rhs) const { return data >= rhs.data; }

    // 与底层的 T 类型直接比较
    bool operator==(const T& rhs) const { return data == rhs; }
    bool operator!=(const T& rhs) const { return data != rhs; }
    bool operator<(const T& rhs) const { return data < rhs; }
    bool operator>(const T& rhs) const { return data > rhs; }
    bool operator<=(const T& rhs) const { return data <= rhs; }
    bool operator>=(const T& rhs) const { return data >= rhs; }
};

} // namespace hpl