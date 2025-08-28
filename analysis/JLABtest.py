#!/usr/bin/env python

import ROOT as r
import argparse, os, sys
import itertools

def init_jlab_analyzer():
    r.gROOT.ProcessLine(".L jlab_analyzer.C+")
    r.gSystem.Load("jlab_analyzer_C.so")


def print2(message, file_handle=None):
    """Print to screen and optionally to file"""
    print(message)  # To screen
    if file_handle:
        print(message, file=file_handle)
        file_handle.flush()  # Ensure immediate write

    
def getStats(datadir,outfile):
    with open(outfile, "w") as f:
        for i in range(4):
            tr=r.TreeReader(f"crystal_{i}",f"{datadir}/outfile_LG.root")
            print2(f'{datadir}/outfile_LG.root, crystal_{i}: {tr.num_entries()} events',f)
            print2(f'SampleInterval: {tr.horizontal_interval():0.1f} ns',f)
            for ch in range(4):
                print2(f'   sigma ch{ch}={r.calc_noise(tr,ch):0.1f} mV',f)
        for i in range(3):
            tr=r.TreeReader(f"crystal_{i}",f"{datadir}/outfile_HG.root")
            print2(f"{datadir}/outfile_hG.root, crystal_{i}: {tr.num_entries()} events",f)
            print2(f'SampleInterval: {tr.horizontal_interval():0.1f} ns',f)
            for ch in range(4):
                print2(f'   sigma ch{ch}={r.calc_noise(tr,ch):0.1f} mV',f)

def set_cut_range(h, x_cut1=0, x_cut2=0):
    """cut display ranges for a histogram"""
    nx=h.GetNbinsX();
    xmin = h.GetBinLowEdge(1+x_cut1)
    xmax = h.GetBinLowEdge(1+nx-x_cut2)
    h.GetXaxis().SetRangeUser(xmin, xmax)

        
def plotter(datadir,gain,tree,tc,threshold=5):
    tr=r.TreeReader(tree,f"{datadir}/outfile_{gain}.root")
    tc.Divide(2,4)
    for channel in range(0,4):
        tm=r.plot_samples(tr, channel, threshold)
        tc.cd(channel*2+1)
        tm.Draw("alp")
        tp=r.channel_profile(tr, channel, threshold)
        tc.cd(channel*2+2)
        set_cut_range(tp,0,40)
        tp.Draw('hist,c')
    tc.Update()
    
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Quick analysis of JLAB waveforms')
    parser.add_argument('datadir', type=str, default=None, help='directory name required')
    args = parser.parse_args()
    datadir=args.datadir
    if datadir==None:
        print('no data directory given, trying .')
        datadir='.'
    
    # check for data
    print(f'Processing directory {datadir}')
    fLG=f'{datadir}/outfile_LG.root'
    fHG=f'{datadir}/outfile_HG.root'
    if not os.path.exists(fLG) or not os.path.exists(fHG):
        print(f'Missing ROOT files(s) in directory {datadir}')
        sys.exit(1)

    # Set global style
    r.gStyle.SetLabelSize(0.05, "XYZ")
    r.gStyle.SetTitleSize(0.06, "XYZ")
    r.gStyle.SetTitleFontSize(0.08)
    r.gStyle.SetOptStat(0)
    # Set larger bottom margin to accommodate title
    r.gStyle.SetPadBottomMargin(0.15)  # Default is usually 0.1
    r.gStyle.SetPadLeftMargin(0.12)    # Also adjust left for Y-axis title

    # To prevent warnings for overwriting histograms
    r.TH1.AddDirectory(False)
        
    print('Processing files:')
    print(fLG)
    print(fHG)
    
    init_jlab_analyzer()

    # pulse height amplitude threshold for plotting waveforms
    thresholdLG=[20,20,20,20]
    thresholdHG=[20,20,20]
    cryNames=['PWO','PbF','BSO','BGO']

    tcLG=[]
    tcHG=[]
    for ic in range(4):
        tcLG.append(r.TCanvas(f'tcLG{cryNames[ic]}',f'{cryNames[ic]} LG'))
        plotter(datadir,'LG',f'crystal_{ic}',tcLG[ic],thresholdLG[ic])
        if ic<3:
            tcHG.append(r.TCanvas(f'tcHG{cryNames[ic]}',f'{cryNames[ic]} HG'))
            plotter(datadir,'HG',f'crystal_{ic}',tcHG[ic],thresholdHG[ic])

    
    getStats(datadir,f"{datadir}/runsummary.txt")
    tf_out=r.TFile(f"{datadir}/runsummary.root","recreate")
    for tc in itertools.chain(tcLG,tcHG): tc.Write()
    tf_out.Close()
    input('hit return to exit')
