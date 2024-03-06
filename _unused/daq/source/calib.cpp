#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstring>

#include <filesystem>

#include "../../data_reader/common.h"

#include <CAENDigitizer.h>

static CAEN_DGTZ_ErrorCode gLastErrorCode;

void check_error(const std::string& message) {
    if (gLastErrorCode != CAEN_DGTZ_Success) {
        std::cerr << "ERROR: " << message << "\n";
        exit(1);
    }
}

void SaveCorrectionTables(CAEN_DGTZ_DRS4Correction_t *tables, int sernum);


//==========================================================================================
int main(int argc, char **argv) {

    int linkNum = 0;

    // ======================= INIT =====================
    std::cout << linkNum << " *** Retrieving calibration tables: ";
    int fHandle;
    gLastErrorCode = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, // link type
                                             linkNum, // link num
                                             0, // conetNode
                                             0, // VMEBaseAddress
                                             &fHandle);
    check_error("Cannot open digitizer");

    std::cout << "[CONNECTION ESTABLISHED]" << std::endl;

    CAEN_DGTZ_BoardInfo_t fBoardInfo;
    gLastErrorCode = CAEN_DGTZ_GetInfo(fHandle, &fBoardInfo);
    check_error("Unable to read digitizer");

    int SERNUM = fBoardInfo.SerialNumber;
    std::cout << "Serial Number: " << SERNUM << std::endl;


    // ======================= MAIN LOOP  =====================
    // Load the Correction Tables from the Digitizer flash
    CAEN_DGTZ_DRS4Correction_t X742Tables[MAX_X742_GROUP_SIZE];

    gLastErrorCode = CAEN_DGTZ_GetCorrectionTables(fHandle, CAEN_DGTZ_DRS4_1GHz, (void*)X742Tables);
    check_error("Failed to retrieve tables");

    SaveCorrectionTables(X742Tables, SERNUM);

    // ======================= CLOSE =====================
    gLastErrorCode = CAEN_DGTZ_CloseDigitizer(fHandle); 
    check_error("Error closing digitizer");

    std::cout << "DAQ closed. Goodbye!" << std::endl;
    return gLastErrorCode;
}

void SaveCorrectionTables(CAEN_DGTZ_DRS4Correction_t *tables, int sernum = 0) {
    const std::filesystem::path calib_dir = std::filesystem::path("..") / "calib" / std::to_string(sernum);
    std::filesystem::create_directories(calib_dir);

    for(int group = 0; group < N_Groups; group++) {
        CAEN_DGTZ_DRS4Correction_t& table = tables[group];

        DRSGroupCalibration calibration;

        for (int i = 0; i < N_Samples; i++) {
            calibration.timing_correction[i] = table.time[i];
            calibration.trigger_offset[i] = table.cell[N_Channels + 1][i];
            for (int c = 0; c < N_Channels; c++) {
                calibration.channel_offset[c][i] = table.cell[c][i];
            }
        }

        // Should handle nsample offset?
        // tb->nsample[channel][i]

        calibration.write(calib_dir / ("Tables_gr" + std::to_string(group) + ".txt"));
    }
}
