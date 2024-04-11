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
    MaxNumEventsBLT
);

INDEXED_ENUM(GroupCommand,
    EnableInput,
    ChannelDcOffsets
);

INDEXED_ENUM(TriggerCommand,
    DcOffset,
    Threshold
);

void run_setup(std::istream& input, Digitizer& digi);
