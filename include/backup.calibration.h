#pragma once

#include "forward.h"
#include "binary_io.h"
#include "CaenEnums.h"
#include "CaenError.h"

/*
 * Calibration data for a single group.
 */
class DRSGroupCalibration {
public:

    DRSGroupCalibration()
    {}

    DRSGroupCalibration(const CAEN_DGTZ_DRS4Correction_t& table) {
        for (int i = 0; i < N_Samples; i++) {
            timing_[i] = table.time[i];
            trigger_offset_[i] = table.cell[N_Channels][i];
            trigger_sample_[i] = table.nsample[N_Channels][i];

            for (int j = 0; j < N_Channels; j++) {
                channel_offset_[j][i] = table.cell[j][i];
                channel_sample_[j][i] = table.nsample[j][i];
            }
        }
    }

    CAEN_DGTZ_DRS4Correction_t to_table() const {
        CAEN_DGTZ_DRS4Correction_t table;
        for (int i = 0; i < N_Samples; i++) {
            table.time[i] = timing_[i];
            table.cell[N_Channels][i] = trigger_offset_[i];
            table.nsample[N_Channels][i] = trigger_sample_[i];

            for (int j = 0; j < N_Channels; j++) {
                table.cell[j][i] = channel_offset_[j][i];
                table.nsample[j][i] = channel_sample_[j][i];
            }
        }
    }

    void read(BinaryInputFileStream& infile) {
        for (auto&& channel : channel_offset_) {
            infile.read_buffer(channel);
        }
        infile.read_buffer(trigger_offset_);

        for (auto&& channel : channel_sample_) {
            infile.read_buffer(channel);
        }
        infile.read_buffer(trigger_sample_);

        infile.read_buffer(timing_);
    }

    void write(BinaryOutputFileStream& outfile) const {
        for (const auto& channel : channel_offset_) {
            outfile.write_buffer(channel);
        }
        outfile.write_buffer(trigger_offset_);

        for (const auto& channel : channel_sample_) {
            outfile.write_buffer(channel);
        }
        outfile.write_buffer(trigger_sample_);

        outfile.write_buffer(timing_);
    }

private:
    ChannelMatrix<UIntType> channel_offset_;
    SampleArray<UIntType> trigger_offset_;

    ChannelMatrix<UIntType> channel_sample_;
    SampleArray<UIntType> trigger_sample_;

    SampleArray<FloatingType> timing_;
};


template <typename T>
using FrequencyArray = std::array<T, _CAEN_DGTZ_DRS4_COUNT_>;

constexpr static FrequencyArray<CAEN_DGTZ_DRS4Frequency_t> Frequencies = {
        CAEN_DGTZ_DRS4_5GHz,
        CAEN_DGTZ_DRS4_2_5GHz,
        CAEN_DGTZ_DRS4_1GHz,
        CAEN_DGTZ_DRS4_750MHz
    };

/*
 * Total calibration data for all groups and frequencies
 */
class CalibrationTables {
public:
    CalibrationTables()
    {}

    void read(CAEN_DGTZ_DRS4Frequency_t frequency) {
        BinaryInputFileStream in(CalibrationTables::filename(frequency));
        for (auto&& group : tables_[static_cast<UIntType>(frequency)]) {
            group.read(in);
        }
    }

    void write(CAEN_DGTZ_DRS4Frequency_t frequency) const {
        BinaryOutputFileStream out(CalibrationTables::filename(frequency));
        for (const auto& group : tables_[static_cast<UIntType>(frequency)]) {
            group.write(out);
        }
    }

    void load_from_digitizer(int handle) {
        GroupArray<CAEN_DGTZ_DRS4Correction_t> correction_table;
        for (const auto frequency : Frequencies) {
            check(CAEN_DGTZ_GetCorrectionTables(handle, frequency, correction_table.data()));
            for (UIntType g = 0; g < N_Groups; g++) {
                table(frequency, g) = DRSGroupCalibration(correction_table[g]);
            }
        }
    }

    const DRSGroupCalibration& table(CAEN_DGTZ_DRS4Frequency_t frequency, UIntType group) const {
        return tables[static_cast<UIntType>(frequency)][group];
    }

    DRSGroupCalibration& table(CAEN_DGTZ_DRS4Frequency_t frequency, UIntType group) {
        return tables[static_cast<UIntType>(frequency)][group];
    }

private:
    static std::string filename(CAEN_DGTZ_DRS4Frequency_t frequency) const {
        return canonical_name("calibration", DRS4Frequency_to_string(frequency)) + ".dat";
    }

    std::array<GroupArray<DRSGroupCalibration>, _CAEN_DGTZ_DRS4_COUNT_> tables_;
};
