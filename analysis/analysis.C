#include "TMath.h"
#include <array>

constexpr unsigned int N_Channels = 8;
constexpr unsigned int N_Samples = 1024;


class TreeReader {
public:
        TreeReader(TString filename) {
                file_ = new TFile(filename);
                tree_ = (TTree*) file_->Get("tree");

                tree_->SetBranchAddress("vertical_gain", vertical_gain_.data());
                tree_->SetBranchAddress("vertical_offset", vertical_offset_.data());
                tree_->SetBranchAddress("horizontal_interval", &horizontal_interval_);
                tree_->SetBranchAddress("channels", channels_[0].data());
        }

        UInt_t num_entries() const {
                return tree_->GetEntries();
        }

        void get_entry(UInt_t i) {
                tree_->GetEntry(i);
        }

        std::array<Double_t, N_Samples> time() const {
                std::array<Double_t, N_Samples> t;
                for (int i = 0; i < N_Samples; i++) {
                        t[i] = static_cast<Double_t>(i) * horizontal_interval_ * 1e9;
                }
                return t;
        }

        std::array<Double_t, N_Samples> voltages(UInt_t channel) const {
                std::array<Double_t, N_Samples> volts;
                const Double_t gain = vertical_gain_[channel];
                const Double_t offset = vertical_offset_[channel];
                for (int i = 0; i < N_Samples; i++) {
                        volts[i] = gain * 1000 * (static_cast<Double_t>(channels_[channel][i]) - offset);
                }
                return volts;
        }

        Float_t horizontal_interval() const { return horizontal_interval_; }

private:
        TFile* file_;
        TTree* tree_;

        Float_t horizontal_interval_;
        std::array<Float_t, N_Channels> vertical_offset_, vertical_gain_;
        std::array<std::array<Short_t, N_Samples>, N_Channels> channels_;
};

void plot_samples(TreeReader& tree, int channel) {

        std::cout << "Plotting samples\n";
        
        constexpr int num_to_plot = 3;
        auto mg = new TMultiGraph();
        mg->SetTitle(TString::Format("Sample buffer channel %d;Time [ns];Voltage [mV]", channel));
        for (int i = 0; i < num_to_plot; i++) {
                tree.get_entry(i);
                auto g = new TGraph(N_Samples, tree.time().data(), tree.voltages(channel).data());
                g->SetLineColor(kGray + i);
                mg->Add(g);
        }
        mg->Draw("ac");
}


void analysis(TString filename, int channel) {

        gStyle->SetOptStat(0);

        TreeReader tree(filename);

        auto canvas = new TCanvas("tcsum", "summary");
        canvas->Divide(2,2);
        canvas->cd(1);

        plot_samples(tree, channel);
        

        canvas->cd(2);
        
        auto hprof = new TProfile("hprof", "Average waveform;Time [ns];Voltage [mV]", N_Samples,
                        -tree.horizontal_interval(), 
                        -(static_cast<Double_t>(N_Samples) - 0.5) * tree.horizontal_interval());

        Double_t volt_min = 10000, volt_max = -10000;
        for (int i = 0; i < tree.num_entries(); i++) {
                const auto times = tree.time();
                const auto volts = tree.voltages(channel);
                for (int s = 0; s < N_Samples; s++) {
                        volt_min = std::min(volt_min, volts[s]);
                        volt_max = std::max(volt_max, volts[s]);
                        hprof->Fill(times[s], volts[s]);
                }
        }
        hprof->Draw();


        canvas->Update();
}
