#include "Calibration.h"

DRSGroupCalibration::DRSGroupCalibration()
{}

DRSGroupCalibration::DRSGroupCalibration(const CAEN_DGTZ_DRS4Correction_t& table) {
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

CAEN_DGTZ_DRS4Correction_t DRSGroupCalibration::to_table() const {
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
    return table;
}

void DRSGroupCalibration::read(BinaryInputFileStream& infile) {
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

void DRSGroupCalibration::write(BinaryOutputFileStream& outfile) const {
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




CalibrationTables::CalibrationTables()
{}

void CalibrationTables::read_all() {
    for (const auto freq : Frequencies) {
        read(freq);
    }
}

void CalibrationTables::write_all() const {
    for (const auto freq : Frequencies) {
        write(freq);
    }
}

void CalibrationTables::read(CAEN_DGTZ_DRS4Frequency_t frequency) {
    BinaryInputFileStream in(CalibrationTables::filename(frequency));
    for (auto&& group : tables_[static_cast<UIntType>(frequency)]) {
        group.read(in);
    }
}

void CalibrationTables::write(CAEN_DGTZ_DRS4Frequency_t frequency) const {
    BinaryOutputFileStream out(CalibrationTables::filename(frequency));
    for (const auto& group : tables_[static_cast<UIntType>(frequency)]) {
        group.write(out);
    }
}

void CalibrationTables::load_from_digitizer(int handle) {
    GroupArray<CAEN_DGTZ_DRS4Correction_t> correction_table;
    for (const auto frequency : Frequencies) {
        check(CAEN_DGTZ_GetCorrectionTables(handle, frequency, correction_table.data()));
        for (UIntType g = 0; g < N_Groups; g++) {
            table(frequency, g) = DRSGroupCalibration(correction_table[g]);
        }
    }
}

const DRSGroupCalibration& CalibrationTables::table(CAEN_DGTZ_DRS4Frequency_t frequency, UIntType group) const {
    return tables_[static_cast<UIntType>(frequency)][group];
}

DRSGroupCalibration& CalibrationTables::table(CAEN_DGTZ_DRS4Frequency_t frequency, UIntType group) {
    return tables_[static_cast<UIntType>(frequency)][group];
}

std::string CalibrationTables::filename(CAEN_DGTZ_DRS4Frequency_t frequency) {
    return "calibration_" + DRS4Frequency_to_string(frequency) + ".dat";
}

