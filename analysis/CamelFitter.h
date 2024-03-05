
#include <vector>

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
    
    const int window = static_cast<int>(bins_per_mv * 2);
    std::cout << "window: " << window << "\n";

    int i_prev = 0;
    bool find_max = true;
    for (int i = 0; i < hist->GetNbinsX() + 1 - window; i++) {
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

        if (!extremum(find_max, hist->GetBinContent(i_window_ext), hist->GetBinContent(i_prev))) {

            if (find_max) {
                std::cout << "max: bin " << i_prev << ", content: " << hist->GetBinContent(i_prev) << ", center: " << hist->GetBinCenter(i_prev) << "\n";
                auto line = new TLine(hist->GetBinCenter(i_prev), 0, hist->GetBinCenter(i_prev), hist->GetBinContent(i_prev));
                line->SetLineColor(find_max ? kBlue : kGreen);
                line->Draw();
            }

            if (find_max) peaks.push_back(hist->GetBinCenter(i_prev));
            else troughs.push_back(hist->GetBinCenter(i_prev));

            find_max = !find_max;
        }

        i_prev = i_window_ext;
    }

    double average_peak_sep = 0.0;
    for (int i = 1; i < peaks.size(); i++) {
        average_peak_sep += peaks[i] - peaks[i-1];
    }
    average_peak_sep /= peaks.size() - 1;

    std::cout << "Average separation: " << average_peak_sep << "\n";
    
    // for (int i = 0; i < peaks.size(); i++) {
    //     double x = peaks[0] + i * average_peak_sep;
    //     auto line = new TLine(x, 0, x, hist->GetMaximum());
    //     line->SetLineColor(kRed);
    //     line->Draw();
    // }

    const double x_min = hist->GetBinCenter(1);
    const double x_max = hist->GetBinCenter(hist->GetNbinsX());

    const int N_peaks = peaks.size();

    TF1* hedgehog = new TF1("hedgehog" + TString(hist->GetName()),
            [peaks = peaks] (double* t, double* par) {
                const double x = t[0];
                // const double offset = par[0];
                // const double separation = par[1];
                const double width = par[0];
                const double* heights = par+1;
                double sum = 0;
                for (int i = 0; i < peaks.size(); i++) {
                    const double n = (x - peaks[i] /*(offset + i * separation)*/) / width;
                    sum += heights[i] * exp(-0.5 * n * n);
                }
                return sum;
            },
            x_min, x_max, 1 + N_peaks);

    // hedgehog->FixParameter(0, peaks[0]);
    // hedgehog->SetParameter(1, average_peak_sep);
    hedgehog->SetParameter(0, 0.8);
    for (int i = 0; i < N_peaks; i++) {
        int count = 0;
        const int N = static_cast<int>(bins_per_mv * 0.5);
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
        std::cout << "peak " << i << " height: " << hist->GetBinContent(center) << ", average: " << height << "\n";
        hedgehog->SetParameter(i+1, height);
    }

    hedgehog->SetNpx(10000);

    hist->Fit("hedgehog" + TString(hist->GetName()), "N");

    hedgehog->Draw("same");

    hist->GetXaxis()->SetRangeUser(peaks.front() - average_peak_sep, peaks.back() + 3 * average_peak_sep);
}
