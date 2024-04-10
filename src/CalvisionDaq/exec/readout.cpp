#include "CalvisionDaq/common/BufferedFileIO.h"
#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/CaenError.h"


#include <iostream>

volatile bool quit_readout = false;

void listen_for_cin() {
    std::string message;
    while (std::getline(std::cin, message)) {
        std::cout << "Got the message: [" << message << "]\n";
        if (message == "stop") {
            quit_readout = true;
            return;
        }
    }
}



int main(int argc, char** argv) {

    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [config_file] raw_output.dat\n";
        return 1;
    }

    std::string config_file(argv[1]);
    std::string outfile_name(argv[2]);
    
    std::cout << "Opening digitizer\n";
    Digitizer digi(config_file, &std::cout);

    try {
        std::thread listen_thread(&listen_for_cin);

        std::cout << "Opening buffered file writer\n";
        BufferedFileWriter buffered_io(outfile_name);
        digi.set_event_callback([&buffered_io] (const char* data, UIntType count) {
                    buffered_io.write((const BufferedType*) data, count * sizeof(char) / sizeof(BufferedType));
                });

        digi.print();

        std::cout << "Beginning readout\n";
        digi.readout([](const Digitizer& d) { 
            std::cout << "Read " << d.num_events_read() << "\n";
            return !quit_readout;
            /*return d.num_events_read() < 100;*/ });
        std::cout << "Stopped readout\n";

        std::cout << "Writing file...\n";
        buffered_io.close();

        std::cout << "Resetting digitizer...\n";
        digi.reset();

        std::cout << "Done.\n";

        listen_thread.join();

    } catch (CaenError error) {
        error.print_error(std::cerr);
        digi.reset();
    }

    return 0;
}
