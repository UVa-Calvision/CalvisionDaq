#include "Bitmanip.h"

namespace bmp {

ChannelArray<UIntType> read_8_channels(const std::array<UIntType, 3>& data) {
    ChannelArray<UIntType> channels;

    UIntType c5_high, c2_high;

    std::tie(channels[7],channels[6],c5_high) = bmp::read<12,12,8>(data[2]);
    std::tie(channels[5],channels[4],channels[3],c2_high) = bmp::read<4,12,12,4>(data[1]);
    std::tie(channels[2],channels[1],channels[0]) = bmp::read<8,12,12>(data[0]);

    channels[5] |= c5_high << 4;
    channels[2] |= c2_high << 8;

    return channels;
}

}
