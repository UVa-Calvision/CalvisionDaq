#include "CaenError.h"

#include <iostream>

CaenError::CaenError(CAEN_DGTZ_ErrorCode code)
    : error_(code)
{}

CAEN_DGTZ_ErrorCode CaenError::error() const {
    return error_;
}

void CaenError::print_error() const {
    std::cerr << "[CAEN_DGTZ_ERROR " << error_ << "]: "
        << ErrorCode_to_string(error_)
        << "\n";
}

/*
* Check the error code of CAEN_DGTZ function calls
*/
void check(CAEN_DGTZ_ErrorCode error) {
    if (error != CAEN_DGTZ_Success) {
        throw CaenError(error);
    }
}
