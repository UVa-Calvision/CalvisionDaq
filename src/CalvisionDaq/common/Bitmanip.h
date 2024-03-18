#pragma once

#include <tuple>
#include "Forward.h"

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

ChannelArray<UIntType> read_8_channels(const std::array<UIntType, 3>& data);

}
