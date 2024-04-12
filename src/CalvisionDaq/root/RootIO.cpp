#include "RootIO.h"

#include "CalvisionDaq/digitizer/Frequency.h"
#include <cblas.h>


// constexpr data for adc -> mV conversion
struct AdcConversion {
    constexpr static UIntType adc_size = 0x1000;
    constexpr static FloatingType adc_scale = 1000.0 / adc_size;

    constexpr AdcConversion()
        : offset()
    {
        FrequencyArray<FloatingType> periods;
        for (const auto freq : DRS4FrequencyIndexer::values) {
            periods[static_cast<UIntType>(freq)] = *FrequencyTable.get<FrequencyValue::SamplingPeriod>(freq);
        }

        for (UIntType i = 0; i < N_Samples; i++) {
            offset[i] = -adc_scale * adc_size / 2;

            for (UIntType f = 0; f < periods.size(); f++) {
                times[f][i] = i * periods[f];
            }
        }
    }

    SampleArray<FloatingType> offset;
    FrequencyArray<SampleArray<FloatingType> > times;
};




FloatingType adc_to_voltage(FloatingType adc) {
    const UIntType adc_size = 0x1000;
    return 1000.0 * (adc - (adc_size / 2)) / adc_size;
}

RootWriter::RootWriter(const std::string& filename)
{
    file_ = TFile::Open(filename.c_str(), "RECREATE");
    file_->SetCompressionLevel(0);   // no compression for fastest writes
    tree_ = new TTree("tree", "DRS Data");
    tree_->SetMaxTreeSize(5'000'000'000LL);

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
    tree_->Branch("horizontal_offset", &horizontal_offset_, "horizontal_offset/F");
    tree_->Branch("event", &event.event_counter, "event/I");
    tree_->Branch("trigger_time", &event.group_data[0].trigger_time, "trigger_time/F");
    tree_->Branch("trigger_offset", &trigger_offset_, "trigger_offset/F");
    tree_->Branch("time", time_, "time[1024]/F");
    tree_->Branch("samples", &samples_, "samples/I");
    tree_->Branch("channels", channels_, "channels[8][1024]/F");
}

constexpr static AdcConversion adc_to_mv;


// this should be same event passed to setup!
void RootWriter::handle_event(const x742EventData& event) {
    // for (UIntType i = 0; i < N_Samples; i++) {
    //     time_[i] = i * event.group_data[0].sample_period;

    //     for (UIntType j = 0; j < N_Channels; j++) {
    //         channels_[j][i] = adc_to_voltage(event.group_data[0].channel_data[j][i]);
    //     }
    // }

    cblas_scopy(N_Samples, adc_to_mv.times[event.group_data[0].frequency].data(), 1, time_, 1);
    for (UIntType i = 0; i < N_Channels; i++) {
        cblas_scopy(N_Samples, adc_to_mv.offset.data(), 1, channels_[i], 1);
        cblas_saxpy(N_Samples, adc_to_mv.adc_scale, event.group_data[0].channel_data[i].data(), 1, channels_[i], 1);
    }

    tree_->Fill();
}

void RootWriter::write() {
    tree_->Write();
    file_->Close();

    tree_ = nullptr;
    file_ = nullptr;
}
