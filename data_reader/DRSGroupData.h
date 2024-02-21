#ifndef __DRSGROUPDATA__
#define __DRSGROUPDATA__ 1

#include <TString.h>
#include <array>
#include <optional>
#include "common.h"

class DRSGroupData {
public:
    DRSGroupData();

    // Getters
    UIntType GetTC() {return fTC;}
    UIntType GetFR() {return fFR;}

    // Calibration
    void LoadCalibrations(TString file1, TString file2, TString file3);

    // Setters
    void SetTriggerCell(UIntType val);
    void SetFrequency(UIntType val);
    void SetTimeStamp(UIntType val) {fTimeStamp = val;}

    void SetChannelData(int isa, const ChannelArray<UIntType>& values) {
        for (int ich = 0; ich < N_Channels; ich++) {
            fChannel[ich][isa] = values[ich];
        }
    }

    void SetTRData(int chunk, const ChannelArray<UIntType>& values) {
        for (int i = 0; i < N_Channels; i++) {
            fTR[chunk * N_Channels + i] = values[i];
        }
    }

    // Corrected readout values
    void sample_timing(SampleArray<FloatingType>& time_buffer) const;
    void channel_value(int channel, SampleArray<FloatingType>& channel_buffer) const;
    void trigger_value(SampleArray<FloatingType>& trigger_buffer) const;

    // Event timestep in us
    FloatingType timestamp() const { return 8.5e-3 * static_cast<FloatingType>(fTimeStamp); }

protected:
    // Cell index for a given sample
    int cell_index(int sample_index) const {
        return (sample_index + fTC) % N_Samples;
    }

    // Converts ADC value to mV
    FloatingType convert_adc(Int_t value) const {
        return 1000.0 * (static_cast<FloatingType>(value) / static_cast<FloatingType>(0xFFF) - 0.5);
    }

private:
    // Sampling frequency of DRS4 chip
    UIntType fFR; // 0: 5 GS/s   1: 2.5 GS/s   2: 1 GS/s   3: 750 MS/s

    // Starting index of first sample of the event
    UIntType fTC;

    // Sampling of channel values for channels 0-7 (group 0) or 8-15 (group 2)
    ChannelMatrix<UIntType> fChannel;

    // Sampling of trigger values
    SampleArray<UIntType> fTR;

    // Group trigger time tag (physical time of pulse arrival)
    UIntType fTimeStamp;

    // calibration
    Bool_t fApplyOfflineCalibration;
    DRSGroupCalibration fCalibration;
};

#endif
