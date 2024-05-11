#include "CalvisionDaq/digitizer/Digitizer.h"
#include "CalvisionDaq/digitizer/CaenError.h"
#include "CalvisionDaq/digitizer/Decoder.h"
#include "CalvisionDaq/digitizer/Staging.h"
#include "CalvisionDaq/common/BinaryIO.h"
#include "CalvisionDaq/root/Sampler.h"

#include <iostream>

int main(int argc, char** argv) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [lg config]\n";
        return 1;
    }

    const std::string config_path(argv[1]);

    try {
        AllDigitizers digis{std::nullopt};

        if (!digis.get<DigiMap::LG>()) {
            std::cerr << "[ERROR]: Low gain digitizer not found... exiting...\n";
            return 1;
        }

        auto& ctx = *digis.get<DigiMap::LG>();
        ctx.digi().load_config(config_path);
        ctx.digi().print();

        Decoder decoder(ctx.serial_code());
        RootSampler root_io;

        ctx.digi().set_event_callback([&decoder, &root_io] (const char* data, UIntType event_size, UIntType num_events) {
                for (UIntType i = 0; i < num_events; i++) {
                    BinaryInputBufferStream input(data, event_size);
                    decoder.read_event(input);
                    decoder.apply_corrections();
                    root_io.add_sample(decoder.event().group_data[0].trigger_time_tag, decoder.event().group_data[0].channel_data[0]);
                    data += event_size;
                }
            });

        std::cout << "Beginning readout\n";
        ctx.digi().readout([] (const Digitizer& d) {
                std::cout << "Events read so far: " << d.num_events_read() << "\n";
                return d.num_events_read() < 50'000;
            });
        std::cout << "Stopped readout\n";

        std::cout << "Writing to file\n";
        root_io.draw("optical_test.pdf");

        std::cout << "Resetting digitzer...\n";
        ctx.digi().reset();

        std::cout << "Done.\n";


    } catch(const CaenError& error) {
        std::cerr << "[FATAL ERROR]: ";
        error.print_error(std::cerr);
    }

    return 0;
}
