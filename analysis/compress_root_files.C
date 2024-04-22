
void compress_root_files(const char* filename) {

    ROOT::EnableImplicitMT();

    auto infile = TFile::Open(filename);
    TTree* tree = (TTree*) infile->Get("tree");
    auto outfile = TFile::Open(("compressed_" + std::string(filename)).c_str(), "RECREATE", "compressed_file", ROOT::RCompressionSetting::EDefaults::kUseSmallest);
    outfile->cd();
    tree->CloneTree()->Write();
}
