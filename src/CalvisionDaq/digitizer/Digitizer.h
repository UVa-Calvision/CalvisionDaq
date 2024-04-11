#pragma once

#include "CAENDigitizer.h"
#include "X742_Data.h"

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

    Digitizer(const std::string& config_file, std::ostream* log_out);
    ~Digitizer();

    std::ostream& log() const {
        return *log_;
    }

    void open(CAEN_DGTZ_ConnectionType link, UIntType device_id);

    void setup();
    void reset();
    void write_calibration_tables(const std::string& calibration_dir);

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

        auto start = std::chrono::high_resolution_clock::now();

        while (/*running() &&*/ keep_running(*this)) {

            auto intermediate = std::chrono::high_resolution_clock::now();

            read();

            auto finish = std::chrono::high_resolution_clock::now();
            auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
            log() << "Keep running time: " << std::chrono::duration_cast<std::chrono::microseconds>(intermediate - start).count() << " us\n";
            log() << "Read time: " << std::chrono::duration_cast<std::chrono::microseconds>(finish - intermediate).count() << " us\n";
            start = std::chrono::high_resolution_clock::now();

            // query_status();

            // if (ready()) {
            //     read();
            // }
        }

        end_acquisition();
    }

    int handle() const { return handle_; }

private:
    int handle_; 
    CAEN_DGTZ_BoardInfo_t board_info_;

    char* readout_buffer_;
    UIntType readout_size_;

    GroupArray<CAEN_DGTZ_DRS4Correction_t> correction_table_;


    CallbackFunc event_callback_;

    // Acquisition statuses
    bool running_, ready_, buffer_full_;
    UIntType num_events_read_;

    std::ostream* log_;
};
