#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include "../include/forward.h"
#include "../include/name_conventions.h"

void new_plot() {
    gROOT->SetBatch(kTRUE);

    TFile* file = TFile::Open("file.root");
    TTree* tree = file->Get<TTree>("tree");

    if (!tree) {
        std::cerr << "Tree doesn't exist in file!\n";
        return;
    }

    // Branch buffers
    Float_t vertical_gain[N_Channels];
    Float_t vertical_offset[N_Channels];
    Float_t horizontal_interval;
    Double_t horizontal_offset;
    Int_t event;
    Double_t trigger_time;
    Double_t trigger_offset;
    Double_t time[N_Samples];
    Int_t samples;
    Short_t channels[N_Channels][N_Samples];

    tree->SetBranchAddress("vertical_gain", vertical_gain);
    tree->SetBranchAddress("veritcal_offset", vertical_offset);
    tree->SetBranchAddress("horizontal_interval", &horizontal_interval);
    tree->SetBranchAddress("horizontal_offset", &horizontal_offset);
    tree->SetBranchAddress("event", &event);
    tree->SetBranchAddress("trigger_time", &trigger_time);
    tree->SetBranchAddress("trigger_offset", &trigger_offset);
    tree->SetBranchAddress("time", time);
    tree->SetBranchAddress("samples", &samples);
    tree->SetBranchAddress("channels", channels);


    constexpr size_t num_examples = 4;
    std::array<std::array<Double_t, N_Samples>, num_examples> example_waveform_data;
    std::array<std::array<Double_t, N_Samples>, num_examples> example_time_data;


    const auto adc_voltage = [&vertical_offset, &vertical_gain, &channels] (int channel, int sample) {
            return static_cast<Double_t>(channels[channel][sample]) - 0x0800;
            // return vertical_gain[channel] * (static_cast<Double_t>(channels[channel][sample]) / 4095.0) - vertical_offset[channel];
        };


    for (auto event = 0; event < tree->GetEntries(); event++) {
        tree->GetEvent(event);

        if (event < num_examples) {
            for (int i = 0; i < N_Samples; i++) {
                example_waveform_data[event][i] = adc_voltage(0, i);
                example_time_data[event][i] = time[i];
            }
            std::cout << "period: " << horizontal_interval << " [ns]\n";
        } else { break; }
    }

    TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 800);
    canvas->cd();

    TPad* pad = new TPad("pad", "", 0, 0, 1, 1);
    pad->SetLeftMargin(0.15);
    pad->SetBottomMargin(0.10);
    pad->SetRightMargin(0.02);
    pad->SetTopMargin(0.07);
    pad->Draw();
    pad->cd();

    std::array<TGraph*, num_examples> example_graphs;
    TMultiGraph* multigraph = new TMultiGraph();
    for (int i = 0; i < num_examples; i++) {
        example_graphs[i] = new TGraph(N_Samples, example_time_data[i].data(), example_waveform_data[i].data());
        multigraph->Add(example_graphs[i]);

        auto low  = *std::min_element(example_waveform_data[i].begin(), example_waveform_data[i].end());
        auto high = *std::max_element(example_waveform_data[i].begin(), example_waveform_data[i].end());

        std::cout << "Example " << i << " minimum voltage: " << low << "\n";
        std::cout << "Example " << i << " maximum voltage: " << high << "\n";
        std::cout << "Example " << i << " midpoint voltage: " << (high + low) / 2 << "\n";
    }

    multigraph->SetTitle("Example Waveforms;Time [ns];Channel value");
    multigraph->Draw("A");

    canvas->Update();
    canvas->SaveAs("example_waveforms.png");
}
