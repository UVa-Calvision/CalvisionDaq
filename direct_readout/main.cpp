#include <iostream>
#include "CAENDigitizer.h"

#include "forward.h"
#include "name_conventions.h"

#include "CaenEnums.h"
#include "Digitizer.h"
#include "CaenError.h"
#include "BufferedFileIO.h"

#include <TFile.h>
#include <TTree.h>

#include <chrono>

class RootWriter {
public:
    RootWriter()
    {
        file_ = TFile::Open("file.root", "RECREATE");
        tree_ = new TTree("tree", "DRS Data");
    }

    void setup(CAEN_DGTZ_EventInfo_t* event_info, EventType* event) {
        tree_->Branch("event_counter", &event_info->EventCounter);
        tree_->Branch("time_tag", &event_info->TriggerTimeTag);

        for (int i = 0; i < N_Groups; i++) {
            tree_->Branch(TString(name_timestamp(i)), &event->DataGroup[i].TriggerTimeTag);
            for (int j = 0; j < N_Channels; j++) {
                tree_->Branch(TString(name_channel(i,j)), event->DataGroup[i].DataChannel[j], TString(name_channel(i,j)) + "[1024]/F");
            }
        }
    }

    void handle_event(const CAEN_DGTZ_EventInfo_t& event_info, const EventType& event) {
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


int main(void) {

    try {

        Digitizer digi;
        digi.setup();

        BufferedFileWriter buffered_io;
        digi.set_event_callback([&buffered_io] (const char* data, UIntType count) {
                    buffered_io.write((const BufferedType*) data, count * sizeof(char) / sizeof(BufferedType));
                });
        std::cout << "BufferSize: " << BufferSize << "\n";

        // RootWriter root_io;
        // root_io.setup(digi.event_info_ptr(), digi.decoded_event_ptr());
        // digi.set_event_callback([&root_io] (const CAEN_DGTZ_EventInfo_t& t, const EventType& e) {
        //             root_io.handle_event(t, e);
        //         });

        digi.print();

        digi.begin_acquisition();

        UIntType n_times_buffer_full = 0;
        UIntType n_times_buffer_full_and_not_ready = 0;

        while (digi.running() && digi.num_events_read() < 3000) {
            digi.query_status();

            if (digi.buffer_full()) {
                n_times_buffer_full++;
                // std::cout << "Onboard buffer full, possibly dropping events?\n";
                if (!digi.ready()) {
                    n_times_buffer_full_and_not_ready++;
                }
            }

            if (digi.ready() /*&& digi.buffer_full()*/) {

                // const auto start = std::chrono::steady_clock::now();

                digi.read();

                // const auto stop = std::chrono::steady_clock::now();
                // using namespace std::literals;
                // std::cout << "Reading and saving buffer took " << (stop - start) / 1us << " us\n";
            }
        }
        
        std::cout << "Stopping acquisition...\n";
        digi.end_acquisition();
        std::cout << "    done.\n";

        std::cout << "Number of full buffer reads: " << n_times_buffer_full << "\n"
            << "Number of full but not ready: " << n_times_buffer_full_and_not_ready << "\n";

        std::cout << "Writing tree...\n";
        buffered_io.close();
        // root_io.write();
        std::cout << "    done.\n";

        std::cout << "Resetting digitizer...\n";
        digi.reset();
        std::cout << "    done.\n";

    } catch (CaenError error) {
        error.print_error();
    }

    return 0;
}
