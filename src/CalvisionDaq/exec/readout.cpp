#include "CalvisionDaq/common/BufferedFileIO.h"
#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/CaenError.h"


#include <iostream>

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " raw_output.dat\n";
        return 1;
    }

    std::string outfile_name(argv[1]);

    try {

        std::cout << "Opening digitizer\n";
        Digitizer digi;

        std::cout << "Setting up digitizer\n";
        digi.setup();

        std::cout << "Opening buffered file writer\n";
        BufferedFileWriter buffered_io(outfile_name);
        digi.set_event_callback([&buffered_io] (const char* data, UIntType count) {
                    buffered_io.write((const BufferedType*) data, count * sizeof(char) / sizeof(BufferedType));
                });

        digi.print();

        std::cout << "Beginning readout\n";
        digi.readout([](const Digitizer& d) { return d.num_events_read() < 10000; });
        std::cout << "Stopped readout\n";

        std::cout << "Writing file...\n";
        buffered_io.close();

        std::cout << "Resetting digitizer...\n";
        digi.reset();

        std::cout << "Done.\n";

    } catch (CaenError error) {
        error.print_error();
    }

    return 0;
}
