#pragma once

#include "CalvisionDaq/common/Forward.h"
#include "CalvisionDaq/common/BinaryIO.h"
#include "CaenEnums.h"
#include "CaenError.h"
#include "CalvisionDaq/common/NameConventions.h"

class x742GroupData;
class x742EventData;

/*
 * Calibration data for a single group.
 */
class DRSGroupCalibration {
public:

    DRSGroupCalibration();
    DRSGroupCalibration(const CAEN_DGTZ_DRS4Correction_t& table);

    CAEN_DGTZ_DRS4Correction_t to_table() const;
    void read(BinaryInputFileStream& infile);
    void write(BinaryOutputFileStream& outfile) const;

    // void apply(x742GroupData& data) const;

private:
    ChannelMatrix<FloatingType> channel_offset_;
    SampleArray<FloatingType> trigger_offset_;

    ChannelMatrix<FloatingType> channel_sample_;
    SampleArray<FloatingType> trigger_sample_;

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
    CalibrationTables();

    void read_all(const std::string& calibration_dir);
    void write_all(const std::string& calibration_dir) const;

    void read(const std::string& calibration_dir, CAEN_DGTZ_DRS4Frequency_t frequency);
    void write(const std::string& calibration_dir, CAEN_DGTZ_DRS4Frequency_t frequency) const;

    void load_from_digitizer(int handle);

    const DRSGroupCalibration& table(CAEN_DGTZ_DRS4Frequency_t frequency, UIntType group) const;
    DRSGroupCalibration& table(CAEN_DGTZ_DRS4Frequency_t frequency, UIntType group);

    // void apply(x742EventData& data) const;

private:
    static std::string filename(const std::string& calibration_dir, CAEN_DGTZ_DRS4Frequency_t frequency);

    FrequencyArray<GroupArray<DRSGroupCalibration> > tables_;
};
