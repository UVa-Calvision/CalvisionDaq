#include <ROOT/RDataFrame.hxx>

#include <array>

using namespace ROOT;

constexpr inline size_t N_Channels = 16;
constexpr inline size_t N_Samples = 1024;

/*
 * filename should be a glob if the tree is split amongst multiple files, e.g.
 *  filename = "output_HG*.root"
 */
void df_analysis(TString filename, int channel) {

    gStyle->SetOptStat(0);

    TString tree_name = "tree";

    TChain chain(tree_name);
    RDataFrame df{std::string_view(tree_name), std::string_view(filename)};

    auto df_volts = df.Define("volts",
        [channel] (float** channels, double* gain, double* offset) {
            std::array<double, N_Samples> out;
            for (size_t i = 0; i < N_Samples; i++) {
                out[i] = gain[channel] * (static_cast<double>(channels[channel][i]) - offset[channel]);
            }
            return out;
        },
        {"channels", "vertical_gain", "vertical_offset"});


    auto canvas = new TCanvas("tcsum", "summary");
    canvas->Divide(2, 2);
    canvas->cd(1);

    auto mg = new TMultiGraph();
    mg->SetTitle(TString::Format("Sample buffer channel %d;Time [ns];Voltage [mV]", channel));
    for (int i = 0; i < 5; i++) {
        auto g = df_volts.Filter([i] (int e) { return e == i; }, {"event"})
                         .Graph("time", "volts");
        g->SetLineColor(kGray + i);
        mg->Add(g.GetPtr());
    }
    mg->Draw("ac");


    canvas->Update();
    canvas->Show();
}
