#pragma once

#include "CppUtils/c_util/Enum.h"
#include "Digitizer.h"
#include <iostream>

INDEXED_ENUM(CommandValue,
    Name
);


INDEXED_ENUM(CommonCommand,
    Open,
    RecordLength,
    PostTrigger,
    TriggerPolarity,
    EnableExternalTrigger,
    FastTrigger,
    CorrectionLevel,
    Frequency,
    FPIOLevel,
    EnableInput,
    DigitizeFastTrigger,
    MaxNumEventsBLT,
    MaxReadoutCount
);

INDEXED_ENUM(GroupCommand,
    EnableInput,     
    ChannelDcOffsets,
    Ch0DcOffset,     
    Ch1DcOffset,     
    Ch2DcOffset,     
    Ch3DcOffset,     
    Ch4DcOffset,     
    Ch5DcOffset,     
    Ch6DcOffset,     
    Ch7DcOffset
);

INDEXED_ENUM(TriggerCommand,
    DcOffset,
    Threshold,
    Type
);

void run_setup(std::istream& input, Digitizer& digi);
