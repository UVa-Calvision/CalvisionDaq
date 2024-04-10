#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/Calibration.h"

#include <iostream>

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " [config_file] [table_location]\n";
        return 1;
    }

    std::string config_file(argv[1]);
    std::string calibration_dir(argv[2]);

    try {
        Digitizer digi(config_file, &std::cout);
        digi.setup();

        std::cout << "Writing calibration tables...\n";

        digi.write_calibration_tables(calibration_dir);

        std::cout << "Calibration tables written.\n";

        digi.reset();

    } catch (CaenError error) {
        error.print_error(std::cerr);
    }

    return 0;
}
