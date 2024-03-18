#include "RootIO.h"

FloatingType adc_to_voltage(UIntType adc) {
    const UIntType adc_size = 0x1000;
    return 1000.0 * (static_cast<FloatingType>(adc) - (adc_size / 2)) / adc_size;
}

RootWriter::RootWriter(const std::string& filename)
{
    file_ = TFile::Open(filename.c_str(), "RECREATE");
    tree_ = new TTree("tree", "DRS Data");

    for (UIntType i = 0; i < N_Channels; i++) {
        vertical_gain_[i] = 1.0;
        vertical_offset_[i] = 0.0;
    }

    samples_ = N_Samples;

    // TODO: fix
    horizontal_offset_ = 0;
    trigger_offset_ = 0;
}

void RootWriter::setup(x742EventData& event) {
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
    tree_->Branch("channels", channels_, "channels[8][1024]/F");
}

// this should be same event passed to setup!
void RootWriter::handle_event(const x742EventData& event) {
    for (UIntType i = 0; i < N_Samples; i++) {
        time_[i] = i * event.group_data[0].sample_period;

        for (UIntType j = 0; j < N_Channels; j++) {
            channels_[j][i] = adc_to_voltage(event.group_data[0].channel_data[j][i]);
        }
    }
    tree_->Fill();
}

void RootWriter::write() {
    tree_->Write();
    file_->Close();
}
