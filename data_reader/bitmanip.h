#pragma once

#include <tuple>
#include <array>
#include "common.h"

namespace bmp {

static constexpr UIntType uint_size = 8 * sizeof(UIntType);

template <UIntType offset, UIntType size>
UIntType read_uint(UIntType data) {
    return (data << offset) >> (uint_size - size);
}

template <UIntType offset, UIntType size>
std::tuple<UIntType> read_helper(UIntType data) {
    return { read_uint<offset, size>(data) };
}

template <UIntType offset, UIntType size, UIntType next_size, UIntType... sizes>
auto read_helper(UIntType data) {
    return std::tuple_cat(read_helper<offset, size>(data),
                          read_helper<offset + size, next_size, sizes...>(data));
}


template <UIntType... sizes>
auto read(UIntType data) {
    static_assert(uint_size == (sizes + ...));
    return read_helper<0, sizes...>(data);
}

inline
std::array<UIntType, 8> read_8_channels(const std::array<UIntType, 3>& data) {
    std::array<UIntType, 8> channels;

    UIntType c5_high, c2_high;

    std::tie(channels[7],channels[6],c5_high) = bmp::read<12,12,8>(data[2]);
    std::tie(channels[5],channels[4],channels[3],c2_high) = bmp::read<4,12,12,4>(data[1]);
    std::tie(channels[2],channels[1],channels[0]) = bmp::read<8,12,12>(data[0]);

    channels[5] |= c5_high << 4;
    channels[2] |= c2_high << 8;

    return channels;
}


}
