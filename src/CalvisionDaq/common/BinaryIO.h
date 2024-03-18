#pragma once

#include <fstream>

#include "Forward.h"

/*
 * Binary file read
 */
class BinaryInputFileStream {
public:
    BinaryInputFileStream(const std::string& filename)
        : input(filename, std::fstream::in | std::fstream::binary)
    {}

    UIntType read_int() {
        static UIntType uint;
        input.read((char*) &uint, sizeof(UIntType));
        return uint;
    }

    template <typename T, size_t N>
    std::array<T,N> read() {
        std::array<T,N> buffer;
        input.read((char*) buffer.data(), N * sizeof(T));
        return buffer;
    }

    template <typename T, size_t N>
    BinaryInputFileStream& read_buffer(std::array<T, N>& buffer) {
        input.read((char*) buffer.data(), N * sizeof(T));
        return *this;
    }

    bool good() const { return input.good(); }

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

    operator bool() const { return good(); }

private:
    std::ifstream input;
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


