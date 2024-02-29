#include "Digitizer.h"
#include "Calibration.h"

#include <iostream>

int main(void) {
    try {
        Digitizer digi;
        digi.setup();

        std::cout << "Writing calibration tables...\n";

        digi.write_calibration_tables();

        std::cout << "Calibration tables written.\n";

        digi.reset();

    } catch (CaenError error) {
        error.print_error();
    }

    return 0;
}
