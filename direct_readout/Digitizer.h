#pragma once

#include "CAENDigitizer.h"
#include "X742_Data.h"
#include "X742CorrectionRoutines.h"

#include "CaenEnums.h"
#include "forward.h"
#include "CaenError.h"
#include "Calibration.h"

#include <cstdlib>
#include <cstring>
#include <functional>
#include <bitset>
#include <chrono>

using EventType = CAEN_DGTZ_X742_EVENT_t;

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
