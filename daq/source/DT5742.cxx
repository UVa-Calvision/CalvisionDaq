#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>

#include "DT5742.h"
#include "CAENDigitizer.h"

DT5742::DT5742(int link, int nev, bool spill) {
  CAEN_DGTZ_ErrorCode error;
  error = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, // link type
				  link, // link num
				  0, // conetNode
				  0, // VMEBaseAddress
				  &fHandle);
  if (error != CAEN_DGTZ_Success) {
    std::cout << "UNABLE TO OPEN DIGITIZER... trying again" << std::endl;
    error = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB, // link type
				    link, // link num
				    0, // conetNode
				    0, // VMEBaseAddress
				    &fHandle);
    if (error != CAEN_DGTZ_Success)
      std::cout << "GIVING UP" << std::endl;
      exit(1);
  }

  CAEN_DGTZ_Reset(fHandle);
  
  CAEN_DGTZ_BoardInfo_t fBoardInfo;
  error = CAEN_DGTZ_GetInfo(fHandle, &fBoardInfo);
  if (error != CAEN_DGTZ_Success) {
    std::cout << "UNABLE TO READ DIGITIZER" << std::endl;
    exit(1);
  }
  std::cout << "[DAQ init] " << error << " LINK #" << link << std::endl;
  //std::cout << "=== COMMUNICATION ====" << std::endl;
  //std::cout <<   "Model: " << fBoardInfo.ModelName << std::endl;
  //std::cout <<   "Channels: " << fBoardInfo.Channels << std::endl;
  //std::cout <<   "FormFactor: " << fBoardInfo.FormFactor << std::endl;
  //std::cout <<   "FamilyCode: " << fBoardInfo.FamilyCode << std::endl;
  //std::cout <<   "ROC_FirmwareRel: " << fBoardInfo.ROC_FirmwareRel << std::endl;
  //std::cout <<   "AMC_FirmwareRel: " << fBoardInfo.AMC_FirmwareRel << std::endl;
  std::cout <<   "Serial Number: " << fBoardInfo.SerialNumber << std::endl;
  //std::cout <<   "PCB_Revision: " << fBoardInfo.PCB_Revision << std::endl;
  //std::cout <<   "ADC_NBits: " << fBoardInfo.ADC_NBits << std::endl;
  //std::cout <<   "CommHandle: " << fBoardInfo.CommHandle << std::endl;
  
  fKeepSavingWhileInSpill = spill;
  fNRecTarget = nev;
  fNRecEvents = 0;
  fNTimesNoReading = 0;
  fNTimesAtFull = 0;
  fNIterations = 0;
}

