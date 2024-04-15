#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/Calibration.h"
#include "CalvisionDaq/digitizer/Staging.h"

#include <iostream>

int main(int argc, char** argv) {

    if (argc != 1) {
        std::cout << "Usage: " << argv[0] << "\n";
        return 1;
    }

    try {
        AllDigitizers digis(std::nullopt);

        for (const auto& ctx : digis.ctxs) {
            std::cout << "Digitizer " << ctx->digi().serial_code() << "\n";
            ctx->digi().setup();
            std::cout << "Writing calibration tables...\n";
            ctx->digi().write_calibration_tables();
            std::cout << "Calibration tables written.\n";
            ctx->digi().reset();
        }
    } catch (const CaenError& error) {
        error.print_error(std::cerr);
    }

    return 0;
}
