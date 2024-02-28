#pragma once

#include <fstream>

#include "forward.h"

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
    void read_buffer(std::array<T, N>& buffer) {
        input.read((char*) buffer.data(), N * sizeof(T));
    }

    bool good() const { return input.good(); }

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

    bool good() const { return output.good(); }

private:
    std::ofstream output;
};