void DT5742::Print() {
  CAEN_DGTZ_ErrorCode error;
  std::cout << "=== TRIGGER ====" << std::endl;

  CAEN_DGTZ_TriggerMode_t tmode; 
  error = CAEN_DGTZ_GetSWTriggerMode(fHandle, &tmode);
  std::cout << "Software Trigger Mode: "; 
  
  error = CAEN_DGTZ_GetExtTriggerInputMode(fHandle, &tmode);
  std::cout << "External Trigger Mode: "; 

  CAEN_DGTZ_RunSyncMode_t runsyncmode;
  error = CAEN_DGTZ_GetRunSynchronizationMode(fHandle, &runsyncmode);
  std::cout << "Run synchronization mode: ";
  std::cout << runsyncmode << std::endl;
  
  CAEN_DGTZ_IOLevel_t iolevel;
  error = CAEN_DGTZ_GetIOLevel(fHandle, &iolevel);
  std::cout << "IO Level: ";
  if(iolevel==CAEN_DGTZ_IOLevel_NIM)
    std::cout << "[NIM] ";
  else
    std::cout << "[TTL] ";
  std::cout << std::endl;
  
  CAEN_DGTZ_TriggerPolarity_t polarity;
  error = CAEN_DGTZ_GetTriggerPolarity(fHandle, 0, &polarity);
  std::cout << "Trigger Polarity: ";
  if(polarity==CAEN_DGTZ_TriggerOnRisingEdge)
    std::cout << "[On rising edge] ";
  else
    std::cout << "[On falling edge] ";
  std::cout << std::endl;

  uint32_t TriggerThreshold = 0;
  error = CAEN_DGTZ_GetGroupFastTriggerThreshold(fHandle, 0, &TriggerThreshold);
  std::cout << "Trig Thresh Group 0:" << TriggerThreshold << std::endl; 
  error = CAEN_DGTZ_GetGroupFastTriggerThreshold(fHandle, 1, &TriggerThreshold);
  std::cout << "Trig Thresh Group 1:" << TriggerThreshold << std::endl;

  uint32_t FastTrigDCOffset = 0;
  error = CAEN_DGTZ_GetGroupFastTriggerDCOffset(fHandle, 0, &FastTrigDCOffset); 
  std::cout << "Fast Trig DC Offset:" << FastTrigDCOffset << std::endl; 
  error = CAEN_DGTZ_GetGroupFastTriggerDCOffset(fHandle, 1, &FastTrigDCOffset); 
  std::cout << "Fast Trig DC Offset Group 2:" << FastTrigDCOffset << std::endl; 

  CAEN_DGTZ_EnaDis_t ft_digtz_enabled;
  error = CAEN_DGTZ_GetFastTriggerDigitizing(fHandle, &ft_digtz_enabled);
  std::cout << "Fast trigger digitizing: ";
  if (ft_digtz_enabled==CAEN_DGTZ_ENABLE)
    std::cout << "[Enabled]" << std::endl;
  else
    std::cout << "[Disabled]" << std::endl;

  CAEN_DGTZ_TriggerMode_t fast_trig_mode;
  error = CAEN_DGTZ_GetFastTriggerMode(fHandle, &fast_trig_mode);
  std::cout << "Fast trigger: ";
  if (fast_trig_mode==CAEN_DGTZ_TRGMODE_ACQ_ONLY) {
    std::cout << "[Acq only]" << std::endl; 
  }
  else {
    std::cout << "[Disabled]" << std::endl;
  }

  CAEN_DGTZ_DRS4Frequency_t freq;
  error = CAEN_DGTZ_GetDRS4SamplingFrequency(fHandle, &freq);
  std::cerr << "DRS4 Sampling Frequency: ";

  std::cout << "=== ACQUISITION ====" << std::endl;
  uint32_t mask; 
  error = CAEN_DGTZ_GetGroupEnableMask(fHandle, &mask); 
  std::cout << "Group Enable Mask: " << std::hex << "0x" <<  mask << std::dec << std::endl; 
  error = CAEN_DGTZ_GetChannelEnableMask(fHandle, &mask); 
  std::cout << "Channel Enable Mask: " << std::hex << "0x" <<  mask << std::dec << std::endl; 
  uint32_t sz; 
  error = CAEN_DGTZ_GetRecordLength(fHandle, &sz); 
  std::cout << "Record Length: " << sz << std::endl; 
  error = CAEN_DGTZ_GetPostTriggerSize(fHandle, &sz); 
  std::cout << "Post Trigger Size: " << sz << std::endl; 
  CAEN_DGTZ_AcqMode_t mode;
  error = CAEN_DGTZ_GetAcquisitionMode(fHandle,&mode);
  std::cout << "Acquisition Mode: ";
  if(mode==CAEN_DGTZ_SW_CONTROLLED)
    std::cout << "[Software controlled] ";
  else if(mode==CAEN_DGTZ_S_IN_CONTROLLED)
    std::cout << "[S_IN CONTROLLED] ";
  else
    std::cout << "[First Trigger Controlled] ";
  std::cout << std::endl;
  
  std::cout << "====================" << std::endl;

  uint32_t reg;
  error = CAEN_DGTZ_ReadRegister(fHandle, 0x810C, &reg); // Global Trigger Mask [31:0]
  std::cout << "Global Trigger Mask => Reg 0x810C:" << std::endl;
  std::cout << " ex_trig: " << ((reg >> 30) & 0b1) << std::endl; // bit 30
  std::cout << " sw_trig: " << ((reg >> 31) & 0b1) << std::endl; // bit 31

  error = CAEN_DGTZ_ReadRegister(fHandle, 0x811C, &reg); // Front Pannel IO Control [31:0]
  std::cout << "Front Pannel IO Control => Reg 0x811C:" << std::hex << reg << std::dec << std::endl;
  std::cout << " bit16: " << ((reg >> 16) & 0b1) << std::endl; // bit 18
  std::cout << " bit17: " << ((reg >> 17) & 0b1) << std::endl; // bit 18
  std::cout << " bit18: " << ((reg >> 18) & 0b1) << std::endl; // bit 18
  std::cout << " bit19: " << ((reg >> 19) & 0b1) << std::endl; // bit 19
  std::cout << " bit20 (busy out): " << ((reg >> 20) & 0b1) << std::endl; // bit 20

  std::cout << "CH-wise configuration: " << std::endl;
  for(int ich=0; ich!=8; ++ich) {
    reg = ich;
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x10A4, reg);
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x1080, &reg);
    std::cout << " channel " << ich << " => thr 0x" << std::hex << reg << std::dec;
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x1098, &reg);
    std::cout << "  adc offset 0x" << std::hex << reg << std::dec << " || ";
    //
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x11A4, reg);
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x1180, &reg);
    std::cout << " channel " << 8+ich << " => thr 0x" << std::hex << reg << std::dec;
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x1198, &reg);
    std::cout << "  adc offset 0x" << std::hex << reg << std::dec << std::endl;

  }
  std::cout << "TR-wise configuration: " << std::endl;
  error = CAEN_DGTZ_ReadRegister(fHandle, 0x10D4, &reg);
  std::cout << " tr0  => thr 0x" << std::hex << reg << std::dec;
  error = CAEN_DGTZ_ReadRegister(fHandle, 0x10DC, &reg);
  std::cout << "  adc offset 0x" << std::hex << reg << std::dec << std::endl;

  error = CAEN_DGTZ_ReadRegister(fHandle, 0x8000, &reg);
  std::cout << "BOARD CONFIGURATION " << std::endl;
  std::cout << " = Test Mode " << (reg & 0b100) << std::endl;
  std::cout << " = Must be 1 " << (reg & 0b1000) << std::endl;
  std::cout << " = TR polarity " << (reg & 0b100000) << " (0: rising edge; 1:faling edge)" << std::endl;
  std::cout << " = Signal TRn readout enabled " << (reg & 0b100000000) << " (0: disable; 1: enable)" << std::endl;
  std::cout << " = TRn local trigger enabled " << (reg & 0b1000000000) << " (0: disable; 1: enable)" << std::endl;
  std::cout << " = Transparent Mode " << (reg & 0b10000000000) << " (0: output; 1: transparent)" << std::endl;
  
  uint32_t numEvents;
  
  error = CAEN_DGTZ_GetMaxNumAggregatesBLT(fHandle, &numEvents);
  if (error != CAEN_DGTZ_Success) {
    std::cerr << "Could not read Max Aggregate Events  " << error << std::endl;
  }
  std::cout << "MaxAggregatesBLT:" << numEvents << std::endl; 
}

