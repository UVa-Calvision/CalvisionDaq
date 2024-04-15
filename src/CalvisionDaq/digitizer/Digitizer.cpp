#include "Digitizer.h"

#include "Command.h"

#include <iostream>

/*
 * 16-bit DAC sets the DC offset.
 */

UIntType DAC_voltage_to_register(float voltage) {
    if (voltage > 0 || voltage < -Digitizer::voltage_p2p()) {
        // std::cout << "[ERROR] DAC voltage outside [-V_pp,0] range: " << voltage << "\n";
    }
    return static_cast<UIntType>(-voltage * static_cast<float>(0xFFFF) / Digitizer::voltage_p2p());
}

float DAC_register_to_voltage(UIntType reg) {
    if (reg > 0xFFFF) {
        // std::cout << "[ERROR] DAC voltage register larger than max size: " << reg << "\n";
    }
    return -Digitizer::voltage_p2p() * static_cast<float>(reg) / static_cast<float>(0xFFFF);
}

float DAC_lower_bound_voltage(UIntType reg) {
    return -Digitizer::voltage_p2p() - DAC_register_to_voltage(reg);
}

float DAC_upper_bound_voltage(UIntType reg) {
    return -DAC_register_to_voltage(reg);
}

float DAC_middle_voltage(UIntType reg) {
    return -0.5 * Digitizer::voltage_p2p() - DAC_register_to_voltage(reg);
}

UIntType DAC_middle_voltage_to_register(float voltage) {
    return DAC_voltage_to_register(-(voltage + 0.5 * Digitizer::voltage_p2p()));
}

Digitizer::Digitizer()
    : handle_(-1)
    , readout_buffer_(nullptr)
    , readout_size_(0)
    , log_(&std::cout)
{}

void Digitizer::load_config(const std::string& config_file)
{
    if (config_file == "") {
        log() << "[WARNING] No config file provided, continuing\n";
    }
    
    std::ifstream config_input(config_file);
    run_setup(config_input, *this);
}

void Digitizer::open(CAEN_DGTZ_ConnectionType link_type, UIntType device_id) {
    check(CAEN_DGTZ_OpenDigitizer(link_type, device_id, 0, 0, &handle_));
    check(CAEN_DGTZ_GetInfo(handle_, &board_info_));
}

Digitizer::~Digitizer()
{
    if (handle_ >= 0) {
        check(CAEN_DGTZ_CloseDigitizer(handle_));
        handle_ = -1;
        log() << "Digitizer closed.\n";
    }

    if (readout_buffer_) {
        check(CAEN_DGTZ_FreeReadoutBuffer(&readout_buffer_));
        readout_buffer_ = nullptr;
        readout_size_ = 0;
    }

    // deallocate_event();
}

void Digitizer::reset() {
    check(CAEN_DGTZ_Reset(handle_));
}

void Digitizer::setup() {
    reset();

    // check(CAEN_DGTZ_IRQWait(handle_, 10));

    // Use TTL IO
    // TODO: Probably not using IO?
    check(CAEN_DGTZ_SetIOLevel(handle_, CAEN_DGTZ_IOLevel_TTL));

    // Use full 1024 event buffer
    check(CAEN_DGTZ_SetMaxNumEventsBLT(handle_, 1000));

    // 1 GHz Sampling frequency
    check(CAEN_DGTZ_SetDRS4SamplingFrequency(handle_, CAEN_DGTZ_DRS4_1GHz));

    // Only using group 1
    check(CAEN_DGTZ_SetGroupEnableMask(handle_, 0b01));

    // ----- Trigger
    // Enable fast trigger TR0
    check(CAEN_DGTZ_SetFastTriggerMode(handle_, CAEN_DGTZ_TRGMODE_ACQ_ONLY));

    // Don't digitize trigger (less hang time)
    check(CAEN_DGTZ_SetFastTriggerDigitizing(handle_, CAEN_DGTZ_ENABLE));

    // Set post trigger size (in %)
    check(CAEN_DGTZ_SetPostTriggerSize(handle_, 50));

    // Set channel offsets to middle scale (0x7FFF)
    // ChannelArray<UIntType> offsets;
    // for (auto& offset : offsets) offset = 0x7FFF /*+ static_cast<UIntType>(7 * 214)*/;
    // set_channel_offsets(offsets);
}

