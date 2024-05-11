#pragma once

#include "CalvisionDaq/digitizer/X742_Data.h"

#include <optional>
#include <string>

class TMultiGraph;
class TProfile;
class TH1D;

/*
 * A simple class for making plots of samples of the readout and 
 * histograms of the averages.
 */
class RootSampler {
public:
    RootSampler();

    void draw(const std::string& filename);
    void add_sample(UIntType timestamp, const std::array<FloatingType, N_Samples>& data);

private:
    TMultiGraph* multigraph_;
    TProfile* hprof_;
    TH1D* timing_hist_;

    std::array<FloatingType, N_Samples> times_;
    std::optional<UIntType> last_time_;
};
