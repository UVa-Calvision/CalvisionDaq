#pragma once

#include <cstdint>
#include <array>

// Fixed size typedefs
using UIntType = uint32_t;
using FloatingType = float;

// DRS Constants
constexpr static UIntType N_Channels = 8;
constexpr static UIntType N_Samples = 1024;
constexpr static UIntType N_Groups = 2;

constexpr static UIntType N_Chunks = 128;

// Array typedefs
template <typename T>
using SampleArray = std::array<T, N_Samples>;

template <typename T>
using ChannelArray = std::array<T, N_Channels>;

template <typename T>
using ChannelMatrix = ChannelArray<SampleArray<T> >;

template <typename T>
using GroupArray = std::array<T, N_Groups>;


// Group masks
inline
UIntType group_mask(UIntType group) {
    return 1 << group;
}
