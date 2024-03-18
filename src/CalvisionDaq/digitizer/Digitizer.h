#pragma once

#include "CAENDigitizer.h"
#include "X742_Data.h"
#include "X742CorrectionRoutines.h"

#include "CalvisionDaq/common/Forward.h"
#include "CaenEnums.h"
#include "CaenError.h"
#include "Calibration.h"

#include <cstdlib>
#include <cstring>
#include <functional>
#include <chrono>

using EventType = CAEN_DGTZ_X742_EVENT_t;

enum TriggerSettings : unsigned int {
        ECL = 0,
        NIM = 1,
        Negative_400mV = 2,
        Negative_200mV = 3,
        Bipolar = 4,
        TTL = 5,
        Positive_2V = 6
};

class Digitizer {
public:
    using CallbackFunc = std::function<void(const char* data, UIntType count)>;

    Digitizer();
    ~Digitizer();

    void setup();
    void reset();
    void write_calibration_tables();

    void print() const;

    void begin_acquisition();
    void end_acquisition();
    void read();

    void query_status();
    bool ready() const;
    bool running() const;
    bool buffer_full() const;
    UIntType num_events_read() const;

    void set_event_callback(const CallbackFunc& event_callback);

    void set_channel_offsets(const ChannelArray<UIntType>& offsets);

    static FloatingType voltage_p2p();

    void set_trigger(TriggerSettings t);


    template <typename PredicateFunc>
    void readout(const PredicateFunc& keep_running) {
        begin_acquisition();

        while (running() && keep_running(*this)) {
            query_status();

            if (ready()) {
                read();
            }
        }

        end_acquisition();
    }

private:
    int handle_; 
    CAEN_DGTZ_BoardInfo_t board_info_;

    char* readout_buffer_;
    UIntType readout_size_;

    CAEN_DGTZ_DRS4Frequency_t frequency_;
    GroupArray<CAEN_DGTZ_DRS4Correction_t> correction_table_;


    CallbackFunc event_callback_;

    // Acquisition statuses
    bool running_, ready_, buffer_full_;
    UIntType num_events_read_;
};
