#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/CaenError.h"

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " [config_file]\n";
        return 1;
    }

    std::string config_file(argv[1]);

    try {

        std::cout << "Opening digitizer\n";
        Digitizer digi(config_file, &std::cout);
        digi.reset();
        
    } catch (CaenError error) {
        error.print_error(std::cerr);
    }

    return 0;
}
