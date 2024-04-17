#pragma once

#include "CalvisionDaq/digitizer/X742_Data.h"

#include <TFile.h>
#include <TTree.h>
#include <string>

FloatingType adc_to_voltage(UIntType adc);

class RootWriter {
public:
    RootWriter(const std::string& filename);

    void setup(x742EventData& event);

    // this should be same event passed to setup!
    void handle_event(const x742EventData& event);

    void write();

    void dump_last_event(const std::string& filename);

private:
    TTree* tree_;
    TFile* file_;

    constexpr static UIntType N_Total_Channels = N_Groups * N_Channels;

    Int_t samples_;
    Float_t vertical_gain_[N_Total_Channels], vertical_offset_[N_Total_Channels];
    Double_t horizontal_offset_, trigger_offset_, time_[N_Samples];
    Float_t channels_[N_Total_Channels][N_Samples];
    Float_t trigger_[N_Groups][N_Samples];
    Bool_t channel_digitized_[N_Total_Channels], trigger_digitized_[N_Groups];
};
