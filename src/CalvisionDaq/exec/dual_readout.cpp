#include "CalvisionDaq/common/BufferedFileIO.h"
#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/CaenError.h"
#include "CalvisionDaq/digitizer/Command.h"

#include <iostream>
#include <fstream>

void main_loop(const std::string& config_file, std::ostream* log_out) {
    try {
        Digitizer digi(config_file, log_out);
        digi.print();

        digi.reset();
    } catch (CaenError error) {
        std::cerr << "[FATAL ERROR]: ";
        error.print_error(std::cerr);
        error.print_error(*log_out);
    }
}


int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [config]\n";
        return 1;
    }

    std::string config(argv[1]);

    std::ofstream log("readout.log");

    main_loop(config, &log);

    return 0;
}
