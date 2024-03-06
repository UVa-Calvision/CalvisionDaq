CXX_FLAGS=-fPIC -g -O2
INCLUDE_DIRS=-L/usr/lib -L. -I../include
LD_LIBS=-lCAENComm -lCAENDigitizer -lm -lpthread

all: daq2  calib

clean: 
	rm -f daq2 daq calib

daq: source/daq.cc
	g++ -fPIC -g -O2 -o daq source/daq.cc -L/usr/lib/ -L. -lCAENComm -lCAENDigitizer -lm -lpthread

calib: source/calib.cpp
	g++ -fPIC -g -O2 -o calib source/calib.cpp -L/usr/lib/ -L. -lCAENComm -lCAENDigitizer -lm -lpthread

daq2: source/daq.cpp source/DT5742.cxx source//DT5742.h
	g++ -fPIC -g -O2 -o daq2 source/daq.cpp source/DT5742.cxx -L/usr/lib/ -L. -lCAENComm -lCAENDigitizer -lm -lpthread

readout: source/readout.cpp source/DT5742.cxx
	g++ $(CXX_FLAGS) -o readout source/readout.cpp source/DT5742.cxx $(INCLUDE_DIRS) $(LD_LIBS)
