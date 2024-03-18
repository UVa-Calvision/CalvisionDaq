#pragma once

#include "CalvisionDaq/common/BinaryIO.h"

#include "Calibration.h"
#include "X742_Data.h"

class Decoder {
public:
    Decoder();
    
    void read_event(BinaryInputFileStream& input);
    void apply_corrections();

    x742EventData& event();

private:
    int n_events = 0;
    x742EventData event_data_;
    CalibrationTables calibration_tables_;
    FrequencyArray<GroupArray<CAEN_DGTZ_DRS4Correction_t> > raw_tables_;
};


