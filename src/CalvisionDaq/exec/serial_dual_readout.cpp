#include "CalvisionDaq/common/BufferedFileIO.h"
#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/CaenError.h"
#include "CalvisionDaq/digitizer/Command.h"

#include <iostream>
#include <fstream>
#include <memory>

volatile bool quit_readout = false;

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

class DigitizerContext {
public:
    DigitizerContext(size_t i, const std::string& config_file)
        : log_(std::make_unique<std::ofstream>("readout_" + std::to_string(i) + ".log")),
          digi_(std::make_unique<Digitizer>(config_file, log_.get())),
          buffered_io_(std::make_unique<BufferedFileWriter>("outfile_" + std::to_string(i) + ".dat"))
    {
        digi_->print();
        digi_->set_event_callback([writer = buffered_io_.get()] (const char* data, UIntType count) {
                writer->write((const BufferedType*) data, count * sizeof(char) / sizeof(BufferedType));
            });
    }

    ~DigitizerContext() {
        buffered_io_->close();
        buffered_io_.reset();
        digi_.reset();
        log_.reset();
    }

    Digitizer& digi() { return *digi_; }

private:
    std::unique_ptr<std::ofstream> log_;
    std::unique_ptr<Digitizer> digi_;
    std::unique_ptr<BufferedFileWriter> buffered_io_;
};

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

    try {

    std::vector<std::unique_ptr<DigitizerContext> > contexts;
    for (size_t i = 1; i <= N_DIGITIZERS; i++) {
        contexts.emplace_back(std::make_unique<DigitizerContext>(i, std::string(argv[i])));
    }
    
    std::thread listener(&interrupt_listener);

    std::cout << "Starting acquisition\n";
    for (size_t i = 0; i < N_DIGITIZERS; i++) {
        contexts[i]->digi().begin_acquisition();
    }

    while (!quit_readout) {
        for (size_t i = 0; i < N_DIGITIZERS; i++) {
            contexts[i]->digi().read();
            std::cout << "Read events: " << contexts[i]->digi().num_events_read() << "\n";
        }
    }

    std::cout << "Stopping acquisition\n";
    for (size_t i = 0; i < N_DIGITIZERS; i++) {
        contexts[i]->digi().end_acquisition();
    }


    

    listener.join();

    } catch(const CaenError& error) {
        std::cerr << "[FATAL ERROR] ";
        error.print_error(std::cout);
    }

    return 0;
}