void Digitizer::begin_acquisition() {
    // Allocate readout memory
    check(CAEN_DGTZ_MallocReadoutBuffer(handle_, &readout_buffer_, &readout_size_));
    log() << "Allocated " << readout_size_ << " bytes in memory for readout\n";

    {
        bool fast_trigger = get_fast_trigger_digitizing() == CAEN_DGTZ_ENABLE;
        UIntType group_mask;
        check(CAEN_DGTZ_GetGroupEnableMask(handle_, &group_mask));

        UIntType n_groups = 0;
        if (group_mask & 0b01) n_groups++;
        if (group_mask & 0b10) n_groups++;

        event_size_ = calc_event_size(n_groups, fast_trigger);
        
        log() << "Number of groups enabled: " << n_groups << "\n"
              << "Trigger digitized: " << fast_trigger << "\n"
              << "Event Size: " << event_size_ << "\n";
    }

    // Start acquisition
    num_events_read_ = 0;

    check(CAEN_DGTZ_SWStartAcquisition(handle_));
    check(CAEN_DGTZ_ClearData(handle_));
    query_status();
    log() << "Acquisition started.\n";
}

void Digitizer::write_calibration_tables() {
    CalibrationTables tables;
    tables.load_from_digitizer(handle_);
    tables.write_all(serial_code());
}

void Digitizer::end_acquisition() {
    // Stop acquisition
    check(CAEN_DGTZ_SWStopAcquisition(handle_));
    log() << "Acquisition stopped.\n";
}

#include "CalvisionDaq/common/Stopwatch.h"

void Digitizer::read() {

    // Stopwatch<std::chrono::microseconds> stopwatch;

    check(CAEN_DGTZ_ReadData(handle_,
                             CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
                             readout_buffer_,
                             &readout_size_));

    
    UIntType num_events = 0;
    check(CAEN_DGTZ_GetNumEvents(handle_, readout_buffer_, readout_size_, &num_events));
    // log() << "Num events: " << num_events << "\n";
    num_events_read_ += num_events;
    
    if (num_events > 0) {
        event_callback_(readout_buffer_, readout_size_);
    }

    // const auto duration = stopwatch();

    // log() << "Read " << num_events << " in " << duration << " (" << (duration / std::min(1u, num_events)) << " / event)\n";
}

#include <bitset>

