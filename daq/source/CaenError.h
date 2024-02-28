#pragma once

#include "CAENDigitizer.h"

class CaenError {
public:
    CaenError(CAEN_DGTZ_ErrorCode code)
        : error_(code)
    {}

    CAEN_DGTZ_ErrorCode error() const { return error_; }

    void print_error() const {
        std::cerr << "[CAEN_DGTZ_ERROR " << error_ << "]: ";
        switch (error_) {
            case CAEN_DGTZ_Success:
                std::cerr << "Success";
                break;
            case CAEN_DGTZ_CommError:
                std::cerr << "Communication error";
                break;
            case CAEN_DGTZ_GenericError:
                std::cerr << "Unspecified error";
                break;
            case CAEN_DGTZ_InvalidParam:
                std::cerr << "Invalid parameter";
                break;
            case CAEN_DGTZ_InvalidLinkType:
                std::cerr << "Invalid link type";
                break;
            case CAEN_DGTZ_InvalidHandle:
                std::cerr << "Invalid device handler";
                break;
            case CAEN_DGTZ_MaxDevicesError:
                std::cerr << "Maximum number of devices allowed";
                break;
            case CAEN_DGTZ_BadBoardType:
                std::cerr << "Operation not allowed on this type of board";
                break;
            case CAEN_DGTZ_BadInterruptLev:
                std::cerr << "Interrupt level not allowed";
                break;
            case CAEN_DGTZ_BadEventNumber:
                std::cerr << "The event number is bad";
                break;
            case CAEN_DGTZ_ReadDeviceRegisterFail:
                std::cerr << "Unable to read registry";
                break;
            case CAEN_DGTZ_WriteDeviceRegisterFail:
                std::cerr << "Unable to write into the registry";
                break;
            case CAEN_DGTZ_InvalidChannelNumber:
                std::cerr << "Invalid channel number";
                break;
            case CAEN_DGTZ_ChannelBusy:
                std::cerr << "Channel is busy";
                break;
            case CAEN_DGTZ_FPIOModeInvalid:
                std::cerr << "Invalid FPIO mode";
                break;
            case CAEN_DGTZ_WrongAcqMode:
                std::cerr << "Wrong acquisition mode";
                break;
            case CAEN_DGTZ_FunctionNotAllowed:
                std::cerr << "Function is not allowed for this module";
                break;
            case CAEN_DGTZ_Timeout:
                std::cerr << "Communication timeout";
                break;
            case CAEN_DGTZ_InvalidBuffer:
                std::cerr << "Buffer is invalid";
                break;
            case CAEN_DGTZ_EventNotFound:
                std::cerr << "Event is not found";
                break;
            case CAEN_DGTZ_InvalidEvent:
                std::cerr << "Event is invalid";
                break;
            case CAEN_DGTZ_OutOfMemory:
                std::cerr << "Out of memory";
                break;
            case CAEN_DGTZ_CalibrationError:
                std::cerr << "Unable to calibrate the board";
                break;
            case CAEN_DGTZ_DigitizerNotFound:
                std::cerr << "Unable to open the digitizer";
                break;
            case CAEN_DGTZ_DigitizerAlreadyOpen:
                std::cerr << "Digitizer is already open";
                break;
            case CAEN_DGTZ_DigitizerNotReady:
                std::cerr << "Digitizer is not ready to operate";
                break;
            case CAEN_DGTZ_InterruptNotConfigured:
                std::cerr << "Digitizer has not the IRQ configured";
                break;
            case CAEN_DGTZ_DigitizerMemoryCorrupted:
                std::cerr << "Digitizer flash memory is corrupt";
                break;
            case CAEN_DGTZ_DPPFirmwareNotSupported:
                std::cerr << "The digitizer DPP firmware is not supported in this lib version";
                break;
            case CAEN_DGTZ_InvalidLicense:
                std::cerr << "Invalid firmware license";
                break;
            case CAEN_DGTZ_InvalidDigitizerStatus:
                std::cerr << "The digitizer is found in a corrupted status";
                break;
            case CAEN_DGTZ_UnsupportedTrace:
                std::cerr << "The given trace is not supported by the digitizer";
                break;
            case CAEN_DGTZ_InvalidProbe:
                std::cerr << "The given probe is not supported for the given digitizer's trace";
                break;
            case CAEN_DGTZ_UnsupportedBaseAddress:
                std::cerr << "The base address is not supported";
                break;
            case CAEN_DGTZ_NotYetImplemented:
                std::cerr << "The function is not yet implemented";
                break;
            default:
                std::cerr << "Unrecognized error code?";
                break;
        }
        std::cerr << "\n";
    }

private:
    CAEN_DGTZ_ErrorCode error_;
};

/*
 * Check the error code of CAEN_DGTZ function calls
 */
inline void check(CAEN_DGTZ_ErrorCode error) {
    if (error != CAEN_DGTZ_Success) {
        throw CaenError(error);
    }
}


