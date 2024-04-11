#include "CalvisionDaq/digitizer/Decoder.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " raw_output.dat triggerFrequency\n";
        return 1;
    }

    std::string raw_input_name(argv[1]);
    double trigger_rate = std::stod(std::string(argv[2]));

    BinaryInputFileStream input(raw_input_name);

    Decoder decoder;

    std::vector<double> trigger_times;

    while (input) {
        decoder.read_event(input);

        const x742EventData& event = decoder.event();
        trigger_times.push_back(event.group_data[0].trigger_time);
    }

    const double tolerance = 0.1;
    // const double trigger_rate = 1e3;                     // in Hz
    const double trigger_period = 1e9 / trigger_rate;    // ns

    for (size_t i = 1; i < trigger_times.size(); i++) {
        double diff = trigger_times[i] - trigger_times[i-1];

        if (std::abs(diff - trigger_period) > tolerance * trigger_period) {
            std::cout << "At i = " << i << " there was a dropped event:\n"
                << "\tPrevious event time: " << trigger_times[i-1] << "\n"
                << "\tNext     event time: " << trigger_times[i  ] << "\n"
                << "\tDifference: " << diff << "\n"
                << "\tExpected: " << trigger_period << "\n";
        }
    }


    return 0;
}
