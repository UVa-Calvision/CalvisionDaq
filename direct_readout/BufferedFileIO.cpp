#include "BufferedFileIO.h"

#include <mutex>
#include <iostream>
#include <chrono>

using namespace std::chrono_literals;

std::mutex swap_mutex, done_mutex;

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
    if (count * sizeof(BufferedType) > BufferSize) {
        std::cout << "Attempting to write more events than in a single buffer!\n"
            << "Buffer Size: " << BufferSize << "; write size: " << count * sizeof(BufferedType) << "\n";
    }
    UIntType written = input_buffer_->write(t, count);
    if (input_buffer_->full()) {
        swap_write_buffers();

        if (written < count) {
            input_buffer_->write(t + written, count - written);
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
    while (intermediate_buffer_dirty()) {
        std::this_thread::sleep_for(1ms);
    }

    std::lock_guard<std::mutex> lock(swap_mutex);
    std::swap(input_buffer_, intermediate_buffer_);
}

void BufferedFileWriter::swap_output_buffers() {
    output_buffer_->clear();

    while (!finished() && !intermediate_buffer_dirty()) {
        std::this_thread::sleep_for(1ms);
    }

    if (finished()) return;

    std::lock_guard<std::mutex> lock(swap_mutex);
    std::swap(output_buffer_, intermediate_buffer_);
}

void BufferedFileWriter::save() {
    if (output_buffer_->size() > 0) {
        std::cout << "[thread] Writing " << output_buffer_->size() << " bytes\n";
        outfile_.write((char*) output_buffer_->buffer(), output_buffer_->size() * sizeof(BufferedType));
        outfile_.flush();
    }
}

void BufferedFileWriter::close() {

    if (saving_thread.joinable()) {
        done_mutex.lock();
        finished_ = true;
        done_mutex.unlock();

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
