#pragma once

#include <string>
#include <fstream>
#include <array>
#include <cstdint>

using UIntType = uint32_t;
using FloatingType = double;

constexpr static UIntType N_Channels = 8;
constexpr static UIntType N_Samples = 1024;
constexpr static UIntType N_Chunks = 128;
constexpr static UIntType N_Groups = 2;

template <typename T>
using SampleArray = std::array<T, N_Samples>;

template <typename T>
using ChannelArray = std::array<T, N_Channels>;

template <typename T>
using ChannelMatrix = ChannelArray<SampleArray<T> >;

template <typename T>
using GroupArray = std::array<T, N_Groups>;



inline
UIntType group_mask(UIntType group) {
    return 1 << group;
}



template <typename T>
std::string to_string_helper(T t) {
    return std::to_string(t);
}

template <> inline
std::string to_string_helper<const char*>(const char* c) {
    return std::string(c);
}

template <typename Arg>
std::string canonical_name(const Arg& arg) {
    return to_string_helper(arg);
}

template <typename Arg, typename Next, typename... Args>
std::string canonical_name(const Arg& a, const Next& next, const Args&... args) {
    return canonical_name(a) + "_" + canonical_name(next, args...);
}

inline
std::string name_timestamp(int g) {
    return canonical_name("timestap", g);
}

inline
std::string name_time(int g) {
    return canonical_name("sample_time", g);
}

inline
std::string name_trigger(int g) {
    return canonical_name("trigger", g);
}

inline
std::string name_channel(int g, int c) {
    return canonical_name("channel", g, c);
}

/*
 * Binary file read
 */
class BinaryInputFileStream {
public:
    BinaryInputFileStream(const std::string& filename)
        : input(filename)
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
        : output(filename)
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



/*
 * Calibration data for a single group.
 */
class DRSGroupCalibration {
public:

    void read(const std::string& filename) {
        BinaryInputFileStream infile(filename);

        for (auto&& channel : channel_offset) {
            infile.read_buffer(channel);
        }

        infile.read_buffer(trigger_offset);
        infile.read_buffer(timing_correction);
    }

    void write(const std::string& filename) const {
        BinaryOutputFileStream outfile(filename);
        
        for (const auto& channel : channel_offset) {
            outfile.write_buffer(channel);
        }

        outfile.write_buffer(trigger_offset);
        outfile.write_buffer(timing_correction);
    }

public:
    ChannelMatrix<UIntType> channel_offset;
    SampleArray<UIntType> trigger_offset;
    SampleArray<FloatingType> timing_correction;
};
