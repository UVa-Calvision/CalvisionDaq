#include "DRSGroupData.h"
#include <fstream>
#include <TString.h>

#include <cstring>

#include <iostream>

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
void DRSGroupData::LoadCalibrations(TString fileCell, TString filePhase, TString fileTime) {
    int ch, ce;
    Double_t offset;
    //===========
    std::ifstream icelloffset( fileCell.Data() );
    for (int i = 0; i < (N_Channels + 1) * N_Samples; i++) {
        icelloffset >> ch >> ce >> offset;

        if (ch == 9)
            fCalibration.trigger_offset[ce] = offset;
        else
            fCalibration.channel_offset[ch][ce] = offset;
    }
    icelloffset.close();

    //===========
    std::ifstream icelltime( fileTime.Data() );
    for(int icell = 0; icell < N_Samples; ++icell) {
        icelltime >> ce >> offset;
        fCalibration.timing_correction[ce] = offset;
    }
    icelltime.close();
}

//====================================
void DRSGroupData::sample_timing(SampleArray<Double_t>& time_buffer) const {
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
