#pragma once

#include <fstream>
#include <cstring>

#include "Forward.h"

class BinaryInputStream {
public:
    virtual ~BinaryInputStream() {}

    UIntType read_int() {
        UIntType uint;
        this->read_impl((char*) &uint, sizeof(UIntType));
        return uint;
    }

    template <typename T, size_t N>
    std::array<T,N> read() {
        std::array<T,N> buffer;
        this->read_buffer(buffer);
        return buffer;
    }

    template <typename T, size_t N>
    BinaryInputStream& read_buffer(std::array<T,N>& buffer) {
        this->read_impl((char*) buffer.data(), sizeof(T) * N);
        return *this;
    }

    virtual bool good() const = 0;

    operator bool() const { return good(); }

protected:
    virtual void read_impl(char* buffer, size_t N) = 0;
};

/*
 * Binary file read
 */
class BinaryInputFileStream : public BinaryInputStream {
public:
    explicit BinaryInputFileStream(const std::string& filename)
        : input(filename, std::fstream::in | std::fstream::binary)
    {}

    virtual ~BinaryInputFileStream() {}

    virtual bool good() const override { return input.good(); }

    template <typename FuncType>
    void follow(const FuncType& f) {
        while (true) {

            f(*this);

            if (!input.eof()) break;
            input.clear();
        }
    }

    bool eof() const { return input.eof(); }
    void clear() { input.clear(); }

protected:

    virtual void read_impl(char* buffer, size_t N) override {
        input.read(buffer, N);
    }

    std::ifstream input;
};

class BinaryInputBufferStream : public BinaryInputStream {
public:
    explicit BinaryInputBufferStream(const char* data, size_t N)
        : ptr_(data), offset_(0), size_(N)
    {}

    virtual ~BinaryInputBufferStream() {}

    virtual bool good() const override { return offset_ < size_; }

protected:
    virtual void read_impl(char* buffer, size_t N) override {
        if (!good()) return;
        std::memcpy((void*) buffer, (void*) (offset_ + ptr_), N);
        offset_ += N;
    }

    const char* ptr_;
    size_t offset_, size_;
};


/*
 * Binary file write
 */
class BinaryOutputFileStream {
public:
    BinaryOutputFileStream(const std::string& filename)
        : output(filename, std::fstream::out | std::fstream::binary)
    {}

    void write_int(UIntType uint) {
        output.write((char*) &uint, sizeof(UIntType));
    }

    template <typename T, size_t N>
    void write_buffer(const std::array<T, N>& buffer) {
        output.write((char*) buffer.data(), N * sizeof(T));
    }

    template <typename T>
    void write(const T* buffer, UIntType size) {
        output.write((char*) buffer, size * sizeof(T));
    }

    void flush() {
        output.flush();
    }

    void close() {
        output.close();
    }

    bool is_open() const { return output.is_open(); }

    bool good() const { return output.good(); }

private:
    std::ofstream output;
};


