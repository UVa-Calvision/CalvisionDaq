#pragma once

#include "Forward.h"

#include "BinaryIO.h"

#include <cstring>
#include <thread>

using BufferedType = UIntType;

constexpr static UIntType N_EventsBuffer = 1000;
constexpr static UIntType EventSize = (4 + 2 + 3 * N_Samples) * sizeof(UIntType);
constexpr static UIntType BufferSize = N_EventsBuffer * EventSize / sizeof(BufferedType);

class DataBuffer {
public:
    DataBuffer();
    ~DataBuffer();

    /*
     * Appends the array in t of length count to the buffer. Returns the number of written elements.
     */
    UIntType write(const BufferedType* t, UIntType count);
    void clear();
    const BufferedType* buffer() const;
    UIntType size() const;
    bool full() const;

private:
    BufferedType* buffer_;
    UIntType size_;
};

class BufferedFileWriter {
public:
    BufferedFileWriter(const std::string& filename);
    ~BufferedFileWriter();

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
    BinaryOutputFileStream outfile_;

    volatile bool finished_;
};


