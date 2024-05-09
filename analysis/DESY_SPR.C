#include "TMath.h"
#include <array>
#include "PulseFitter.h"
//#include "findpeaks.h"
#include "CamelFitter.h"

constexpr unsigned int N_Channels = 2 * 10;
constexpr unsigned int N_Samples = 1024;

int binCalc(double x_min, double x_max) {
  int n = (int) ((x_max - x_min) * 4.096);  
  std::cout << "n bins: " << n << "\n";
  return n;
}

class TreeReader {
public:
  TreeReader(TString filename, TString treename="tree") {
    tree_ = new TChain(treename);
    tree_->Add(filename);

    std::cout << "The following files were added to the TChain:\n";
    const auto& fileElements = *tree_->GetListOfFiles();
    for (TObject const* op : fileElements) {
      auto chainElement = static_cast<const TChainElement*>(op);
      std::cout << chainElement->GetTitle() << "\n";
    }
    
    tree_->Print();
    tree_->SetBranchAddress("horizontal_interval", &horizontal_interval_);
    tree_->SetBranchAddress("channels", channels_[0].data());

  }
  
  UInt_t num_entries() const {
    //return 5000; // std::min(5000, tree_->GetEntries());
    return tree_->GetEntries();
  }

  void get_entry(UInt_t i) {
    tree_->GetEntry(i);
  }

  const std::array<Double_t, N_Samples> time() const {
    std::array<Double_t, N_Samples> t;
    for (int i = 0; i < N_Samples; i++) {
      t[i] = static_cast<Double_t>(i) * horizontal_interval_;
    }
    return t;
  }

  std::array<Double_t, N_Samples> voltages(UInt_t channel) const {
    std::array<Double_t, N_Samples> volts;
    for (int i = 0; i < N_Samples; i++) {
      volts[i] = static_cast<Double_t>(channels_[channel][i]);
    }
    return volts;
  }

  Float_t horizontal_interval() const { return horizontal_interval_; }

  UInt_t get_bin(Double_t t) const {
    return static_cast<UInt_t>(t / horizontal_interval_);
  }

private:
  TChain* tree_;

  Float_t horizontal_interval_;
  std::array<std::array<Float_t, N_Samples>, N_Channels> channels_;
  std::array<Double_t, N_Samples> times_;
};


// plot examples of captured wave forms
void plot_samples(TreeReader& tree, int channel, int num_to_plot=5) {

    std::cout << "Plotting samples\n";

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

// filename: root file contianing TTree named 'tree'
// channel: sipm channel
// nmax: max events to plot, -1 = all
void analysis(TString filename, int channel, int nmax=-1) {

    gStyle->SetOptStat(0);

    TreeReader tree(filename);

    TFile tf("temp.root","recreate");
    auto canvas = new TCanvas("tcsum", "summary");
    canvas->Divide(2,2);
    canvas->cd(1);
    canvas->Draw();
    canvas->Update();
    
    plot_samples(tree, channel);
    
    canvas->cd(2);

    // Average pulse shape plot

    auto hprof = new TProfile("hprof", "Average waveform;Time [ns];Voltage [mV]", N_Samples,
            -0.5 * tree.horizontal_interval(), 
            (static_cast<Double_t>(N_Samples) - 0.5) * tree.horizontal_interval());

    int entries=tree.num_entries();
    if (nmax>0) entries = min(entries,nmax);
    std::cout << "Avearing " << entries << " wave buffers\n";

    
    for (int i = 0; i < entries; i++) {

        if (i % 10000 == 0) {
            std::cout << "Event " << i << "\n";
        }

        tree.get_entry(i);
        const auto times = tree.time();
        const auto volts = tree.voltages(channel);

        for (int s = 0; s < N_Samples; s++) {
            hprof->Fill(times[s], volts[s]);
        }
    }
    hprof->DrawCopy();

    // Fit profile histogram to get pulse start and peak locations
    PulseFitter pulse_fit(hprof->ProjectionX());
    pulse_fit.pulse_fit->Draw("same");

    // Get Integration and BLS data

    const PulseParams* params = pulse_fit.parameters();

    const double baseline = params->pedestal;

    const double start = params->x0;
    const int i_start = hprof->GetBin(start);

    const double peak = pulse_fit.x_peak;
    const int i_peak = pulse_fit.i_peak;

    const double stop = pulse_fit.x_peak + 60;
    const int i_stop = hprof->GetBin(stop);
    // try a shorter integration range around peak of profile hist
    // i_start = i_peak - 40;
    // i_stop = i_peak + 20;

    std::cout << "Sample peak at: " << peak
	      << " value " << pulse_fit.y_peak << "\n"
	      << "Integrating over range: " << start << " : " << stop << "\n"
	      << "Integrating over bins: " << i_start << " : " << i_stop << "\n";

    double y_min = baseline*0.95;
    double y_max = pulse_fit.y_peak;

    for (double x : {start, stop, peak}) {
        auto line = new TLine(x, y_min, x, y_max);
        line->SetLineStyle(2);
        if (x == peak) line->SetLineColor(kRed);
        line->Draw();
    }

    // scaling so range of pulse integral histogram is simmilar to pulse height histogram
    double i_scale = pulse_fit.y_peak / hprof->Integral(i_start, i_stop);

    double v_max = (hprof->GetMaximum()-baseline)*3;
    double v_min = -v_max/2.0;
    if (v_min>0) v_min=0;
    cout << "vmax, baseline, vmin " << v_max << " , " << baseline << " , "
	 << v_min << endl;
    v_max+=20;
    int nx = binCalc(v_min, v_max);
    cout << "vmax, vmin, nx " << v_max << " , " << v_min << " , " << nx << endl;
    auto phd = new TH1D("phd", "Pulse Heights;Voltage [mV];Count", nx, v_min, v_max);
    // auto pid = new TH1D("pid", "Pulse Integral around peak / #Deltat;Voltage [mV];Count", nx, x_min, x_max);

    tree.get_entry(0);
    const auto times = tree.time();

    
    for (int i = 0; i < entries; i++) {
        tree.get_entry(i);
        const auto volts = tree.voltages(channel);
       
        //phd->Fill(volts[i_peak] - baseline);
	phd->Fill(volts[i_peak] - volts[i_start]);
	
	// "integrals"
        // double sum = 0;
        // for (int n = i_start; n < i_stop; n++) {
        //     sum += volts[n] - baseline;
        // }
        // pid->Fill(sum * i_scale);
    }


    canvas->cd(3);
    phd->SetLineColor(kBlack);
    phd->DrawCopy();
    find_peaks(phd); 
    

    
//     canvas->cd(4);
//     pid->SetLineColor(kBlack);
//     pid->Draw();
//     find_peaks(pid);

    canvas->Update();
    canvas->SaveAs(TString::Format("peaks_%d.png", channel));
    tf.Write();
    tf.Close();
}
