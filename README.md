# CalvisionDaq

DRS readout and analysis for Calvision.

## Dependencies

Uses the `calvision` conda environment on the Calvision portable computer. Needs CAEN libraries
which is a little bit of a mess at the moment (not in the git repo since they're proprietary?)

## Building

Usual cmake build process:
```bash
git clone https://github.com/hhollenb/CalvisionDaq.git
cd CalvisionDaq
mkdir build && cd build
cmake ..
make
```

## Running

Executables will be in `build/src/CalvisionDaq/exec`. The current executables are:

 - `calibrate`: Downloads and saves the digitizer's calibration files.
 - `readout`: Multithreaded readout of raw digitizer data. Very fast if events are at risk of being dropped.
 - `decode`: Decodes a raw digitizer data file into a root tree. Follows in real time the raw digitizer output.
    Because it's continuously following the raw data file, type `q<Enter>` to quit the decoding (handled asynchronously,
    so can be typed at any time).
 - 'read\_and\_decode`: Reads and decodes the digitizer. Slower than readout/decode, and not tested yet.