void DT5742::Close() {
  CAEN_DGTZ_ErrorCode error;
  error = CAEN_DGTZ_CloseDigitizer(fHandle); 
  std::cout << "[DAQ closed. Goodbye!] " << error << std::endl;
}

void DT5742::Setup(std::string ifilename) {
  CAEN_DGTZ_ErrorCode error = CAEN_DGTZ_Reset(fHandle); // Reset everything
  CAEN_DGTZ_SetRecordLength(fHandle,1024); // Set the record length to 1024 samples (5 GS / s)

  uint32_t gemask = 0x3;
  CAEN_DGTZ_SetGroupEnableMask(fHandle, gemask); // groups "11"
  gemask = 0xffff;
  CAEN_DGTZ_SetChannelEnableMask(fHandle, gemask); // channels "1111 1111 1111 1111"
  CAEN_DGTZ_SetMaxNumAggregatesBLT(fHandle, 1); // 1000? // 1 is fast but is there a risk of lose?

  CAEN_DGTZ_SetFastTriggerDigitizing(fHandle, CAEN_DGTZ_ENABLE);    // Enables digitization of TR0
  CAEN_DGTZ_SetAcquisitionMode(fHandle,CAEN_DGTZ_SW_CONTROLLED);

  std::cout << "Reading from " << ifilename << std::endl;
  std::ifstream ifile(ifilename);
  std::string lcmd;
  for(;;std::getline(ifile,lcmd)) {
    if(!ifile.good()) break;
    ProcessLine( lcmd );
  }
}

