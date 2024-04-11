#include "CalvisionDaq/common/BufferedFileIO.h"
#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/CaenError.h"
#include "CalvisionDaq/digitizer/Command.h"

#include <iostream>
#include <fstream>
#include <memory>

volatile bool quit_readout = false;

bool stop_readout(const Digitizer&) {
    return !quit_readout;
}

void main_loop(size_t thread_id, Digitizer* digi, std::ostream* log_out) {

    try {
        digi->print();

        *log_out << "Opening buffered file writer\n";
        BufferedFileWriter buffered_io("outfile_" + std::to_string(thread_id) + ".dat");
        digi->set_event_callback([&buffered_io] (const char* data, UIntType count) {
                buffered_io.write((const BufferedType*) data, count * sizeof(char) / sizeof(BufferedType));
            });

        *log_out << "Beginning readout\n";
        digi->readout([](const Digitizer& d) {
                d.log() << "Read " << d.num_events_read() << "\n";
                return !quit_readout;
            });

        *log_out << "Stopped readout\n";

        *log_out << "Writing file...\n";
        buffered_io.close();

        *log_out << "Resetting digitzer...\n";
        digi->reset();

        *log_out << "Done.\n";

    } catch (const CaenError& error) {
        std::cerr << "[FATAL ERROR]: ";
        error.print_error(std::cerr);
        error.print_error(*log_out);
    }
}


void interrupt_listener() {
    std::string message;
    while (std::getline(std::cin, message)) {
        std::cout << "Got the message: [" << message << "]\n";
        if (message == "stop") {
            quit_readout = true;
            return;
        }
    }
}

constexpr static size_t N_DIGITIZERS = 2;

int main(int argc, char** argv) {

    if (argc != 1 + N_DIGITIZERS) {
        std::cout << "Usage: " << argv[0];

        for (size_t i = 0; i < N_DIGITIZERS; i++) {
            std::cout << " [config " << (i+1) << "]";
        }
        std::cout << "\n";

        return 1;
    }

    std::vector<std::thread> main_threads;
    std::vector<std::unique_ptr<Digitizer> > digis;
    std::vector<std::unique_ptr<std::ostream> > logs;
    for (size_t i = 1; i <= N_DIGITIZERS; i++) {
        logs.push_back(std::make_unique<std::ofstream>("readout_" + std::to_string(i) + ".log"));
        digis.push_back(std::make_unique<Digitizer>(std::string(argv[i]), logs.back().get()));

        main_threads.emplace_back(&main_loop, i, digis.back().get(), logs.back().get());
    }

    std::thread listener(&interrupt_listener);

    for (size_t i = 0; i < main_threads.size(); i++) {
        main_threads[i].join();
    }

    listener.join();

    for (size_t i = 0; i < N_DIGITIZERS; i++) {
        digis[i].reset();
        logs[i].reset();
    }

    return 0;
}
