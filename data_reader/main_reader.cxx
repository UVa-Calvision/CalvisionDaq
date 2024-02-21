#include "x742RawReader.h"
#include <iostream>
#include <memory>

#include "TFile.h"
#include "TTree.h"

#include "common.h"

struct EventBuffer {

    UIntType event_counter, time_tag;
    GroupArray<DRSGroupData> data;

    GroupArray<FloatingType> timestamps;
    GroupArray<SampleArray<FloatingType> > times;
    GroupArray<SampleArray<FloatingType> > triggers;
    GroupArray<ChannelMatrix<FloatingType> > channels;

    void create_branches(TTree* tree) {
        tree->Branch("event_counter", &event_counter);
        tree->Branch("time_tag", &time_tag);
        // tree->Branch("drs_data", &data[0], N_Groups);

        for (int i = 0; i < N_Groups; i++) {
            tree->Branch(TString(name_time(i)), times[i].data(), TString(name_time(i)) + "[1024]/D");
            tree->Branch(TString(name_trigger(i)), triggers[i].data(), TString(name_trigger(i)) + "[1024]/D");
            tree->Branch(TString(name_timestamp(i)), &timestamps[i]);
            for (int j = 0; j < N_Channels; j++) {
                tree->Branch(TString(name_channel(i,j)), channels[i][j].data(), TString(name_channel(i,j)) + "[1024]/D");
            }
        }
    }

    bool read_event(x742RawReader& reader) {
        if (reader.ReadEvent()) {
            event_counter = reader.GetEventNumber();
            time_tag = reader.GetTimeTag();

            for (int i = 0; i < N_Groups; i++) {
                data[i].sample_timing(times[i]);
                data[i].trigger_value(triggers[i]);
                timestamps[i] = data[i].timestamp();
                for (int j = 0; j < N_Channels; j++) {
                    data[i].channel_value(j, channels[i][j]);
                }
            }

            return true;
        }

        return false;
    }
};

TString table_name(int group, const std::string& name) {
    return TString(("../calib/29622/Tables_gr" + std::to_string(group) + "_" + name + ".txt").c_str());
}


int main(void) {

    x742RawReader reader("../daq/output.dat");

    EventBuffer buffer;
    for (int i = 0; i < N_Groups; i++) {
        reader.SetGroupData(i, &buffer.data[i]);
        buffer.data[i].LoadCalibrations(table_name(i, "cell"), table_name(i, "sample"), table_name(i, "time"));
    }

    std::unique_ptr<TFile> myFile(TFile::Open("file.root", "RECREATE"));
    auto tree = std::make_unique<TTree>("tree", "DRS Data");

    buffer.create_branches(tree.get());


    int count = 0;

    std::cout << "Reading data...\n";
    while (buffer.read_event(reader)) {
        std::cout << "Event counter: " << reader.GetEventNumber() << ", "
                  << "Time Tag: " << reader.GetTimeTag() << "\n";

        tree->Fill();
    }

    tree->Write();
    
    std::cout << "Done\n";


    return 0;
}
