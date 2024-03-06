#include "DRSGroupData.h"
#include <fstream>
#include <TString.h>

//====================================
DRSGroupData::DRSGroupData() {
    fFR = 0;
    fTC = 0;

    fApplyOfflineCalibration = kTRUE;
    // fApplyOfflineCalibration = kFALSE;
}

//====================================
void DRSGroupData::SetFrequency(UIntType val) {
    fFR = val;
}

void DRSGroupData::SetTriggerCell(UIntType val) {
    fTC = val;
}

//====================================
void DRSGroupData::LoadCalibrations(const std::string& filename) {
    fCalibration.read(filename);
}

//====================================
void DRSGroupData::sample_timing(SampleArray<Double_t>& time_buffer) const {

    // double dt = 0.2;
    // if (fFR == 1) dt = 0.4;
    // if (fFR == 2) dt = 1.0;
    // if (fFR == 3) dt = 4.0 / 3.0;

    // time_buffer[0] = 0;
    // for (int i = 1; i < time_buffer.size(); i++) {
    //     time_buffer[i] = time_buffer[i-1] + dt;
    // }

    double time_offset = fCalibration.timing_correction[fTC];
    for (int i = 0; i < N_Samples; i++) {
        int cell = cell_index(i);
        if (cell == 0) {
            time_offset = fCalibration.timing_correction[(fTC + N_Samples - 1) % N_Samples]
                          - (fCalibration.timing_correction[N_Samples-1] - fCalibration.timing_correction[0]);
        }

        time_buffer[i] = fCalibration.timing_correction[cell] - time_offset;
    }
}

//====================================
void DRSGroupData::channel_value(int channel, SampleArray<Double_t>& channel_buffer) const {
    for(int i = 0; i < N_Samples; ++i) {
        Int_t adc = fChannel[channel][i];
        if(fApplyOfflineCalibration) {
            // subtracting measured cell offset
            adc -= fCalibration.channel_offset[channel][cell_index(i)]; 
        }
        channel_buffer[i] = convert_adc(adc);
    }
}

//====================================
void DRSGroupData::trigger_value(SampleArray<Double_t>& trigger_buffer) const {
    for(int i = 0; i < N_Samples; ++i) {
        Int_t adc = fTR[i];
        if (fApplyOfflineCalibration) {
            adc -= fCalibration.trigger_offset[cell_index(i)];
        }
        trigger_buffer[i] = convert_adc(adc);
    }
}
