#include "BufferedFileIO.h"

#include <mutex>
#include <iostream>
#include <chrono>
#include <condition_variable>

using namespace std::chrono_literals;

std::mutex swap_mutex, done_mutex;
std::condition_variable cleared_intermediate, ready_to_save;

#include <sstream>
#include <string>
template <typename T>
std::string to_string(const T* t, UIntType count) {
    std::stringstream out;
    out << "[";
    for (int i = 0; i < count; i++) {
        if (i > 0)
            out << ", ";
        out << t[i];
    }
    out << "]";
    return out.str();
}

BufferedFileWriter::BufferedFileWriter()
    : input_buffer_(&buffer_1_),
    output_buffer_(&buffer_2_),
    intermediate_buffer_(&buffer_3_),
    finished_(false),
    outfile_("output.dat", std::fstream::binary | std::fstream::trunc)
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

void BufferedFileWriter::swap_write_buffers() {
    std::unique_lock<std::mutex> lock(swap_mutex);
    cleared_intermediate.wait(lock, [this] () { return !this->intermediate_buffer_dirty(); });

    std::swap(input_buffer_, intermediate_buffer_);

    lock.unlock();
    ready_to_save.notify_one();
}

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
        outfile_.write((char*) output_buffer_->buffer(), output_buffer_->size() * sizeof(BufferedType));
        outfile_.flush();
    }
}

void BufferedFileWriter::close() {

    if (saving_thread.joinable()) {
        done_mutex.lock();
        finished_ = true;
        done_mutex.unlock();

        ready_to_save.notify_one();

        saving_thread.join();
    }

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
