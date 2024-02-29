#include "BufferedFileIO.h"

#include <mutex>
#include <iostream>
#include <chrono>
#include <condition_variable>

using namespace std::chrono_literals;

// ===== DataBuffer

DataBuffer::DataBuffer()
    : buffer_(new BufferedType[BufferSize]), size_(0)
{}

DataBuffer::~DataBuffer() {
    delete[] buffer_;
}

UIntType DataBuffer::write(const BufferedType* t, UIntType count) {
    UIntType num_to_write = std::min(count, BufferSize - size_);
    // Accessing end of array is potential UB, even if num_to_write is 0.
    if (num_to_write > 0) {
        std::memcpy((void*) &buffer_[size_], (void*) t, sizeof(BufferedType) * num_to_write);
    }
    size_ += num_to_write;
    return num_to_write;
}

void DataBuffer::clear() {
    size_ = 0;
}

const BufferedType* DataBuffer::buffer() const {
    return buffer_;
}

UIntType DataBuffer::size() const {
    return size_;
}

bool DataBuffer::full() const {
    return size_ == BufferSize;
}


// ===== BufferedFileWriter

std::mutex swap_mutex, done_mutex;
std::condition_variable cleared_intermediate, ready_to_save;

BufferedFileWriter::BufferedFileWriter()
    : input_buffer_(&buffer_1_),
    output_buffer_(&buffer_2_),
    intermediate_buffer_(&buffer_3_),
    finished_(false),
    outfile_("output.dat")
{
    saving_thread = std::thread(&BufferedFileWriter::save_loop, std::ref(*this));
}

BufferedFileWriter::~BufferedFileWriter()
{
    close();
}

void BufferedFileWriter::write(const BufferedType* t, UIntType count) {
    UIntType written = 0;
    while (written < count) {
        written += input_buffer_->write(t + written, count - written);

        if (input_buffer_->full()) {
            swap_write_buffers();
        }
    }
}

bool BufferedFileWriter::intermediate_buffer_dirty() const {
    return intermediate_buffer_->full();
}

bool BufferedFileWriter::finished() const {
    return finished_;
}

/*
 * Exchange the input and intermediate buffers.
 * Waits for intermediate_buffer_dirty() to be false and may be notified via cleared_intermediate.
 */
void BufferedFileWriter::swap_write_buffers() {
    std::unique_lock<std::mutex> lock(swap_mutex);
    cleared_intermediate.wait(lock, [this] () { return !this->intermediate_buffer_dirty(); });

    std::swap(input_buffer_, intermediate_buffer_);

    lock.unlock();
    ready_to_save.notify_one();
}

/*
 * Exchange the output and intermediate buffers.
 * Waits for intermediate_buffer_dirty() to be true or finished, and may be notified via ready_to_save.
 */
void BufferedFileWriter::swap_output_buffers() {
    output_buffer_->clear();

    std::unique_lock<std::mutex> lock(swap_mutex);
    ready_to_save.wait(lock, [this] () { return this->intermediate_buffer_dirty() || this->finished(); });

    if (finished()) return;

    std::swap(output_buffer_, intermediate_buffer_);
    
    lock.unlock();
    cleared_intermediate.notify_one();
}

void BufferedFileWriter::save() {
    if (output_buffer_->size() > 0) {
        // std::cout << "[buffered] saving " << output_buffer_->size() << " uints\n";
        outfile_.write<BufferedType>(output_buffer_->buffer(), output_buffer_->size());
        outfile_.flush();
    }
}

void BufferedFileWriter::close() {

    // If the thread is still running, tell it to stop.
    if (saving_thread.joinable()) {
        done_mutex.lock();
        finished_ = true;
        done_mutex.unlock();

        ready_to_save.notify_one();

        saving_thread.join();
    }

    // Save the remaining buffers
    if (!outfile_.is_open()) return;

    save();

    if (intermediate_buffer_->size() > 0) {
        std::swap(output_buffer_, intermediate_buffer_);
        save();
    }

    if (input_buffer_->size() > 0) {
        std::swap(output_buffer_, input_buffer_);
        save();
    }

    outfile_.close();
}

void BufferedFileWriter::save_loop() {
    do {

        done_mutex.lock();
        if (finished_) break;
        done_mutex.unlock();

        save();
        swap_output_buffers();
    } while (true);
}
