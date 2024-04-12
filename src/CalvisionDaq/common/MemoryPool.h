#pragma once

#include "CppUtils/c_util/CUtil.h"

#include <array>
#include <cstdint>
#include <mutex>
#include <atomic>
#include <vector>


template <size_t N>
class MemoryBlock {
public:
    MemoryBlock()
        : next_free_(nullptr)
    {}

    ~MemoryBlock() {
        if (next_free_) {
            delete next_free_;
            next_free_ = nullptr;
        }
    }

    std::array<uint8_t, N>& block() { return block_; }
    const std::array<uint8_t, N>& block() const { return block_; }

    MemoryBlock<N>*& next_free() { return next_free_; }

private:
    MemoryBlock<N>* next_free_;
    std::array<uint8_t, N> block_;
};



/*
 * Thread-safe memory pool
 */
template <size_t N>
class MemoryPool {
public:
    using BlockType = MemoryBlock<N>;

    // Constructor not threadsafe
    MemoryPool(size_t initial_size)
        : free_list_(nullptr)
    {
        for (size_t i = 0; i < initial_size; i++) {
            deallocate(new BlockType());
            count_++;
        }
    }

    // Destructor not threadsafe
    ~MemoryPool() {
        if (free_list_) {
            delete free_list_;
            free_list_ = nullptr;
        }
    }

    // Threadsafe - blocking
    BlockType* allocate() {
        const std::lock_guard<std::mutex> lock(free_list_mutex_);

        BlockType* block = free_list_;

        if (!block) {
            block = new BlockType();
            count_++;
        }

        free_list_ = block->next_free();
        return block;
    }

    std::vector<BlockType*> allocate(size_t n) {
        std::vector<BlockType*> blocks(n, nullptr);

        if (n == 0) return blocks;

        const std::lock_guard<std::mutex> lock(free_list_mutex_);

        for (size_t i = 0; i < n; i++) {
            blocks[i] = free_list_;

            if (!blocks[i]) {
                blocks[i] = new BlockType();
                count_++;
            }

            free_list_ = blocks[i]->next_free();
        }

        return blocks;
    }

    // Threadsafe - blocking
    void deallocate(BlockType* block) {
        const std::lock_guard<std::mutex> lock(free_list_mutex_);

        block->next_free() = free_list_;
        free_list_ = block;
    }

    size_t total_count() const { return count_; }

private:
    BlockType* free_list_;
    std::mutex free_list_mutex_;

    // debug
    size_t count_ = 0;
};


/*
 * Thread-safe single-producer single-consumer queue
 */
template <typename T, size_t N = 1024>
class SPSCQueue {
public:
    SPSCQueue() 
        : read_pos_(0), write_pos_(0)
    {}

    void add(T* t) {
        size_t next_write_pos = next(write_pos_.load(std::memory_order_relaxed));
        while (next_write_pos == read_pos_.load(std::memory_order_relaxed)) {
            // don't yield?
            write_waits_++;
        }

        buffer_[write_pos_.load(std::memory_order_relaxed)] = t;
        write_pos_.store(next_write_pos, std::memory_order_relaxed);
    }

    T* pop() {
        size_t read_pos = read_pos_.load(std::memory_order_relaxed);
        while (!closed() && read_pos == write_pos_.load(std::memory_order_relaxed)) {
            // yield?
            read_waits_++;
        }

        if (read_pos == write_pos_.load(std::memory_order_relaxed) && closed()) return nullptr;

        T* result = buffer_[read_pos];
        read_pos_.store(next(read_pos), std::memory_order_relaxed);

        return result;
    }

    void close() {
        closed_.store(true, std::memory_order_release);
    }

    bool closed() const {
        return closed_.load(std::memory_order_acquire);
    }

    uint64_t write_waits() const { return write_waits_; }
    uint64_t read_waits() const { return read_waits_; }

private:
    static size_t next(size_t i) { return (i + 1) % N; }

    std::array<T*, N> buffer_;
    std::atomic<size_t> write_pos_, read_pos_;

    std::atomic<bool> closed_;

    uint64_t write_waits_ = 0;
    uint64_t read_waits_ = 0;
};
