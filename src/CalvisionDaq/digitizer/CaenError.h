#pragma once

#include "CAENDigitizer.h"

#include <iostream>

class CaenError {
public:
    CaenError(CAEN_DGTZ_ErrorCode code);

    CAEN_DGTZ_ErrorCode error() const;

    void print_error(std::ostream& out) const;

private:
    CAEN_DGTZ_ErrorCode error_;
};

/*
 * Check the error code of CAEN_DGTZ function calls
 */
void check(CAEN_DGTZ_ErrorCode error);
