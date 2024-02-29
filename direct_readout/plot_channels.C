#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"

#include <string>
#include <iostream>
#include <vector>

#include "../include/forward.h"
#include "../include/name_conventions.h"

void plot_waveform(const TString& name, const std::vector<Float_t>& x, const std::vector<Float_t>& y) {
    TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 800);

    TGraph* graph = new TGraph(x.size(), x.data(), y.data());

    graph->SetName(name);
    graph->SetTitle(name + ";Sample Time [ns];" + name);
    graph->Draw("AC");

    canvas->SaveAs(name + ".png");

    delete graph;
    delete canvas;

}

void plot_channels() {
    gROOT->SetBatch(kTRUE);

    TFile* file = TFile::Open("file.root");
    TTree* tree = file->Get<TTree>("tree");

    if (!tree) {
        std::cerr << "Tree doesn't exist in file!\n";
        return;
    }

    // Branch buffers
    UInt_t event_counter;
    UInt_t timestamps[N_Groups];
    UInt_t times[N_Groups][N_Samples];
    Float_t triggers[N_Groups][N_Samples];
    Float_t channels[N_Groups][N_Channels][N_Samples];

    std::vector<Float_t> total_times[N_Groups];
    std::vector<Float_t> total_triggers[N_Groups];
    std::vector<Float_t> total_channels[N_Groups][N_Channels];
    std::vector<Float_t> total_timestamps[N_Groups];

    tree->SetBranchAddress("event_counter", &event_counter);
    for (int i = 0; i < N_Groups; i++) {
        total_times[i].reserve(1000 * N_Samples);
        total_triggers[i].reserve(1000 * N_Samples);
        total_timestamps[i].reserve(1000);

        tree->SetBranchAddress(TString(name_timestamp(i)), &timestamps[i]);
        // tree->SetBranchAddress(TString(name_time(i)), &times[i][0]);
        // tree->SetBranchAddress(TString(name_trigger(i)), &triggers[i][0]);
        for (int j = 0; j < N_Channels; j++) {
            total_channels[i][j].reserve(1000 * N_Samples);

            tree->SetBranchAddress(TString(name_channel(i,j)), &channels[i][j][0]);
        }

        for (UInt_t j = 0; j < N_Samples; j++) {
            times[i][j] = j;
        }
    }


    for (auto event = 0; event < tree->GetEntries(); event++) {
        tree->GetEvent(event);

        if (event >= 10) break;

        for (int i = 0; i < N_Groups; i++) {
            total_timestamps[i].push_back(8.5 * timestamps[i]);
            // std::cout << "Timestamp: " << timestamps[i] << "\n";

            for (int s = 0; s < N_Samples; s++) {

                total_times[i].push_back(/*timestamps[i] + */ times[i][s]);
                // total_triggers[i].push_back(triggers[i][s]);

                for (int j = 0; j < N_Channels; j++) {
                    total_channels[i][j].push_back(channels[i][j][s]);
                }
            }
        }
    }

    std::vector<Float_t> iota(total_timestamps[0].size(), 0);
    for (int i = 0; i < iota.size(); i++)
        iota[i] = static_cast<Float_t>(i);

    for (int i = 0; i < N_Groups; i++) {
        // plot_waveform(name_trigger(i), total_times[i], total_triggers[i]);
        plot_waveform(name_timestamp(i), iota, total_timestamps[i]);

        for (int j = 0; j < N_Channels; j++) {
            plot_waveform(name_channel(i,j), total_times[i], total_channels[i][j]);
        }
    }
}
