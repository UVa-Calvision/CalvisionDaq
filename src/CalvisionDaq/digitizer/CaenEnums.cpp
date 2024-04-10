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

#define CAEN_ERROR_ENUM(name, postfix, n, ...) \
    std::string name##_to_string(CAEN_DGTZ_##name v) { \
        switch (v) { \
            CAEN_ENUM_RECURSIVE_CASE(n, postfix, __VA_ARGS__) \
            default: \
                     return "UnrecognizedEnum"; \
        } \
    }

CAEN_ERROR_ENUM(ErrorCode, , 35,
        Success, CommError, GenericError, InvalidParam, InvalidLinkType, InvalidHandle, MaxDevicesError,
        BadBoardType, BadInterruptLev, BadEventNumber, ReadDeviceRegisterFail, WriteDeviceRegisterFail,
        InvalidChannelNumber, ChannelBusy, FPIOModeInvalid, WrongAcqMode, FunctionNotAllowed, Timeout,
        InvalidBuffer, EventNotFound, InvalidEvent, OutOfMemory, CalibrationError, DigitizerNotFound,
        DigitizerAlreadyOpen, DigitizerNotReady, InterruptNotConfigured, DigitizerMemoryCorrupted,
        DPPFirmwareNotSupported, InvalidLicense, InvalidDigitizerStatus, UnsupportedTrace,
        InvalidProbe, UnsupportedBaseAddress, NotYetImplemented);
