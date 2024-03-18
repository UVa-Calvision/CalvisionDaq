#pragma once

#include "CAENDigitizer.h"

#include <string>

#define CAEN_ENUM_MAP(name) \
    std::string name##_to_string(CAEN_DGTZ_##name##_t v);

CAEN_ENUM_MAP(BoardModel);
CAEN_ENUM_MAP(BoardFamilyCode);
CAEN_ENUM_MAP(ReadMode);
CAEN_ENUM_MAP(DRS4Frequency);
CAEN_ENUM_MAP(TriggerMode);
CAEN_ENUM_MAP(RunSyncMode);
CAEN_ENUM_MAP(IOLevel);
CAEN_ENUM_MAP(TriggerPolarity);
CAEN_ENUM_MAP(EnaDis);
CAEN_ENUM_MAP(OutputSignalMode);
CAEN_ENUM_MAP(AcqMode);
CAEN_ENUM_MAP(ZS_Mode);
CAEN_ENUM_MAP(ThresholdWeight);
// fast trigger mode documentation has wrong enum typedef?
CAEN_ENUM_MAP(ErrorCode);
