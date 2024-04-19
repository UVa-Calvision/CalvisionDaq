

void check_times(const char* filename, Double_t freq) {
    TChain* tree = new TChain("tree");
    tree->Add(filename);

    std::cout << "The following files were added to the TChain:\n";
    const auto& fileElements = *tree->GetListOfFiles();
    for (TObject const* op : fileElements) {
        auto chainElement = static_cast<const TChainElement*>(op);
        std::cout << chainElement->GetTitle() << "\n";
    }

    std::cout << "Reading tree (" << tree->GetEntries() << " events)\n";

    Double_t period = 1e9 / freq;

    std::vector<Double_t> times;
    Double_t trigger_time;
    tree->SetBranchAddress("trigger_time", &trigger_time);
    for (UInt_t i = 0; i < tree->GetEntries(); i++) {
        tree->GetEntry(i);
        times.push_back(trigger_time);
    }

    std::cout << "Finished reading tree.\n";

    UInt_t num_bad_events = 0;

    for (UInt_t i = 0; i < times.size()-1; i++) {
        Double_t interval = times[i+1] - times[i];
        if (interval < 0) {
            std::cout << "Event " << i << ": likely wrapped:\n"
                << "\tInterval: " << interval << "\n";
        } else if (std::abs(interval - period) / period >= 0.01) {
            std::cout << "Event " << i << ":\n"
                << "\tExpected: " << period << "\n"
                << "\tActual  : " << interval << "\n";
            num_bad_events++;
        }
    }

    std::cout << "Number of bad times: " << num_bad_events << "\n";
}