void DT5742::ProcessLine(std::string lcmd) {
  CAEN_DGTZ_ErrorCode error;
  std::string cmd = lcmd.substr(0,8);
  uint32_t reg;
  //=================================================
  if( cmd=="DRS4FREQ" ) {
    std::cout << "[accept] DRS4FREQ ";
    CAEN_DGTZ_DRS4Frequency_t freq = CAEN_DGTZ_DRS4Frequency_t( atoi( lcmd.substr(10,1).c_str() ) );
    CAEN_DGTZ_SetDRS4SamplingFrequency( fHandle, freq); // 0: 5Gs   2: 1Gs  3: 0.75 GHz
    std::cout << freq << std::endl;
    //=================================================
  } else if( cmd=="CHNOFFSE" ) {
    std::cout << "[accept] CHNOFFSE ";
    int offset = atoi( lcmd.substr(10,5).c_str() );
    uint wchn = atoi( lcmd.substr(16,2).c_str() );
    uint wgrp = atoi( lcmd.substr(19,1).c_str() );
    if(wgrp==0) {
      reg = (wchn<<16)|(offset); // 0xf means channels 1111 will be affected
      error = CAEN_DGTZ_WriteRegister(fHandle, 0x1098, reg); // GROUP 0
    } else {
      reg = (wchn<<16)|(offset); // 0xf means channels 1111 will be affected
      error = CAEN_DGTZ_WriteRegister(fHandle, 0x1198, reg); // GROUP 1
    }
    std::cout << offset << " GR" << wgrp << " CHN" << wchn << std::endl;
    //=================================================
  } else if( cmd=="TR0OFFSE" ) {
    std::cout << "[accept] TR0OFFSE ";
    int offset =  atoi( lcmd.substr(10,5).c_str() );
    reg = offset;
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x10DC, reg);
    std::cout << offset << std::endl;
    //=================================================
  } else if( cmd=="TRGPOLAR" ) {
    std::cout << "[accept] TRGPOLAR ";
    int val =  atoi( lcmd.substr(10,1).c_str() );
    if(val!=0) {
      CAEN_DGTZ_SetTriggerPolarity(fHandle, 0, CAEN_DGTZ_TriggerOnRisingEdge);
      std::cout << " Rise " << std::endl;
    } else {
      CAEN_DGTZ_SetTriggerPolarity(fHandle, 0, CAEN_DGTZ_TriggerOnFallingEdge);
      std::cout << " Fall " << std::endl;
    }
    //=================================================
  } else if( cmd=="TRG__CHN" ) {
    std::cout << "[accept] TRG__CHN ";

    int offset = atoi( lcmd.substr(10,5).c_str() );
    uint map0 = atoi( lcmd.substr(16,3).c_str() );
    uint map1 = atoi( lcmd.substr(20,3).c_str() );
    std::cout << map0 << " " << map1 << " || ";
    
    CAEN_DGTZ_ReadRegister(fHandle, 0x10A8, &reg);
    reg |= map0; // channels 00001111 (first four channels)
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x10A8, reg); // group0
    CAEN_DGTZ_ReadRegister(fHandle, 0x11A8, &reg);
    reg |= map1; // channels 00001111 (first four channels)
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x11A8, reg); // group1

    CAEN_DGTZ_ReadRegister(fHandle, 0x1080, &reg);
    reg |= (0xf000) | offset;  //setting threshold for all channels (0xf)
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x1080, reg);

    CAEN_DGTZ_ReadRegister(fHandle, 0x1180, &reg);
    reg |= (0xf000) | offset;  //setting threshold for all channels (0xf)
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x1180, reg);
    std::cout << offset << std::endl;
    //=================================================
  } else if( cmd=="TRG__TR0" ) {
    std::cout << "[accept] TRG__TR0 ";
    CAEN_DGTZ_ReadRegister(fHandle, 0x1088, &reg); // Threshold
    int bit = reg & 0x4;
    std::cout << "[check " << bit << "] ";
    //1088 bit[2]==0
    CAEN_DGTZ_SetFastTriggerMode(fHandle, CAEN_DGTZ_TRGMODE_ACQ_ONLY);// Trigger with TR0
    CAEN_DGTZ_ReadRegister(fHandle, 0x10D4, &reg); // Threshold
    int offset = atoi( lcmd.substr(10,5).c_str() );
    reg |= offset;
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x10D4, reg); // Threshold
    std::cout << offset << std::endl;
    //=================================================
  } else if( cmd=="TRG__EXT" ) {
    std::cout << "[accept] TRG__EXT ";
    int val = atoi( lcmd.substr(10,1).c_str() );
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x810C, &reg);
    reg |= (1<<30); /// enables EXT
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x810C, reg);
    CAEN_DGTZ_SetExtTriggerInputMode(fHandle, CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);
     if(val==0) {
      CAEN_DGTZ_SetIOLevel(fHandle, CAEN_DGTZ_IOLevel_NIM); // NIM trigger
      std::cout << val << " NIM" << std::endl;
    } else {
      CAEN_DGTZ_SetIOLevel(fHandle, CAEN_DGTZ_IOLevel_TTL); // TTL trigger
      std::cout << val << " TTL" << std::endl;
    }
    //=================================================
  } else if( cmd=="POSTTRIG" ) {
    std::cout << "[accept] POSTTRIG ";
    int val = atoi( lcmd.substr(10,3).c_str() );
    CAEN_DGTZ_SetPostTriggerSize(fHandle,val); // percentage of record length
    std::cout << val << std::endl;
    //=================================================
  } else if( cmd=="GPO_BUSY" ) {
    std::cout << "[accept] GPO_BUSY ";
    
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x811C, &reg);
    reg |= (3<<18)|(1<<16); /// makes GPO send 'busy' signal
    reg &= ~(1<<20);
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x811C, reg);
    
    //CAEN_DGTZ_SetOutputSignalMode(fHandle,CAEN_DGTZ_BUSY);
    std::cout << std::endl;
    //=================================================
  } else if( cmd=="GPO_TRIG" ) {
    std::cout << "[accept] GPO_TRIG ";
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x811C, &reg);
    reg &= ~(3<<16); /// makes GPO send trigger signal
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x811C, reg);
    std::cout << std::endl;    
    //=================================================
  } else if( cmd=="LEMO_LEV" ) {
    std::cout << "[accept] LEMO_LEV ";
    int val = atoi( lcmd.substr(10,1).c_str() );
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x811C, &reg);
    if(val!=0) { // TTL
      reg |= (1<<0);
      std::cout << "TTL" << std::endl;
    } else { // NIM
      reg &= ~(1<<0);
      std::cout << "NIM" << std::endl;
    }
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x811C, reg);
    //=================================================
  } else if( cmd=="PLL_CLCK" ) {
    std::cout << "[accept] PLL_CLCK ";
    int val = atoi( lcmd.substr(10,1).c_str() );
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x8100, &reg);
    if(val!=0) {
      reg |= (1<<6);
      std::cout << "EXT" << std::endl;
    } else {
      reg &= ~(1<<6);
      std::cout << "INT" << std::endl;
    }
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x8100, reg);
    //=================================================
  } else if( cmd=="GATEVETO" ) {
    std::cout << "[accept] GATEVETO ";
    int val = atoi( lcmd.substr(10,1).c_str() );
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x8000, &reg);
    reg |= (1<<17)|(1<<15); // Enable signal on TRG-IN to gate the acquisition
    if(val!=0) {
      std::cout << "VETO" << std::endl;
    } else {
      reg &= ~(1<<17); // Enable signal on TRG-IN to gate the acquisition
      std::cout << "GATE" << std::endl;
    }
    error = CAEN_DGTZ_WriteRegister(fHandle, 0x8000, reg);
    //error = CAEN_DGTZ_ReadRegister(fHandle, 0x811C, &reg);
    //reg |= (3<<10);
    //error = CAEN_DGTZ_WriteRegister(fHandle, 0x811C, reg);
    //=================================================
 } else if( cmd=="SYNC_MAS" ) {
    std::cout << "[accept] SYNC_MAS ";
    //CAEN_DGTZ_SetOutputSignalMode(fHandle, CAEN_DGTZ_FASTTRG_ACCEPTED);
    CAEN_DGTZ_SetRunSynchronizationMode(fHandle,CAEN_DGTZ_RUN_SYNC_SinFanout);
    CAEN_DGTZ_SetAcquisitionMode(fHandle,CAEN_DGTZ_S_IN_CONTROLLED);
    //=================================================
  } else {
    //std::cout << "[unrecognized] " << lcmd << std::endl;
  }
}

