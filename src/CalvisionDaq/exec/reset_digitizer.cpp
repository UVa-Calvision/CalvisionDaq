#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/CaenError.h"
#include "CalvisionDaq/digitizer/Staging.h"

int main(int argc, char** argv) {

    if (argc != 1) {
        std::cout << "Usage: " << argv[0] << "\n";
        return 1;
    }

    try {
        AllDigitizers digis(std::nullopt);

        for (auto& ctx : digis.ctxs) {
            std::cout << "Reseting digitizer " << ctx->serial_code() << "\n";
            ctx->digi().reset();
        }
    } catch (const CaenError& error) {
        error.print_error(std::cerr);
    }

    return 0;
}
