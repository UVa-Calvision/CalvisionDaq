#pragma once

#include "CalvisionDaq/common/Forward.h"
#include "CaenEnums.h"

template <typename T>
using FrequencyArray = std::array<T, DRS4FrequencyIndexer::size>;

INDEXED_ENUM(FrequencyValue,
    Frequency,
    SamplingPeriod
)

constexpr inline auto FrequencyTable = EnumTable<DRS4FrequencyIndexer, FrequencyValueIndexer, FloatingType, FloatingType>::make_table(
    std::pair{CAEN_DGTZ_DRS4_5GHz  , std::tuple(5.0,   1 / 5.0  )},
    std::pair{CAEN_DGTZ_DRS4_2_5GHz, std::tuple(2.5,   1 / 2.5  )},
    std::pair{CAEN_DGTZ_DRS4_1GHz  , std::tuple(1.0,   1 / 1.0  )},
    std::pair{CAEN_DGTZ_DRS4_750MHz, std::tuple(0.750, 1 / 0.750)} 
);
