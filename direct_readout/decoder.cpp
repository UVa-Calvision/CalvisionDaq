#include "binary_io.h"

#include "bitmanip.h"
#include "X742_Data.h"
#include "Calibration.h"
#include "name_conventions.h"

#include <iostream>
#include <array>
#include <fstream>

template <UIntType N, typename FuncType>
void read_channels(const std::array<UIntType, 3 * N>& raw, const FuncType& target) {
    for (UIntType i = 0; i < N; i++) {
        UIntType value = raw[3 * i + 0];
        target(i, 0)  = static_cast<float>((value & 0x00000FFF)      );
        target(i, 1)  = static_cast<float>((value & 0x00FFF000) >> 12);
        target(i, 2)  = static_cast<float>((value & 0xFF000000) >> 24);

        value = raw[3 * i + 1];
        target(i, 2) += static_cast<float>((value & 0x0000000F) <<  8);
        target(i, 3)  = static_cast<float>((value & 0x0000FFF0) >>  4);
        target(i, 4)  = static_cast<float>((value & 0x0FFF0000) >> 16);
        target(i, 5)  = static_cast<float>((value & 0xF0000000) >> 28);

        value = raw[3 * i + 2];
        target(i, 5) += static_cast<float>((value & 0x000000FF) <<  4);
        target(i, 6)  = static_cast<float>((value & 0x000FFF00) >>  8);
        target(i, 7)  = static_cast<float>((value & 0xFFF00000) >> 20);
    }
}

class Decoder {
public:
    Decoder()
    {
        calibration_tables_.read_all();
        for (const auto freq : Frequencies) {
            for (UIntType g = 0; g < N_Groups; g++) {
                raw_tables_[static_cast<UIntType>(freq)][g] = calibration_tables_.table(freq, g).to_table();
            }
        }
    }

    void read_event(BinaryInputFileStream& input) {
        // Static buffers
        static std::array<UIntType, 4> header;
        static std::array<UIntType, 3 * N_Samples> channels;
        static std::array<UIntType, 3 * N_Chunks> trigger;

        // Event header
        if (!input.read_buffer(header)) return;

        // const auto& [INIT, TOTAL_EVENT_SIZE] = bmp::read<4,28>(std::get<0>(header));
        const auto& [BOARDID, BF, RES1, PATTERN, RES2, GROUPMASK] = bmp::read<5,1,2,16,6,2>(std::get<1>(header));
        std::tie(std::ignore, event_data_.event_counter) = bmp::read<8,24>(std::get<2>(header));
        event_data_.time_tag = std::get<3>(header);

        // TODO: Check BF

        // Read groups
        for (UIntType g = 0; g < N_Groups; g++) {
            event_data_.group_present[g] = GROUPMASK & (1 << g);
            if (event_data_.group_present[g]) {
                auto& group_data = event_data_.group_data[g];

                UIntType TR, size;
                std::tie(std::ignore,
                         group_data.start_index_cell,
                         std::ignore,
                         group_data.frequency,
                         std::ignore,
                         TR,
                         size)
                    = bmp::read<2,10,2,2,3,1,12>(input.read_int());
                group_data.trigger_digitized = (TR == 1);

                // TODO: check size == 3 * N_Samples

                input.read_buffer(channels);
                read_channels<N_Samples>(channels,
                        [&group_data] (UIntType i, UIntType c) -> float& {
                            return group_data.channel_data[c][i];
                        });

                if (group_data.trigger_digitized) {
                    input.read_buffer(trigger);

                    read_channels<N_Chunks>(trigger,
                            [&group_data] (UIntType i, UIntType c) -> float& {
                                return group_data.trigger_data[N_Channels * i + c];
                            });
                }

                std::tie(std::ignore, group_data.trigger_time_tag) = bmp::read<2,30>(input.read_int());
            }
        }

        std::cout << "Read event: " << event_data_.event_counter << "\n";
    }

    void apply_corrections() {
        for (UIntType g = 0; g < N_Groups; g++) {
            if (event_data_.group_present[g]) {
                auto& group_data = event_data_.group_data[g];
                group_data.ApplyDataCorrection(&raw_tables_[group_data.frequency][g], 0b111);
            }
        }
    }

    x742EventData& event() { return event_data_; }

private:
    int n_events = 0;
    x742EventData event_data_;
    CalibrationTables calibration_tables_;
    FrequencyArray<GroupArray<CAEN_DGTZ_DRS4Correction_t> > raw_tables_;
};


#include <TFile.h>
#include <TTree.h>


class RootWriter {
public:
    RootWriter()
    {
        file_ = TFile::Open("file.root", "RECREATE");
        tree_ = new TTree("tree", "DRS Data");
    }

    void setup(x742EventData& event) {
        tree_->Branch("event_counter", &event.event_counter);
        tree_->Branch("time_tag", &event.time_tag);

        for (int i = 0; i < N_Groups; i++) {
            tree_->Branch(TString(name_timestamp(i)), &event.group_data[i].trigger_time_tag);
            tree_->Branch(TString(name_trigger(i)), event.group_data[i].trigger_data.data(), TString(name_trigger(i)) + "[1024]/F");
            for (int j = 0; j < N_Channels; j++) {
                tree_->Branch(TString(name_channel(i,j)), event.group_data[i].channel_data[j].data(), TString(name_channel(i,j)) + "[1024]/F");
            }
        }
    }

    void handle_event() {
        tree_->Fill();
    }

    void write() {
        tree_->Write();
        file_->Close();
    }

private:
    TTree* tree_;
    TFile* file_;
};


#include "async_input.h"

int main(void) {

    BinaryInputFileStream input("output.dat");

    Decoder decoder;
    // decoder.load_corrections("calibrations.dat");

    RootWriter root_io;
    root_io.setup(decoder.event());

    listen_for_key interrupt_thread('q');

    while (!interrupt_thread.hit()) {
        while (input) {
            decoder.read_event(input);
            decoder.apply_corrections();
            root_io.handle_event();
        }
    }

    root_io.write();

    return 0;
}
