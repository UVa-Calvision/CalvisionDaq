CXX_FLAGS=-fPIC -g -O2 $(shell root-config --cflags)
INCLUDE_DIRS=-L/usr/lib -L. -I../include
LD_LIBS=-lCAENComm -lCAENDigitizer -lm -lpthread $(shell root-config --glibs) $(shell root-config --ldflags)

all:
	g++ $(CXX_FLAGS) -o test main.cpp CaenEnums.cpp Digitizer.cpp BufferedFileIO.cpp X742_Data.cpp X742CorrectionRoutines.c X742DecodeRoutines.c $(INCLUDE_DIRS) $(LD_LIBS)

thread_test:
	g++ -o thread_test test_main.cpp BufferedFileIO.cpp -lpthread $(INCLUDE_DIRS)

clean:
	rm -rf test file.root *.png