void Digitizer::query_status() {
    UIntType status_reg;
    check(CAEN_DGTZ_ReadRegister(handle_, CAEN_DGTZ_ACQ_STATUS_ADD, &status_reg));
    // log() << "status: " << std::bitset<32>(status_reg) << "\n";
    running_     = (status_reg & 0b0000'0100) != 0;
    ready_       = (status_reg & 0b0000'1000) != 0;
    buffer_full_ = (status_reg & 0b0001'0000) != 0;
}

bool Digitizer::ready() const { return ready_; }
bool Digitizer::running() const { return running_; }
bool Digitizer::buffer_full() const { return buffer_full_; }
UIntType Digitizer::num_events_read() const { return num_events_read_; }

void Digitizer::set_event_callback(const CallbackFunc& event_callback) {
    event_callback_ = event_callback;
}

void Digitizer::print() const {
    // Board info
    log() << " ----- Board Info:\n"
              << "Model Name: " << board_info_.ModelName << "\n"
              << "Model: " << *BoardModelTable.get<CaenEnumValue::Name>(static_cast<CAEN_DGTZ_BoardModel_t>(board_info_.Model)) << "\n"
              << "Family Code: " << *BoardFamilyCodeTable.get<CaenEnumValue::Name>(static_cast<CAEN_DGTZ_BoardFamilyCode_t>(board_info_.FamilyCode)) << "\n"
              // << "Family Code: " << BoardFamilyCode_to_string(static_cast<CAEN_DGTZ_BoardFamilyCode_t>(board_info_.FamilyCode)) << "\n"
              << "Channels: " << board_info_.Channels << "\n"
              << "Serial Number: " << board_info_.SerialNumber << "\n"
              << "\n";


    log() << " ----- Data Readout:\n";
    UIntType max_num_events;
    check(CAEN_DGTZ_GetMaxNumEventsBLT(handle_, &max_num_events));
    log() << "Max Number Events BLT: " << max_num_events << "\n";

    log() << "\n"
              << " ----- Trigger Info:\n";
    CAEN_DGTZ_TriggerMode_t trigger_mode;
    
    check(CAEN_DGTZ_GetSWTriggerMode(handle_, &trigger_mode));
    log() << "SW Trigger Mode: " << *TriggerModeTable.get<CaenEnumValue::Name>(trigger_mode) << "\n";

    check(CAEN_DGTZ_GetExtTriggerInputMode(handle_, &trigger_mode));
    log() << "EXT Trigger Mode: " << *TriggerModeTable.get<CaenEnumValue::Name>(trigger_mode) << "\n";

    CAEN_DGTZ_RunSyncMode_t run_sync_mode;
    check(CAEN_DGTZ_GetRunSynchronizationMode(handle_, &run_sync_mode));
    log() << "Run Synchronization Mode: " << *RunSyncModeTable.get<CaenEnumValue::Name>(run_sync_mode) << "\n";

    CAEN_DGTZ_IOLevel_t io_level;
    check(CAEN_DGTZ_GetIOLevel(handle_, &io_level));
    log() << "IO Level: " << *IOLevelTable.get<CaenEnumValue::Name>(io_level) << "\n";

    for (UIntType i = 0; i < N_Channels; i++) {
        CAEN_DGTZ_TriggerPolarity_t trigger_polarity;
        check(CAEN_DGTZ_GetTriggerPolarity(handle_, i, &trigger_polarity));
        log() << "Trigger polarity channel " << i << ": " << *TriggerPolarityTable.get<CaenEnumValue::Name>(trigger_polarity) << "\n";
    }

    for (UIntType i = 0; i < N_Groups; i++) {
        UIntType threshold;
        check(CAEN_DGTZ_GetGroupFastTriggerThreshold(handle_, i, &threshold));
        log() << "Group " << i << " fast trigger threshold: " << threshold << "\n";

        check(CAEN_DGTZ_GetGroupFastTriggerDCOffset(handle_, i, &threshold));
        log() << "Group " << i << " fast trigger DC offset: " << threshold << "\n";
    }


    log() << "Fast Trigger digitizing: " << *EnaDisTable.get<CaenEnumValue::Name>(get_fast_trigger_digitizing()) << "\n";

    check(CAEN_DGTZ_GetFastTriggerMode(handle_, &trigger_mode));
    log() << "Fast Trigger mode: " << *TriggerModeTable.get<CaenEnumValue::Name>(trigger_mode) << "\n";

    CAEN_DGTZ_DRS4Frequency_t frequency;
    check(CAEN_DGTZ_GetDRS4SamplingFrequency(handle_, &frequency));
    log() << "DRS4 Sampling Frequency: " << *DRS4FrequencyTable.get<CaenEnumValue::Name>(frequency) << "\n";

    CAEN_DGTZ_OutputSignalMode_t output_signal_mode;
    check(CAEN_DGTZ_GetOutputSignalMode(handle_, &output_signal_mode));
    log() << "Output signal Mode: " << *OutputSignalModeTable.get<CaenEnumValue::Name>(output_signal_mode) << "\n";

    log() << "\n"
              << " ----- Acquisition\n";

    UIntType mask;
    check(CAEN_DGTZ_GetGroupEnableMask(handle_, &mask));
    log() << "Group enable mask: " << std::hex << mask << std::dec << "\n";

    UIntType size;
    check(CAEN_DGTZ_GetRecordLength(handle_, &size));
    log() << "Record Length: " << size << "\n";

    UIntType percent;
    check(CAEN_DGTZ_GetPostTriggerSize(handle_, &percent));
    log() << "Post Trigger Size: " << percent << "%\n";

    CAEN_DGTZ_AcqMode_t acquisition_mode;
    check(CAEN_DGTZ_GetAcquisitionMode(handle_, &acquisition_mode));
    log() << "Acquisition Mode: " << *AcqModeTable.get<CaenEnumValue::Name>(acquisition_mode) << "\n";

    for (UIntType i = 0; i < N_Channels; i++) {
        UIntType offset;
        check(CAEN_DGTZ_GetChannelDCOffset(handle_, i, &offset));
        float dc_offset = -voltage_p2p() * static_cast<float>(offset) / static_cast<float>(0xFFFF);
        log() << "Channel " << i << " DC offset: " << dc_offset << "\n";
        log() << "Channel " << i << " range: ["
            << -(dc_offset + voltage_p2p()) << ", "
            << -(dc_offset +             0) << "]\n";
    }

    CAEN_DGTZ_ZS_Mode_t zs_mode;
    check(CAEN_DGTZ_GetZeroSuppressionMode(handle_, &zs_mode));
    log() << "Zero suppression mode: " << *ZS_ModeTable.get<CaenEnumValue::Name>(zs_mode) << "\n";

    log() << "\n";
}

void Digitizer::set_channel_offsets(const ChannelArray<UIntType>& offsets) {
    for (UIntType c = 0; c < N_Channels; c++) {
        check(CAEN_DGTZ_SetChannelDCOffset(handle_, c, offsets[c]));
    }
}

// Peak to peak voltage is 1 V = 1000 mV
FloatingType Digitizer::voltage_p2p() {
    return 1000.0;
}

constexpr static std::array<UIntType, 7> trigger_dc_offsets = {
            0x55A0,
            0x8000,
            0x51C6,
            0x8000,
            0xA800,
            0x91A7
        };

constexpr static std::array<UIntType, 7> trigger_thresholds = {
            0x6666,
            0x51C6,
            0x51C6,
            0x613E,
            0x6666,
            0x6666,
            0x6666
        };

void Digitizer::set_trigger(TriggerSettings t) {
    unsigned int t_index = static_cast<unsigned int>(t);
    if (t_index >= 7) {
        log() << "Trigger Settings out of bounds\n";
        return;
    }

    for (UIntType group = 0; group < 2; group++) {
        check(CAEN_DGTZ_SetGroupFastTriggerThreshold(handle_, group, trigger_thresholds[t_index]));
        check(CAEN_DGTZ_SetGroupFastTriggerDCOffset(handle_, group, trigger_dc_offsets[t_index]));
    }
}

UIntType Digitizer::event_size() const {
    return event_size_;
}

UIntType Digitizer::serial_code() const {
    return board_info_.SerialNumber;
}

CAEN_DGTZ_EnaDis_t Digitizer::get_fast_trigger_digitizing() const {
    // CAEN_DGTZ_EnaDis_t enable;
    // check(CAEN_DGTZ_GetFastTriggerDigitizing(handle_, &enable));
    
    // Using GetFastTriggerDigitizing returns 2 when enabled? 
    // Just read from the board config register instead
    UIntType trigger_enable_temp;
    check(CAEN_DGTZ_ReadRegister(handle_, 0x8000, &trigger_enable_temp));
    return static_cast<CAEN_DGTZ_EnaDis_t>((trigger_enable_temp & (0b1 << 11)) >> 11);
}

void Digitizer::set_log(std::ostream* log) {
    log_ = log;
}

std::ostream& Digitizer::log() const {
    return *log_;
}
