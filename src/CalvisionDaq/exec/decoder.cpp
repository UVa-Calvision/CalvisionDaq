#include "CalvisionDaq/common/AsyncInput.h"
#include "CalvisionDaq/digitizer/Decoder.h"
#include "CalvisionDaq/root/RootIO.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " raw_output.dat decoded_output.root\n";
        return 1;
    }

    std::string raw_input_name(argv[1]);
    std::string decoded_output_name(argv[2]);

    BinaryInputFileStream input(raw_input_name);

    Decoder decoder;

    RootWriter root_io(decoded_output_name);
    root_io.setup(decoder.event());

    listen_for_key interrupt_thread('q');

    while (!interrupt_thread.hit()) {
        while (input) {
            decoder.read_event(input);
            decoder.apply_corrections();
            root_io.handle_event(decoder.event());
        }
    }

    root_io.write();

    return 0;
}
