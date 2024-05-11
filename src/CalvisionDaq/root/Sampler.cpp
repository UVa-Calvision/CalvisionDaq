#include "Sampler.h"

#include <TFile.h>
#include <TH1D.h>
#include <TMultiGraph.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TPad.h>
#include <TPaveStats.h>
#include <TLatex.h>
#include <TStyle.h>

RootSampler::RootSampler()
    : last_time_(std::nullopt)
{

    for (UIntType i = 0; i < N_Samples; i++) {
        times_[i] = static_cast<FloatingType>(i);
    }
    
    multigraph_ = new TMultiGraph();
    multigraph_->SetTitle("Sample buffer channel 0;Sample Number;Voltage [mV]");

    hprof_ = new TProfile("hprof", "Average waveform;Sample Number;Voltage [mV]", N_Samples, -0.5, N_Samples-0.5);

    timing_hist_ = new TH1D("timing", "Sample Intervals;Interval time [ms];Count", 10000, 1e-1, 1e1);
}

void RootSampler::draw(const std::string& filename) {
    Double_t w = 1000;
    Double_t h = 600;
    TCanvas* canvas = new TCanvas("tcsum", "summary", w, h);
    canvas->SetCanvasSize(w, h);
    gStyle->SetOptStat();
    canvas->Divide(3,1);
    
    canvas->cd(1);
    multigraph_->Draw("ac");

    canvas->cd(2);
    hprof_->Draw();

    auto* pad = canvas->cd(3);
    pad->SetLogx();
    pad->SetLogy();
    timing_hist_->Draw();
    canvas->Update();

    // Get average readout frequency
    double avg_interval = timing_hist_->GetMean();
    double avg_freq = 1.0 / avg_interval;
    Int_t avg_bin = timing_hist_->FindBin(avg_interval);

    // count number of intervals outside the average bin (including over/under flows)
    UIntType num_outside_bin = 0;
    for (Int_t i = 0; i <= timing_hist_->GetNbinsX(); i++) {
        if (i != avg_bin) {
            num_outside_bin += timing_hist_->GetBinContent(i);
        }
    }

    TPaveStats* ps = (TPaveStats*) pad->GetPrimitive("stats");
    ps->SetName("mystats");
    TList* listOfLines = ps->GetListOfLines();

    TLatex* freq_label = new TLatex(*(TLatex*)listOfLines->At(0));
    freq_label->SetText(0, 0, TString::Format("Avg. Freq = %.3f kHz", avg_freq));
    freq_label->SetTextSize(0);
    listOfLines->Add(freq_label);

    TLatex* outside_label = new TLatex(*(TLatex*)listOfLines->At(0));
    outside_label->SetText(0, 0, TString::Format("Num outside = %d", num_outside_bin));
    outside_label->SetTextSize(0);
    listOfLines->Add(outside_label);
    
    ps->SetBBoxX1(ps->GetX1() * 1.5);
    ps->SetBBoxY2(ps->GetY2() * 1.5);
    
    timing_hist_->SetStats(0);

    canvas->Modified();

    canvas->SaveAs(filename.c_str());
}

void RootSampler::add_sample(UIntType timestamp, const std::array<FloatingType, N_Samples>& data) {

    if (last_time_) {
        double interval = static_cast<double>(timestamp) - static_cast<double>(*last_time_);
        if (interval < 0) {
            // 30-bit timestamp has wrapped around.
            constexpr UIntType max_timestamp = (1 << 30) - 1;
            interval += static_cast<double>(max_timestamp);
        }
        interval *= 8.5 / 1e6; // ns

        timing_hist_->Fill(interval);
    }

    last_time_ = timestamp;

    const int num_graphs = multigraph_->GetListOfGraphs() ? multigraph_->GetListOfGraphs()->GetSize() : 0;
    if (num_graphs < 4) {
        TGraph* g = new TGraph(N_Samples, times_.data(), data.data());
        g->SetLineColor(kGray + num_graphs);
        multigraph_->Add(g);
    }

    for (UIntType i = 0; i < N_Samples; i++) {
        hprof_->Fill(times_[i], data[i]);
    }
}

