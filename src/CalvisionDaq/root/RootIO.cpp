#include "RootIO.h"

#include "CppUtils/io/FileHandle.h"
#include "CppUtils/c_util/CUtil.h"
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
    // Zero all buffers to start
    zero_buffer<N_Total_Channels>(vertical_gain_);
    zero_buffer<N_Total_Channels>(vertical_offset_);
    zero_buffer<N_Samples>(time_);
    zero_buffer<N_Total_Channels>(channel_digitized_);
    zero_buffer<N_Groups>(trigger_digitized_);
    for (UIntType i = 0; i < N_Total_Channels; i++)
        zero_buffer<N_Samples>(channels_[i]);
    for (UIntType i = 0; i < N_Groups; i++)
        zero_buffer<N_Samples>(trigger_[i]);


    file_ = TFile::Open(filename.c_str(), "RECREATE");
    // file_->SetCompressionLevel(0);   // no compression for fastest writes
    tree_ = new TTree("tree", "DRS Data");
    tree_->SetMaxTreeSize(2'000'000'000LL);

    for (UIntType i = 0; i < N_Channels; i++) {
        vertical_gain_[i] = 1.0;
        vertical_offset_[i] = 0.0;
    }

    samples_ = N_Samples;

    // TODO: fix
    horizontal_offset_ = 0;
    trigger_offset_ = 0;
}

#include <sstream>

TString branch_typename(const std::string& name, const std::string& type, const std::vector<UIntType>& array_dims) {
    std::stringstream result;
    result << name;
    for (UIntType s : array_dims) {
        result << "[" << s << "]";
    }
    result << "/" << type;
    return TString(result.str());
}

void RootWriter::setup(x742EventData& event) {
    tree_->Branch("vertical_gain", vertical_gain_, branch_typename("vertical_gain", "F", {N_Total_Channels}));
    tree_->Branch("vertical_offset", vertical_offset_, branch_typename("vertical_offset", "F", {N_Total_Channels}));
    tree_->Branch("horizontal_interval", &event.group_data[0].sample_period, branch_typename("horizontal_interval", "F", {}));
    tree_->Branch("horizontal_offset", &horizontal_offset_, branch_typename("horizontal_offset", "D", {}));
    tree_->Branch("event", &event.event_counter, branch_typename("event", "I", {}));
    tree_->Branch("trigger_time", &event.group_data[0].trigger_time, branch_typename("trigger_time", "D", {}));
    tree_->Branch("trigger_offset", &trigger_offset_, branch_typename("trigger_offset", "D", {}));
    tree_->Branch("time", time_, branch_typename("time", "D", {N_Samples}));
    tree_->Branch("samples", &samples_, branch_typename("samples", "I", {}));
    tree_->Branch("channels", channels_, branch_typename("channels", "F", {N_Total_Channels, N_Samples}));
    tree_->Branch("trigger", trigger_, branch_typename("trigger", "F", {N_Groups, N_Samples}));
    tree_->Branch("channel_digitized", channel_digitized_, branch_typename("channel_digitized", "B", {N_Total_Channels}));
    tree_->Branch("trigger_digitized", trigger_digitized_, branch_typename("trigger_digitized", "B", {N_Groups}));

    for (size_t i = 0; i < N_Total_Channels; i++) {
        std::string name = "channel" + std::to_string(i);
        tree_->Branch(name, channels_[i], branch_typename(name, "F", {N_Samples}));
    }

    for (size_t i = 0; i < N_Groups; i++) {
        std::string name = "trigger" + std::to_string(i);
        tree_->Branch(name, trigger_[i], branch_typename("trigger", 
    }
}

constexpr static AdcConversion adc_to_mv;

void convert_adc_to_mv(const SampleArray<FloatingType>& adc, Float_t* target) {
    cblas_scopy(N_Samples, adc_to_mv.offset.data(), 1, target, 1);
    cblas_saxpy(N_Samples, adc_to_mv.adc_scale, adc.data(), 1, target, 1);
}

// this should be same event passed to setup!
void RootWriter::handle_event(const x742EventData& event) {
    for (UIntType i = 0; i < N_Samples; i++) {
        time_[i] = i * event.group_data[0].sample_period;
    }

    for (UIntType g = 0; g < N_Groups; g++) {
        if (event.group_present[g]) {
            if (event.group_data[g].trigger_digitized) {
                trigger_digitized_[g] = true;
                for (UIntType i = 0; i < N_Samples; i++) {
                    trigger_[g][i] = adc_to_voltage(event.group_data[g].trigger_data[i]);
                }
            } else {
                trigger_digitized_[g] = false;
            }

            for (UIntType c = 0; c < N_Channels; c++) {
                channel_digitized_[g * N_Channels + c] = true;
                for (UIntType i = 0; i < N_Samples; i++) {
                    channels_[g * N_Channels + c][i] = adc_to_voltage(event.group_data[g].channel_data[c][i]);
                }
            }
        } else {
            trigger_digitized_[g] = false;
            for (UIntType c = 0; c < N_Channels; c++) {
                channel_digitized_[g * N_Channels + c] = false;
            }
        }
    }

    // cblas_scopy(N_Samples, adc_to_mv.times[event.group_data[0].frequency].data(), 1, time_, 1);
    // for (UIntType i = 0; i < N_Channels; i++) {
    //     convert_adc_to_mv(event.group_data[0].channel_data[i], channels_[i]);
    // }
    // convert_adc_to_mv(event.group_data[0].trigger_data, trigger_);

    tree_->Fill();
}

void RootWriter::write() {
    tree_->Write();

    file_ = tree_->GetCurrentFile();
    file_->Write();
    file_->Close();

    tree_ = nullptr;
    file_ = nullptr;
}

void RootWriter::dump_last_event(const std::string& filename) {
    FileWriter out(filename, OpenMode::Truncate);

    out.write<Double_t, N_Samples>(time_);
    // First 8 channels are going to SiPM readout signals, other channels will be auxiliary devices
    for (UIntType i = 0; i < N_Channels; i++) {
        out.write<Float_t, N_Samples>(channels_[i]);
    }
}
