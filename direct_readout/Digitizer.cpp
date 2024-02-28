#include "Digitizer.h"

#include <iostream>

void Digitizer::print() const {
    // Board info
    std::cout << " ----- Board Info:\n"
              << "Model Name: " << board_info_.ModelName << "\n"
              << "Model: " << BoardModel_to_string(static_cast<CAEN_DGTZ_BoardModel_t>(board_info_.Model)) << "\n"
              << "Family Code: " << BoardFamilyCode_to_string(static_cast<CAEN_DGTZ_BoardFamilyCode_t>(board_info_.FamilyCode)) << "\n"
              << "Channels: " << board_info_.Channels << "\n"
              << "\n";


    std::cout << " ----- Data Readout:\n";
    UIntType max_num_events;
    check(CAEN_DGTZ_GetMaxNumEventsBLT(handle_, &max_num_events));
    std::cout << "Max Number Events BLT: " << max_num_events << "\n";

    std::cout << "\n"
              << " ----- Trigger Info:\n";
    CAEN_DGTZ_TriggerMode_t trigger_mode;
    
    check(CAEN_DGTZ_GetSWTriggerMode(handle_, &trigger_mode));
    std::cout << "SW Trigger Mode: " << TriggerMode_to_string(trigger_mode) << "\n";

    check(CAEN_DGTZ_GetExtTriggerInputMode(handle_, &trigger_mode));
    std::cout << "EXT Trigger Mode: " << TriggerMode_to_string(trigger_mode) << "\n";

    CAEN_DGTZ_RunSyncMode_t run_sync_mode;
    check(CAEN_DGTZ_GetRunSynchronizationMode(handle_, &run_sync_mode));
    std::cout << "Run Synchronization Mode: " << RunSyncMode_to_string(run_sync_mode) << "\n";

    CAEN_DGTZ_IOLevel_t io_level;
    check(CAEN_DGTZ_GetIOLevel(handle_, &io_level));
    std::cout << "IO Level: " << IOLevel_to_string(io_level) << "\n";

    for (int i = 0; i < N_Channels; i++) {
        CAEN_DGTZ_TriggerPolarity_t trigger_polarity;
        check(CAEN_DGTZ_GetTriggerPolarity(handle_, i, &trigger_polarity));
        std::cout << "Trigger polarity channel " << i << ": " << TriggerPolarity_to_string(trigger_polarity) << "\n";
    }

    for (int i = 0; i < N_Groups; i++) {
        UIntType threshold;
        check(CAEN_DGTZ_GetGroupFastTriggerThreshold(handle_, i, &threshold));
        std::cout << "Group " << i << " fast trigger threshold: " << threshold << "\n";

        check(CAEN_DGTZ_GetGroupFastTriggerDCOffset(handle_, i, &threshold));
        std::cout << "Group " << i << " fast trigger DC offset: " << threshold << "\n";
    }

    CAEN_DGTZ_EnaDis_t enable;
    check(CAEN_DGTZ_GetFastTriggerDigitizing(handle_, &enable));
    std::cout << "Fast Trigger digitizing: " << EnaDis_to_string(enable) << "\n";

    check(CAEN_DGTZ_GetFastTriggerMode(handle_, &trigger_mode));
    std::cout << "Fast Trigger mode: " << TriggerMode_to_string(trigger_mode) << "\n";

    CAEN_DGTZ_DRS4Frequency_t frequency;
    check(CAEN_DGTZ_GetDRS4SamplingFrequency(handle_, &frequency));
    std::cout << "DRS4 Sampling Frequency: " << DRS4Frequency_to_string(frequency) << "\n";

    CAEN_DGTZ_OutputSignalMode_t output_signal_mode;
    check(CAEN_DGTZ_GetOutputSignalMode(handle_, &output_signal_mode));
    std::cout << "Output signal Mode: " << OutputSignalMode_to_string(output_signal_mode) << "\n";

    std::cout << "\n"
              << " ----- Acquisition\n";

    UIntType mask;
    check(CAEN_DGTZ_GetGroupEnableMask(handle_, &mask));
    std::cout << "Group enable mask: " << std::hex << mask << std::dec << "\n";

    UIntType size;
    check(CAEN_DGTZ_GetRecordLength(handle_, &size));
    std::cout << "Record Length: " << size << "\n";

    UIntType percent;
    check(CAEN_DGTZ_GetPostTriggerSize(handle_, &percent));
    std::cout << "Post Trigger Size: " << percent << "%\n";

    CAEN_DGTZ_AcqMode_t acquisition_mode;
    check(CAEN_DGTZ_GetAcquisitionMode(handle_, &acquisition_mode));
    std::cout << "Acquisition Mode: " << AcqMode_to_string(acquisition_mode) << "\n";

    for (int i = 0; i < N_Channels; i++) {
        UIntType offset;
        check(CAEN_DGTZ_GetChannelDCOffset(handle_, i, &offset));
        std::cout << "Channel " << i << " DC offset: " << offset << "\n";
    }

    CAEN_DGTZ_ZS_Mode_t zs_mode;
    check(CAEN_DGTZ_GetZeroSuppressionMode(handle_, &zs_mode));
    std::cout << "Zero suppression mode: " << ZS_Mode_to_string(zs_mode) << "\n";

    std::cout << "\n";
}
