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
    // using CallbackFunc = std::function<void(const CAEN_DGTZ_EventInfo_t&, const EventType&)>;
    using CallbackFunc = std::function<void(const char* data, UIntType count)>;

    Digitizer()
        : handle_(-1)
        , readout_buffer_(nullptr)
        , readout_size_(0)
        , frequency_(CAEN_DGTZ_DRS4_1GHz)
    {
        // Open digitizer
        check(CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, 0, 0, 0, &handle_));

        // Read board info
        check(CAEN_DGTZ_GetInfo(handle_, &board_info_));

        // Allocate event space
        // allocate_event();
    }

    ~Digitizer()
    {
        if (handle_ >= 0) {
            check(CAEN_DGTZ_CloseDigitizer(handle_));
            handle_ = -1;
            std::cout << "Digitizer closed.\n";
        }

        if (readout_buffer_) {
            check(CAEN_DGTZ_FreeReadoutBuffer(&readout_buffer_));
            readout_buffer_ = nullptr;
            readout_size_ = 0;
        }

        // deallocate_event();
    }

    void reset() {
        check(CAEN_DGTZ_Reset(handle_));
    }

    void setup() {
        reset();

        // check(CAEN_DGTZ_IRQWait(handle_, 10));

        // Use TTL IO
        // TODO: Probably not using IO?
        check(CAEN_DGTZ_SetIOLevel(handle_, CAEN_DGTZ_IOLevel_TTL));

        // Use full 1024 event buffer
        check(CAEN_DGTZ_SetMaxNumEventsBLT(handle_, 1000));

        // 1 GHz Sampling frequency
        check(CAEN_DGTZ_SetDRS4SamplingFrequency(handle_, frequency_));

        // Only using group 1
        check(CAEN_DGTZ_SetGroupEnableMask(handle_, 0b01));

        // ----- Trigger
        // Enable fast trigger TR0
        check(CAEN_DGTZ_SetFastTriggerMode(handle_, CAEN_DGTZ_TRGMODE_ACQ_ONLY));

        // Don't digitize trigger (less hang time)
        check(CAEN_DGTZ_SetFastTriggerDigitizing(handle_, CAEN_DGTZ_DISABLE));
    }

    void print() const;

    void begin_acquisition() {
        // Setup calibration and corrections
        // {
        //     check(CAEN_DGTZ_GetCorrectionTables(handle_, frequency_, correction_table_.data()));
        //     BinaryOutputFileStream outfile("calibration.dat");
        //     for (const auto& table : correction_table_) {
        //         DRSGroupCalibration calibration(table);
        //         calibration.write(outfile);
        //     }
        // }

        // Allocate readout memory
        check(CAEN_DGTZ_MallocReadoutBuffer(handle_, &readout_buffer_, &readout_size_));
        std::cout << "Allocated " << readout_size_ << " bytes in memory for readout\n";

        // Start acquisition
        num_events_read_ = 0;

        check(CAEN_DGTZ_SWStartAcquisition(handle_));
        check(CAEN_DGTZ_ClearData(handle_));
        query_status();
        std::cout << "Acquisition started.\n";
    }

    void write_calibration_tables() {
        CalibrationTables tables;
        tables.load_from_digitizer(handle_);

        for (const auto freq : Frequencies) {
            tables.write(freq);
        }
    }

    void end_acquisition() {
        // Stop acquisition
        check(CAEN_DGTZ_SWStopAcquisition(handle_));
        std::cout << "Acquisition stopped.\n";
    }

    void read() {
        check(CAEN_DGTZ_ReadData(handle_,
                                 CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
                                 readout_buffer_,
                                 &readout_size_));

        UIntType num_events = 0;
        check(CAEN_DGTZ_GetNumEvents(handle_, readout_buffer_, readout_size_, &num_events));

        event_callback_(readout_buffer_, readout_size_);

        // for (UIntType i = 0; i < num_events; i++) {
        //     char* encoded_event_ptr = nullptr;
        //     check(CAEN_DGTZ_GetEventInfo(handle_, readout_buffer_, readout_size_, i, &event_info_, &encoded_event_ptr));

        //     check(decode((UIntType*) encoded_event_ptr, decoded_event_));
        //     for (int g = 0; g < N_Groups; g++) {
        //         if (decoded_event_.GrPresent[g]) {
        //             ApplyDataCorrection(&correction_table_[g], frequency_, 0b111, &decoded_event_.DataGroup[g]);
        //         }
        //     }

        //     event_callback_(event_info_, decoded_event_);
        // }
        num_events_read_ += num_events;

        // std::cout << "Read: " << readout_size_ << " bytes; " << num_events << " events\n";
    }

    void query_status() {
        UIntType status_reg;
        check(CAEN_DGTZ_ReadRegister(handle_, CAEN_DGTZ_ACQ_STATUS_ADD, &status_reg));
        // std::cout << "status: " << std::bitset<32>(status_reg) << "\n";
        running_     = (status_reg & 0b0000'0100) != 0;
        ready_       = (status_reg & 0b0000'1000) != 0;
        buffer_full_ = (status_reg & 0b0001'0000) != 0;
    }

    bool ready() const { return ready_; }
    bool running() const { return running_; }
    bool buffer_full() const { return buffer_full_; }
    UIntType num_events_read() const { return num_events_read_; }

    void set_event_callback(const CallbackFunc& event_callback) {
        event_callback_ = event_callback;
    }

    // CAEN_DGTZ_EventInfo_t* event_info_ptr() { return &event_info_; }
    // EventType* decoded_event_ptr() { return &decoded_event_; }

private:
    int handle_; 

    char* readout_buffer_;
    UIntType readout_size_;

    // CAEN_DGTZ_EventInfo_t event_info_;
    // EventType decoded_event_;
    CAEN_DGTZ_DRS4Frequency_t frequency_;
    GroupArray<CAEN_DGTZ_DRS4Correction_t> correction_table_;

    CAEN_DGTZ_BoardInfo_t board_info_;

    CallbackFunc event_callback_;

    // Acquisition statuses
    bool running_, ready_, buffer_full_;
    UIntType num_events_read_;

    // void allocate_event() {
    //     for (int i = 0; i < MAX_X742_GROUP_SIZE; i++) {
    //         for (int j = 0; j < MAX_X742_CHANNEL_SIZE; j++) {
    //             decoded_event_.DataGroup[i].DataChannel[j] = (float*) malloc(N_Samples * sizeof(float));
    //         }
    //     }
    // }

    // void deallocate_event() {
    //     for (int i = 0; i < MAX_X742_GROUP_SIZE; i++) {
    //         for (int j = 0; j < MAX_X742_CHANNEL_SIZE; j++) {
    //             free(decoded_event_.DataGroup[i].DataChannel[j]);
    //             decoded_event_.DataGroup[i].DataChannel[j] = nullptr;
    //         }
    //     }
    // }
};
