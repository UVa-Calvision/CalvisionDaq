#pragma once

#include "forward.h"

#include <fstream>
#include <cstring>
#include <cstdint>
#include <thread>

using BufferedType = char;

constexpr static UIntType N_EventsBuffer = 1000;
constexpr static UIntType EventSize = (4 + 2 + 3 * N_Samples) * sizeof(UIntType);
constexpr static UIntType BufferSize = N_EventsBuffer * EventSize / sizeof(BufferedType);

class DataBuffer {
public:
    DataBuffer()
        : buffer_(new BufferedType[BufferSize]), size_(0)
    {}

    ~DataBuffer()
    {
        delete[] buffer_;
    }

    /*
     * Appends the array in t of length count to the buffer. Returns the number of written elements.
     */
    UIntType write(const BufferedType* t, UIntType count) {
        UIntType num_to_write = std::min(count, BufferSize - size_);
        // Accessing end of array is potential UB, even if num_to_write is 0.
        if (num_to_write > 0) {
            std::memcpy((void*) &buffer_[size_], (void*) t, sizeof(BufferedType) * num_to_write);
        }
        size_ += num_to_write;
        return num_to_write;
    }

    void clear() {
        size_ = 0;
    }

    const BufferedType* buffer() const {
        return buffer_;
    }

    UIntType size() const {
        return size_;
    }

    bool full() const {
        return size_ == BufferSize;
    }

private:
    BufferedType* buffer_;
    UIntType size_;
};

class BufferedFileWriter {
public:
    BufferedFileWriter();
    ~BufferedFileWriter();

    /*
     * Pre: count < BufferSize
     */
    void write(const BufferedType* t, UIntType count);

    void close();

private:
    bool intermediate_buffer_dirty() const;
    bool finished() const;

    void swap_write_buffers();
    void swap_output_buffers();

    void save_loop();
    void save();

    DataBuffer buffer_1_, buffer_2_, buffer_3_;
    DataBuffer* input_buffer_;          // Currently has data being written to it
    DataBuffer* intermediate_buffer_;   // Temporary storage buffer
    DataBuffer* output_buffer_;         // Data currently being written to a file

    std::thread saving_thread;
    std::ofstream outfile_;

    volatile bool finished_;
};


