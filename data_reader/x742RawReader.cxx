#include <fstream>
#include <iostream>
#include <TString.h>
#include "x742RawReader.h"

#include "bitmanip.h"

#include <bitset>

//=======
x742RawReader::x742RawReader(TString filename) 
 : fIFS(filename.Data())
{
  std::cout << "x742RawReader :: " << filename.Data() << std::endl;

  fGroupMask = 0b11;
  for (auto& group : fGroup)
      group = nullptr;
}

//=======
bool x742RawReader::ReadEvent() {
  //RUN HEADER
  const auto& [INIT, TOTAL_EVENT_SIZE] = bmp::read<4,28>(fIFS.read_int());
  if(!fIFS.good()) return false;

  const auto& [BOARDID, BF, RES1, PATTERN, RES2, GROUPMASK] = bmp::read<5,1,2,16,6,2>(fIFS.read_int());

  // TODO: Handle board failure better
  if (BF != 0) {
      std::cerr << "BF bit set!\n";
      return false;
  }

  fGroupMask = GROUPMASK;
  
  const auto& [RES3, EVENTCOUNTER] = bmp::read<8,24>(fIFS.read_int());
  fEventCounter = EVENTCOUNTER;

  fTimeTag = fIFS.read_int();

  bool allGood = true;
  for (int i = 0; i < N_Groups; i++) {
      if (fGroupMask & group_mask(i)) {
          allGood = allGood && ReadGroup(*fGroup[i]);
      }
  }
  return allGood;
}

//=======
bool x742RawReader::ReadGroup(DRSGroupData& group) {
  const auto& [CONTROL1, STARTINDEXCELL, CONTROL2, FREQ, CONTROL3, TR, SIZE] = bmp::read<2,10,2,2,3,1,12>(fIFS.read_int());

  // Documentation asserts group event size is always 1024 * 3 words = 0xC00,
  // but can soften this requirement if necessary
  if (SIZE != 3 * N_Samples) {
      std::cerr << "Group event size differs from N_Samples: SIZE = " << SIZE << "\n";
      return false;
  }

  // Set header data
  group.SetTriggerCell( STARTINDEXCELL );
  group.SetFrequency( FREQ );

  // Read channel samples
  std::array<UIntType, 3> buffer; // 4bytes*3 = 12bytes = 96bits = 12bits * 8 channels
  for(int isa = 0; isa < N_Samples; ++isa) {
    fIFS.read_buffer(buffer);
    group.SetChannelData(isa, bmp::read_8_channels(buffer));
  }

  // TR0 channel data exists and should be read out
  if(TR) {
    // TR samples are stored like channel samples, except all 8 data values are 8 consecutive
    // TR samples. Thus there are 1024 / 8 = 128 chunks to read out.
    for(int ichunk = 0; ichunk < N_Chunks; ++ichunk) { //128*8 = 1024
      fIFS.read_buffer(buffer);
      group.SetTRData(ichunk, bmp::read_8_channels(buffer));
    }
  }
  
  // Read timestamp data
  const auto& [_ignore, TIMESTAMP] = bmp::read<2,30>(fIFS.read_int());
  group.SetTimeStamp( TIMESTAMP );
  
  return true;
}
