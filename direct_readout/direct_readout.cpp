#include "forward.h"
#include "CaenEnums.h"
#include "Digitizer.h"
#include "CaenError.h"

#include "BufferedFileIO.h"

#include <iostream>

int main(void) {

    try {

        Digitizer digi;
        digi.setup();

        BufferedFileWriter buffered_io;
        digi.set_event_callback([&buffered_io] (const char* data, UIntType count) {
                    buffered_io.write((const BufferedType*) data, count * sizeof(char) / sizeof(BufferedType));
                });
        std::cout << "BufferSize: " << BufferSize << "\n";

        digi.print();

        digi.readout([](const Digitizer& d) { return d.num_events_read() < 10000; });

        std::cout << "Writing tree...\n";
        buffered_io.close();
        std::cout << "    done.\n";

        std::cout << "Resetting digitizer...\n";
        digi.reset();
        std::cout << "    done.\n";

    } catch (CaenError error) {
        error.print_error();
    }

    return 0;
}
