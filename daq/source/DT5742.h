#include <string>

class DT5742 {
 public:
  DT5742(int link=0, int nev=100, bool spill=false);
  void Setup(std::string ifile);
  void Print();
  void Close();
  void ProcessLine(std::string);
  void Capture();
  void Summary();
  long EventsTarget() {return fNRecTarget;}
  long EventsInDisk() {return fNRecEvents;}

  ~DT5742() {;}
 private:
  int fHandle;
  bool fKeepSavingWhileInSpill;

  long fNRecTarget;
  long fNRecEvents;
  long fNTimesNoReading;
  long fNTimesAtFull;
  long fNIterations;
};
