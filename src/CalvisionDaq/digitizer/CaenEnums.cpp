#include "CaenEnums.h"

#define CAEN_ENUM_CASE(postfix, x     ) case CAEN_DGTZ_##x##postfix : return #x ;
#define CAEN_ENUM_CASE1(postfix, x     ) CAEN_ENUM_CASE(postfix, x)
#define CAEN_ENUM_CASE2(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE1(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE3(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE2(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE4(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE3(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE5(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE4(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE6(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE5(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE7(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE6(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE8(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE7(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE9(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE8(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE10(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE9 (postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE11(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE10(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE12(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE11(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE13(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE12(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE14(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE13(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE15(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE14(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE16(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE15(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE17(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE16(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE18(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE17(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE19(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE18(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE20(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE19(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE21(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE20(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE22(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE21(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE23(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE22(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE24(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE23(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE25(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE24(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE26(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE25(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE27(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE26(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE28(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE27(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE29(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE28(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE30(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE29(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE31(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE30(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE32(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE31(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE33(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE32(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE34(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE33(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE35(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE34(postfix, __VA_ARGS__)
#define CAEN_ENUM_CASE36(postfix, x, ...) CAEN_ENUM_CASE(postfix, x) CAEN_ENUM_CASE35(postfix, __VA_ARGS__)



#define CAEN_ENUM_RECURSIVE_CASE(n,...) CAEN_ENUM_CASE##n(__VA_ARGS__)

#define CAEN_ENUM(name, postfix, n, ...) \
    std::string name##_to_string(CAEN_DGTZ_##name##_t v) { \
        switch (v) { \
            CAEN_ENUM_RECURSIVE_CASE(n, postfix, __VA_ARGS__) \
            default: \
                     return "UnrecognizedEnum"; \
        } \
    }

CAEN_ENUM(BoardModel, , 7, DT5724, DT5721, DT5731, DT5720, DT5740, DT5751, DT5743);
CAEN_ENUM(BoardFamilyCode, _FAMILY_CODE, 15, XX724, XX721, XX731, XX720, XX740, XX751, XX742, XX780, XX761, XX743, XX730, XX790, XX781, XX725, XX782);
CAEN_ENUM(ReadMode, , 6, SLAVE_TERMINATED_READOUT_MBLT, SLAVE_TERMINATED_READOUT_2eVME, SLAVE_TERMINATED_READOUT_2eSST, POLLING_MBLT, POLLING_2eVME, POLLING_2eSST);
CAEN_ENUM(DRS4Frequency, , 4, DRS4_5GHz, DRS4_2_5GHz, DRS4_1GHz, DRS4_750MHz);
CAEN_ENUM(TriggerMode, , 4, TRGMODE_DISABLED, TRGMODE_EXTOUT_ONLY, TRGMODE_ACQ_ONLY, TRGMODE_ACQ_AND_EXTOUT);
CAEN_ENUM(RunSyncMode, , 5, RUN_SYNC_Disabled, RUN_SYNC_TrgOutTrgInDaisyChain, RUN_SYNC_TrgOutSinDaisyChain, RUN_SYNC_SinFanout, RUN_SYNC_GpioGpioDaisyChain);
CAEN_ENUM(IOLevel, , 2, IOLevel_NIM, IOLevel_TTL);
CAEN_ENUM(TriggerPolarity, , 2, TriggerOnRisingEdge, TriggerOnFallingEdge);
CAEN_ENUM(EnaDis, , 2, ENABLE, DISABLE);
// fast trigger mode documentation has wrong enum typedef?
CAEN_ENUM(OutputSignalMode, , 4, TRIGGER, FASTTRG_ALL, FASTTRG_ACCEPTED, BUSY);
CAEN_ENUM(AcqMode, , 3, SW_CONTROLLED, S_IN_CONTROLLED, FIRST_TRG_CONTROLLED);
CAEN_ENUM(ZS_Mode, , 4, ZS_NO, ZS_INT, ZS_ZLE, ZS_AMP);
CAEN_ENUM(ThresholdWeight, , 2, ZS_FINE, ZS_COARSE);

CAEN_ENUM(ErrorCode, , 35,
        Success, CommError, GenericError, InvalidParam, InvalidLinkType, InvalidHandle, MaxDevicesError,
        BadBoardType, BadInterruptLev, BadEventNumber, ReadDeviceRegisterFail, WriteDeviceRegisterFail,
        InvalidChannelNumber, ChannelBusy, FPIOModeInvalid, WrongAcqMode, FunctionNotAllowed, Timeout,
        InvalidBuffer, EventNotFound, InvalidEvent, OutOfMemory, CalibrationError, DigitizerNotFound,
        DigitizerAlreadyOpen, DigitizerNotReady, InterruptNotConfigured, DigitizerMemoryCorrupted,
        DPPFirmwareNotSupported, InvalidLicense, InvalidDigitizerStatus, UnsupportedTrace,
        InvalidProbe, UnsupportedBaseAddress, NotYetImplemented);