void DT5742::Capture() {
  fNRecEvents = 0;
  fNTimesNoReading = 0;
  fNTimesAtFull = 0;
  fNIterations = 0;

  uint32_t reg;
 
  CAEN_DGTZ_ErrorCode error;
  CAEN_DGTZ_EventInfo_t eventInfo;
  char *EventPtr;
  CAEN_DGTZ_X742_EVENT_t *Event742;
  char *buffer = NULL;
  uint32_t size;
  error = CAEN_DGTZ_AllocateEvent(fHandle, (void**)&Event742);
  error = CAEN_DGTZ_MallocReadoutBuffer(fHandle,&buffer,&size);

  
  //----- open fstream
  std::string filename = "output"+std::to_string(fHandle)+".dat";
  std::ofstream fout;
  fout.open( filename.c_str(), std::fstream::out | std::fstream::binary | std::fstream::trunc);
  //----- open communication channel
  error = CAEN_DGTZ_ReadRegister(fHandle, 0x8100, &reg);
  reg |= 0x4;
  error = CAEN_DGTZ_WriteRegister(fHandle, 0x8100, reg);

  //error = CAEN_DGTZ_SWStartAcquisition(fHandle);
  if (error != CAEN_DGTZ_Success) {
    std::cerr << "[" << fHandle << "] Could not start acquisition: " << error << std::endl;
    exit(1);
  }

  // FORCE CLCK SYNC
  /*
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  reg = 0x1;
  error = CAEN_DGTZ_WriteRegister(fHandle, 0x813C, reg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  */  
  //----- run statistics
  bool InSpill = false;
  auto eventStart = std::chrono::steady_clock::now();
  //----- steering loop
  for( ; (fNRecEvents<fNRecTarget) || InSpill; ++fNIterations ) {
    uint32_t reg;
    error = CAEN_DGTZ_ReadRegister(fHandle, 0x8104, &reg); //AcquisitionStatus
    int running = (reg>>2) & 0b1; // 3rd bit => 1=running | 0=stopped
    int ready   = (reg>>3) & 0b1; // 4th bit => 1=ev ready | 0=no event
    int buffull = (reg>>4) & 0x1; // 5th bit => 1=full | 0=not full
    if((buffull==1)) {
      fNTimesAtFull++;
    }
    if(ready==1) {
      error = CAEN_DGTZ_ReadData(fHandle,
            CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,
            buffer,
            &size);
      if (error)
        std::cout << "[" << fHandle << "] Error " << error << std::endl;
      fout.write(buffer, size);
      fNRecEvents++;
      eventStart = std::chrono::steady_clock::now();
    } else {
      fNTimesNoReading++;
    }
    auto eventStop = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = eventStop-eventStart;
    //std::cout << "**** "<< elapsed.count() << std::endl;
    if (elapsed.count() > 1) {
      InSpill = false;
      //if((fNIterations%60)==0)
      //  std::cout << "[" << fHandle << "] Number of events in disk: " << fNRecEvents << std::endl;
    } else {
      InSpill = fKeepSavingWhileInSpill;
      //if((fNIterations%1)==0)
      //  std::cout << "[" << fHandle << "] Number of events in disk: " << fNRecEvents << std::endl;
    }
  }
  fout.close();
  //error = CAEN_DGTZ_SWStopAcquisition(fHandle);
  CAEN_DGTZ_WriteRegister(fHandle, 0x8100, 0x0);
  CAEN_DGTZ_WriteRegister(fHandle, CAEN_DGTZ_SW_CLEAR_ADD, 0x1);

  if (error != CAEN_DGTZ_Success)
    std::cout << "[" << fHandle << "] Could not stop acquisition: " << error << std::endl;
  // ======================= CLOSE =====================
  error = CAEN_DGTZ_CloseDigitizer(fHandle);
  //std::cout << "[" << fHandle << "] [DAQ closed. Goodbye!]" << std::endl;
}

void DT5742::Summary() {
  std::cout << "[" << fHandle << "] =================" << std::endl;
  std::cout << "[" << fHandle << "] ===== STATS =====" << std::endl;
  std::cout << "[" << fHandle << "] Number of events acquired: "         << fNRecEvents << std::endl;
  std::cout << "[" << fHandle << "] Number of cycles with no reading: "  << fNTimesNoReading << std::endl;
  std::cout << "[" << fHandle << "] Number of cycles with buffer full: " << fNTimesAtFull << std::endl;
  std::cout << "[" << fHandle << "] Number of total read attempts: "     <<  fNIterations << std::endl;
  if(fNIterations != (fNRecEvents + fNTimesNoReading)) {
    std::cout << "[" << fHandle << "]   ** WARNING: check stats  ** " << std::endl;
  }// else {
  //  std::cout << "[" << fHandle << "]   ** not a single missed event at this cpp program speed ** " << std::endl;
  //}
}

