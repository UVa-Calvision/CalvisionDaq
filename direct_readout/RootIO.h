#pragma once

#include <TFile.h>
#include <TTree.h>

#include "X742_Data.h"

class RootWriter {
public:
    RootWriter()
    {
        file_ = TFile::Open("file.root", "RECREATE");
        tree_ = new TTree("tree", "DRS Data");

        for (int i = 0; i < N_Channels; i++) {
            vertical_gain_[i] = 1.0 / 0x0FFF;
            vertical_offset_[i] = 0;
        }

	samples_ = N_Samples;

        // TODO: fix
        horizontal_offset_ = 0;
        trigger_offset_ = 0;
    }

    void setup(x742EventData& event) {
        // Using group 0 only
        tree_->Branch("vertical_gain", vertical_gain_, "vertical_gain[8]/F");
        tree_->Branch("vertical_offset", vertical_offset_, "vertical_offset[8]/F");
        tree_->Branch("horizontal_interval", &event.group_data[0].sample_period, "horizontal_interval/F");
        tree_->Branch("horizontal_offset", &horizontal_offset_, "horizontal_offset/D");
        tree_->Branch("event", &event.event_counter, "event/I");
        tree_->Branch("trigger_time", &event.group_data[0].trigger_time, "trigger_time/D");
        tree_->Branch("trigger_offset", &trigger_offset_, "trigger_offset/D");
        tree_->Branch("time", time_, "time[1024]/D");
        tree_->Branch("samples", &samples_, "samples/I");
        tree_->Branch("channels", channels_, "channels[8][1024]/S");
    }

    // this should be same event passed to setup!
    void handle_event(const x742EventData& event) {
        for (int i = 0; i < N_Samples; i++) {
            time_[i] = i * event.group_data[0].sample_period;

            for (int j = 0; j < N_Channels; j++) {
                channels_[j][i] = static_cast<Short_t>(event.group_data[0].channel_data[j][i] - 0x0800);
            }
        }
        tree_->Fill();
    }

    void write() {
        tree_->Write();
        file_->Close();
    }

private:
    TTree* tree_;
    TFile* file_;

    Float_t vertical_gain_[N_Channels], vertical_offset_[N_Channels];
    Double_t horizontal_offset_, trigger_offset_, time_[N_Samples];
    Int_t samples_;
    Short_t channels_[N_Channels][N_Samples];
};
