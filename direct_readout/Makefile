CXX_FLAGS=-fPIC -g -O2 
INCLUDE_DIRS=-L/usr/lib -L. -I../include
LD_LIBS=-lCAENComm -lCAENDigitizer -lm -lpthread
ROOT_FLAGS=$(shell root-config --cflags)
ROOT_LIBS=$(shell root-config --glibs) $(shell root-config --ldflags)

all:
	g++ $(CXX_FLAGS) $(ROOT_FLAGS) -o test main.cpp CaenEnums.cpp Digitizer.cpp BufferedFileIO.cpp X742_Data.cpp X742CorrectionRoutines.c X742DecodeRoutines.c $(INCLUDE_DIRS) $(LD_LIBS) $(ROOT_LIBS)

thread_test:
	g++ $(CXX_FLAGS) $(ROOT_FLAGS) -o thread_test test_main.cpp BufferedFileIO.cpp $(INCLUDE_DIRS) $(LD_LIBS) $(ROOT_LIBS)

decode: decoder.cpp CaenEnums.cpp X742_Data.cpp
	g++ $(CXX_FLAGS) $(ROOT_FLAGS) -o decode decoder.cpp CaenEnums.cpp X742_Data.cpp $(INCLUDE_DIRS) $(LD_LIBS) $(ROOT_LIBS)

calibrate: calibrate.cpp Digitizer.cpp CaenEnums.cpp
	g++ $(CXX_FLAGS) -o calibrate calibrate.cpp Digitizer.cpp CaenEnums.cpp $(INCLUDE_DIRS) $(LD_LIBS)

clean:
	rm -rf test file.root *.png thread_test decode calibrate *.dat
