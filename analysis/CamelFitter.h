
#include <vector>

struct HedgehogParams {

    static int size(int N) { return 1 + 2 * N; }

    HedgehogParams(double* arr, int N)
        : width(arr[0])
    {
        heights = arr + 1;
        peaks = heights + N;
    }

    double* to_array() const {
        return &width;
    }

    double& width;
    double* heights;
    double* peaks;

    double func(double x, int N) {
        double sum = 0;
        for (int i = 0; i < N; i++) {
            const double n = (x - peaks[i]) / width;
            sum += heights[i] * exp(-0.5 * n * n);
        }
        return sum;
    }
};


bool extremum(bool use_max, double a, double b) {
    if (use_max)
        return a >= b;
    else
        return a <= b;
}

void find_peaks(TH1D* hist) {

    std::vector<double> peaks;
    std::vector<double> troughs;

    const double bins_per_mv = (hist->FindBin(100) - hist->FindBin(0)) / 100.0;
    std::cout << "bins per mV: " << bins_per_mv << "\n";
    
    // Find extrema in a 1.5 mV window
    const int window = static_cast<int>(bins_per_mv * 1.5);
    std::cout << "window: " << window << "\n";

    int i_prev = 0;
    bool find_max = true;
    for (int i = 0; i < hist->GetNbinsX() + 1 - window; i++) {
        // Find window extrema (skip empty bins in case of over binning)
        int i_window_ext = i;
        for (int j = 1; j < window; j++) {
            int index = i + j;
            if (hist->GetBinContent(index) == 0) continue;
            if (extremum(find_max, hist->GetBinContent(index), hist->GetBinContent(i_window_ext))) {
                i_window_ext = index;
            }
        }

        if (hist->GetBinContent(i_window_ext) == 0) {
            continue;
        }

        // If derivative swaps sign, then we're at an extrema
        if (!extremum(find_max, hist->GetBinContent(i_window_ext), hist->GetBinContent(i_prev))) {

            // if (find_max) {
            //     std::cout << "max: bin " << i_prev << ", content: " << hist->GetBinContent(i_prev) << ", center: " << hist->GetBinCenter(i_prev) << "\n";
            //     auto line = new TLine(hist->GetBinCenter(i_prev), 0, hist->GetBinCenter(i_prev), hist->GetBinContent(i_prev));
            //     line->SetLineColor(find_max ? kBlue : kGreen);
            //     line->Draw();
            // }

            if (find_max) peaks.push_back(hist->GetBinCenter(i_prev));
            else troughs.push_back(hist->GetBinCenter(i_prev));

            // Search for the other extrema
            find_max = !find_max;
        }

        i_prev = i_window_ext;
    }

    // Calculate average separation
    double average_peak_sep = 0.0;
    for (int i = 1; i < peaks.size(); i++) {
        average_peak_sep += peaks[i] - peaks[i-1];
    }
    average_peak_sep /= peaks.size() - 1;
    std::cout << "Average separation: " << average_peak_sep << "\n";
 
    // Try and fit sum of Gaussians
    const double x_min = hist->GetBinCenter(1);
    const double x_max = hist->GetBinCenter(hist->GetNbinsX());

    const int N_peaks = peaks.size();

    TF1* hedgehog = new TF1("hedgehog" + TString(hist->GetName()),
            [N = N_peaks] (double* t, double* par) {
                return HedgehogParams(par, N).func(t[0], N);
            },
            x_min, x_max, HedgehogParams::size(N_peaks));

    HedgehogParams initial(hedgehog->GetParameters(), N_peaks);
    std::vector<double> lower_buffer(HedgehogParams::size(N_peaks), 0);
    std::vector<double> upper_buffer(HedgehogParams::size(N_peaks), 0);
    HedgehogParams lower(lower_buffer.data(), N_peaks);
    HedgehogParams upper(upper_buffer.data(), N_peaks);
    
    initial.width = 0.8;
    lower.width = 0;
    upper.width = 3;
    for (int i = 0; i < N_peaks; i++) {
        // Initial guess of height by averaging over a 0.5 mV window
        int count = 0;
        const int N = static_cast<int>(bins_per_mv * 0.0);
        double height = 0;
        int center = hist->FindBin(peaks[i]);
        for (int j = -N; j <= N; j++) {
            double value = hist->GetBinContent(center + j);
            if (value > 0) {
                height += value;
                count++;
            }
        }
        height /= count;

        initial.heights[i] = height;
        lower.heights[i] = 0;
        upper.heights[i] = hist->GetBinContent(center);

        initial.peaks[i] = peaks[i];
        lower.peaks[i] = (i == 0) ? x_min : troughs[i-1];
        upper.peaks[i] = (i == troughs.size()) ? x_max : troughs[i];
    }

    hedgehog->SetParameters(initial.to_array());
    for (int i = 0; i < HedgehogParams::size(N_peaks); i++) {
        hedgehog->SetParLimits(i, lower_buffer[i], upper_buffer[i]);
    }

    hist->Fit("hedgehog" + TString(hist->GetName()), "N");

    hedgehog->SetNpx(10000);
    hedgehog->Draw("same");

    // Change histogram plotting axis to see peaks better
    hist->GetXaxis()->SetRangeUser(peaks.front() - average_peak_sep, peaks.back() + 3 * average_peak_sep);
}
