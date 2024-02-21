CFLAGS=$(shell root-config --cflags) -fPIC
LIBS=$(shell root-config --glibs)
LFLAGS=$(shell root-config --ldflags)

all:
	g++ $(CFLAGS) -o test main_reader.cxx DRSGroupData.cxx x742RawReader.cxx $(LIBS) $(LFLAGS)
