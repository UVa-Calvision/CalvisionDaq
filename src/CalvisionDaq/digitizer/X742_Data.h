#pragma once

#include "CaenError.h"
#include "CalvisionDaq/common/Forward.h"
#include "CalvisionDaq/common/Bitmanip.h"

// CAEN_DGTZ_ErrorCode decode(UIntType*& buffer, CAEN_DGTZ_X742_GROUP_t& group);
// CAEN_DGTZ_ErrorCode decode(UIntType* buffer, CAEN_DGTZ_X742_EVENT_t& event);

// static constexpr std::array<FloatingType, 4> sampling_period = {
//     1.0 / 5.0,
//     1.0 / 2.5,
//     1.0 / 1.0,
//     1.0 / 0.750
// };

struct x742GroupData {
    ChannelMatrix<FloatingType> channel_data;
    SampleArray<FloatingType> trigger_data;
    UIntType trigger_time_tag;
    UIntType start_index_cell;
    UIntType frequency;
    bool trigger_digitized;

    double trigger_time;
    FloatingType sample_period;

    void ApplyDataCorrection(CAEN_DGTZ_DRS4Correction_t*, int);
    void PeakCorrection();

    float& raw_data(UIntType c, UIntType i) {
        if (c < N_Channels) return channel_data[c][i];
        return trigger_data[i];
    }
};

struct x742EventData {
    GroupArray<bool> group_present;
    GroupArray<x742GroupData> group_data;
    UIntType event_counter, time_tag;
};
