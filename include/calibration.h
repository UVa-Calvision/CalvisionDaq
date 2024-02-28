#pragma once

#include "forward.h"
#include "binary_io.h"

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
