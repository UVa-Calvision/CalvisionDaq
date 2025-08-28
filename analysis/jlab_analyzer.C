#include "TMath.h"
#include <array>
#include <algorithm>
#include <iostream>
#include <TTree.h>
#include <TProfile.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TFile.h>
#include <TChain.h>
#include <TStyle.h>
#include <TChainElement.h>
using namespace std;


constexpr unsigned int N_Groups = 2;
constexpr unsigned int N_Channels = N_Groups * 8;
constexpr unsigned int N_Samples = 1024;

int binCalc(double x_min, double x_max, double vgain) {
  int n = (int) ((x_max - x_min) / (1000 * vgain / 0x1000));
  std::cout << "n bins: " << n << "\n";
  return n;
}

class TreeReader {
public:
  TreeReader(TString tree_name, TString filename)
    : horizontal_interval_(1.0)
  {
    tree_ = new TChain(tree_name);
    tree_->Add(filename);

    // std::cout << "The following files were added to the TChain:\n";
    const auto& fileElements = *tree_->GetListOfFiles();
    for (TObject const* op : fileElements) {
      auto chainElement = static_cast<const TChainElement*>(op);
      //std::cout << chainElement->GetTitle() << "\n";
    }

    tree_->SetBranchAddress("channel0", channels_[0].data());
    tree_->SetBranchAddress("channel1", channels_[1].data());
    tree_->SetBranchAddress("channel2", channels_[2].data());
    tree_->SetBranchAddress("channel3", channels_[3].data());
  }

  UInt_t num_entries() const {
    if (tree_->GetEntries() < 5000)
      return tree_->GetEntries();
    return 5000;
  }
  
  void get_entry(UInt_t i) {
    tree_->GetEntry(i);

    for (unsigned int j = 0; j < N_Samples; j++) {
      times_[j] = static_cast<Double_t>(j) * horizontal_interval_;
    }
  }

  const std::array<Double_t, N_Samples>& time() const {
    return times_;
  }

  std::array<Double_t, N_Samples> voltages(UInt_t channel) const {
    std::array<Double_t, N_Samples> volts;
    // const Double_t gain = vertical_gain_[channel];
    // const Double_t offset = vertical_offset_[channel];
    for (unsigned int i = 0; i < N_Samples; i++) {
      // volts[i] = gain * (static_cast<Double_t>(channels_[channel][i]) - offset);
      volts[i] = channels_[channel][i];
    }
    return volts;
  }

  Float_t horizontal_interval() const { return horizontal_interval_; }
  // Float_t vertical_gain(UInt_t channel) const { return vertical_gain_[channel]; }
  // Float_t vertical_offset(UInt_t channel) const { return vertical_offset_[channel]; }

  UInt_t get_bin(Double_t t) const {
    return static_cast<UInt_t>(t / horizontal_interval_);
  }

  bool passes_suppression(UInt_t channel, double threshold, int minOT=4) const {
    auto const volts = this->voltages(channel);
    Double_t max = volts[0];
    Double_t min = volts[0];
    int n_successive=0;

    for (unsigned int i = 20; i < N_Samples - 400; i++){
      max = std::max(volts[i], max);
      min = std::min(volts[i], min);
      if ((max - min) > threshold) {
	n_successive++;
	if (n_successive == minOT) return true;
      }
      else n_successive=0;
    }
    return false;
  }

  
  void Print(){
    tree_->Print();
  }

  TChain* GetTree() const {return tree_;}
  
private:
  TChain* tree_;

  Float_t horizontal_interval_;
  // std::array<Float_t, N_Groups> vertical_offset_, vertical_gain_;
  std::array<std::array<Float_t, N_Samples>, 4> channels_;
  std::array<Double_t, N_Samples> times_;
};

// return a multigraph
TMultiGraph* plot_samples(TreeReader& tree, int channel, double threshold) {

  const unsigned colors[]={kOrange+3,kBlack,kRed,kOrange+7};
  //std::cout << "Plotting samples\n";

  unsigned int num_to_plot = tree.num_entries(); // 3;
  auto mg = new TMultiGraph();
  mg->SetTitle(TString::Format("Sample buffer channel %d;Time [ns];Voltage [mV]", channel));
  for (unsigned int i = 0; i < num_to_plot; i++) {
    tree.get_entry(i);
    if (!tree.passes_suppression(channel, threshold))
      continue;
    auto g = new TGraph(N_Samples, tree.time().data(), tree.voltages(channel).data());
    g->SetLineColor(kGray /* +i */);
    //g->SetLineColor(colors[i]);
    mg->Add(g);
  }
  //mg->Draw("ac");
  return mg;
}

// ncut = samples to cut from end to suppress the spike in the DRS readout
TProfile* channel_profile(TreeReader& tree, int channel, double threshold) {


  auto hprof = new TProfile("hprof", "Average waveform;Time [ns];Voltage [mV]", N_Samples,
			    -0.5 * tree.horizontal_interval(), 
			    (static_cast<Double_t>(N_Samples) - 0.5) * tree.horizontal_interval());

  //std::cout << "Entering loop with " << tree.num_entries() << " entries\n";
  for (unsigned int i = 0; i < tree.num_entries(); i++) {
    //if (i % 10000 == 0) {
    //  std::cout << "Event " << i << "\n";
    //}

    tree.get_entry(i);
    const auto times = tree.time();
    const auto volts = tree.voltages(channel);

    if (!tree.passes_suppression(channel, threshold))
      continue;

    for (unsigned int s = 0; s < N_Samples; s++) {
      hprof->Fill(times[s], volts[s]);
    }
  }
  return hprof;
}

// prerange is region in time before start of pulse
double calc_noise(TreeReader& tree, int channel, double prerange=25.) {
  double sum=0;
  double sum2=0;
  int n=0;
  for (unsigned int i = 0; i < tree.num_entries(); i++) {
    tree.get_entry(i);
    const auto times = tree.time();
    const auto volts = tree.voltages(channel);
    for (unsigned int i = 0; i < N_Samples/2; i++){
      if (times[i]>prerange) break;
      sum+=volts[i];
      sum2+=volts[i]*volts[i];
      n++;
      //std::cout << times[i] << " " << volts[i] << std::endl;
    }
  }
  return ( sum2 - sum*sum/n ) / (n-1);
}
