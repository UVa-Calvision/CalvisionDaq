#pragma once

#include <TFile.h>
#include <TTree.h>

#include "X742_Data.h"

FloatingType adc_to_voltage(UIntType adc);

class RootWriter {
public:
    RootWriter();

    void setup(x742EventData& event);

    // this should be same event passed to setup!
    void handle_event(const x742EventData& event);

    void write();

private:
    TTree* tree_;
    TFile* file_;

    Float_t vertical_gain_[N_Channels], vertical_offset_[N_Channels];
    Double_t horizontal_offset_, trigger_offset_, time_[N_Samples];
    Int_t samples_;
    Float_t channels_[N_Channels][N_Samples];
};
