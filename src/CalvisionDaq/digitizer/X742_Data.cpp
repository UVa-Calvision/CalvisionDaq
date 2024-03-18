#include "X742_Data.h"

#include <cstring>

CAEN_DGTZ_ErrorCode decode(UIntType*& buffer, CAEN_DGTZ_X742_GROUP_t& group) {
    UIntType trigger_digitized, size, frequency;
    std::tie(std::ignore,
             group.StartIndexCell,
             std::ignore,
             frequency,
             std::ignore,
             trigger_digitized,
             size)
                = bmp::read<2,10,2,2,3,1,12>(*buffer++);

    if (size != 3 * N_Samples) return CAEN_DGTZ_InvalidEvent;

    for (UIntType i = 0; i < N_Samples; i++) {
        UIntType value = *buffer++;
        group.DataChannel[0][i]  =  (float) ((value & 0x00000FFF)      );    /* S0[11:0] - CH0 */
        group.DataChannel[1][i]  =  (float) ((value & 0x00FFF000) >> 12);    /* S0[11:0] - CH1 */
        group.DataChannel[2][i]  =  (float) ((value & 0xFF000000) >> 24);    /* S0[ 7:0] - CH2 */

        value = *buffer++;
        group.DataChannel[2][i] +=  (float) ((value & 0x0000000F) << 8);
        group.DataChannel[3][i]  =  (float) ((value & 0x0000FFF0) >> 4);     /* S0[11:0] - CH3 */
        group.DataChannel[4][i]  =  (float) ((value & 0x0FFF0000) >> 16);    /* S0[11:0] - CH4 */
        group.DataChannel[5][i]  =  (float) ((value & 0xF0000000) >> 28);    /* S0[3:0]  - CH5 */

        value = *buffer++;
        group.DataChannel[5][i] +=  (float) ((value & 0x000000FF) << 4);
        group.DataChannel[6][i]  =  (float) ((value & 0x000FFF00) >> 8) ;    /* S0[11:0] - CH6 */
        group.DataChannel[7][i]  =  (float) ((value & 0xFFF00000) >> 20);    /* S0[11:0] - CH7 */		
    }

    for (int i = 0; i < N_Channels; i++) {
        group.ChSize[i] = N_Samples;
    }

    if (trigger_digitized) {
        for (UIntType i = 0; i < N_Samples; i++) {
            UIntType value = *buffer++;
            group.DataChannel[8][  i]  =  (float) ((value & 0x00000FFF)      );    /* S0[11:0] - CH0 */
            group.DataChannel[8][++i]  =  (float) ((value & 0x00FFF000) >> 12);    /* S0[11:0] - CH1 */
            group.DataChannel[8][++i]  =  (float) ((value & 0xFF000000) >> 24);    /* S0[ 7:0] - CH2 */

            value = *buffer++;
            group.DataChannel[8][  i] +=  (float) ((value & 0x0000000F) << 8);
            group.DataChannel[8][++i]  =  (float) ((value & 0x0000FFF0) >> 4);     /* S0[11:0] - CH3 */
            group.DataChannel[8][++i]  =  (float) ((value & 0x0FFF0000) >> 16);    /* S0[11:0] - CH4 */
            group.DataChannel[8][++i]  =  (float) ((value & 0xF0000000) >> 28);    /* S0[3:0]  - CH5 */

            value = *buffer++;
            group.DataChannel[8][  i] +=  (float) ((value & 0x000000FF) << 4);
            group.DataChannel[8][++i]  =  (float) ((value & 0x000FFF00) >> 8) ;    /* S0[11:0] - CH6 */
            group.DataChannel[8][++i]  =  (float) ((value & 0xFFF00000) >> 20);    /* S0[11:0] - CH7 */		
        }
        group.ChSize[8] = N_Samples;
    } else {
        group.ChSize[8] = 0;
    }


    group.TriggerTimeTag = *buffer++;

    return CAEN_DGTZ_Success;
}

CAEN_DGTZ_ErrorCode decode(UIntType* buffer, CAEN_DGTZ_X742_EVENT_t& event) {
    const UIntType event_size = (*buffer) & 0x0FFF'FFFF;
    const UIntType channel_mask = (*(buffer + 1)) & 0x0000'000F;
    buffer += 4; // event header is 4 words

    if (event_size == 0) return CAEN_DGTZ_InvalidEvent;

    for (UIntType g = 0; g < N_Groups; g++) {
        event.GrPresent[g] = (channel_mask >> g) & 1;
        if (event.GrPresent[g]) {
            check(decode(buffer, event.DataGroup[g]));
        }
    }

    return CAEN_DGTZ_Success;
}

void x742GroupData::PeakCorrection() {
	int offset;
	int chaux_en;
	unsigned int i;
	int j;

	chaux_en = trigger_digitized ? 0:1;
	for(j=0; j<(8+chaux_en); j++){
		raw_data(j,0) = raw_data(j,1) ;
	}
	for(i=1; i<N_Samples; i++){
		offset=0;
		for(j=0; j<8; j++){
			if (i==1){
				if ((raw_data(j,2) - raw_data(j,1) )>30){								  
					offset++;
				}
				else {
					if (((raw_data(j,3) - raw_data(j,1) )>30)&&((raw_data(j,3) - raw_data(j,2) )>30)){								  
						offset++;
					}
				}
			}
			else{
				if ((i==N_Samples-1)&&((raw_data(j,N_Samples-2) - raw_data(j,N_Samples-1) )>30)){  
					offset++;										 							        
				}
				else{
					if ((raw_data(j,i-1) - raw_data(j,i) )>30){ 
						if ((raw_data(j,i+1) - raw_data(j,i) )>30)
							offset++;
						else {
							if ((i==N_Samples-2)||((raw_data(j,i+2) -raw_data(j,i) )>30))
								offset++;
						} 							        
					}
				}
			}
		}								

		if (offset==8){
			for(j=0; j<(8+chaux_en); j++){
				if (i==1){
					if ((raw_data(j,2) - raw_data(j,1) )>30) {
						raw_data(j,0) =raw_data(j,2) ;
						raw_data(j,1) =raw_data(j,2) ;
					}
					else{
						raw_data(j,0) =raw_data(j,3) ;
						raw_data(j,1) =raw_data(j,3) ;
						raw_data(j,2) =raw_data(j,3) ;
					}
				}
				else{
					if (i==N_Samples-1){
						raw_data(j,N_Samples-1) =raw_data(j,N_Samples-2) ;
					}
					else{
						if ((raw_data(j,i+1) - raw_data(j,i) )>30)
							raw_data(j,i) =((raw_data(j,i+1) +raw_data(j,i-1) )/2);
						else {
							if (i==N_Samples-2){
								raw_data(j,N_Samples-2) =raw_data(j,N_Samples-3) ;
								raw_data(j,N_Samples-1) =raw_data(j,N_Samples-3) ;
							}
							else {
								raw_data(j,i) =((raw_data(j,i+2) +raw_data(j,i-1) )/2);
								raw_data(j,i+1) =( (raw_data(j,i+2) +raw_data(j,i-1) )/2);
							}
						}
					}
				}
			}
		}								
	}
}

void x742GroupData::ApplyDataCorrection(CAEN_DGTZ_DRS4Correction_t* CTable, int CorrectionLevelMask)
{
    int i, j,rpnt = 0, wpnt = 0, size1, size2,trg = 0,k;
    long samples;
    float Time[1024],t0; 
    float Tsamp; 
	float vcorr; 
    uint16_t st_ind=0; 
    // uint32_t freq = frequency;
    float wave_tmp[1024];
	int cellCorrection =CorrectionLevelMask & 0x1;
	int nsampleCorrection = (CorrectionLevelMask & 0x2) >> 1;
	int timeCorrection = (CorrectionLevelMask & 0x4) >> 2;
   
	switch(static_cast<CAEN_DGTZ_DRS4Frequency_t>(frequency)) {
		case CAEN_DGTZ_DRS4_2_5GHz:
			Tsamp =(float)((1.0/2500.0)*1000.0);
			break;
		case CAEN_DGTZ_DRS4_1GHz:
			Tsamp =(float)((1.0/1000.0)*1000.0);
			break;
		default:
			Tsamp =(float)((1.0/5000.0)*1000.0);
			break;
	}

	st_ind =(uint16_t)(start_index_cell);
	for (i=0;i<N_Channels;i++) {
        for (j=0;j<N_Samples;j++) {
			if (cellCorrection)
                channel_data[i][j] -= CTable->cell[i][((st_ind+j) % 1024)]; 
			if (nsampleCorrection)
                channel_data[i][j] -= CTable->nsample[i][j];
		}
	}

    for (j=0;j<N_Samples;j++) {
        if (cellCorrection)
            trigger_data[j] -= CTable->cell[i][((st_ind+j) % 1024)]; 
        if (nsampleCorrection)
            trigger_data[j] -= CTable->nsample[i][j];
    }

	
	if (cellCorrection)
        PeakCorrection();
	if (!timeCorrection)
        return;

	t0 = CTable->time[st_ind];                       
	Time[0]=0.0;
	for(j=1; j < 1024; j++) {
		 t0= CTable->time[(st_ind+j)%1024]-t0;
		 if  (t0 >0) 
		   Time[j] =  Time[j-1]+ t0;
		 else
		   Time[j] =  Time[j-1]+ t0 + (Tsamp*1024);

		 t0 = CTable->time[(st_ind+j)%1024];
	}
	for (j=0;j<N_Channels;j++) {
		channel_data[j][0] = channel_data[j][1];
		wave_tmp[0] = channel_data[j][0];
		vcorr = 0.0;
		k=0;
		i=0;

		for(i=1; i<1024; i++)  {
			while ((k<1024-1) && (Time[k]<(i*Tsamp)))  k++;
			vcorr =(((float)(channel_data[j][k] - channel_data[j][k-1])/(Time[k]-Time[k-1]))*((i*Tsamp)-Time[k-1]));
			wave_tmp[i]= channel_data[j][k-1] + vcorr;
			k--;								
		}
        std::memcpy((char*)channel_data[j].data(),(char*)wave_tmp,1024*sizeof(float));
    }

    if (trigger_digitized) {
        trigger_data[0] = trigger_data[1];
        wave_tmp[0] = trigger_data[0];
        vcorr = 0.0;
        k=0;
        i=0;

        for(i=1; i<1024; i++)  {
            while ((k<1024-1) && (Time[k]<(i*Tsamp)))  k++;
            vcorr =(((float)(trigger_data[k] - trigger_data[k-1])/(Time[k]-Time[k-1]))*((i*Tsamp)-Time[k-1]));
            wave_tmp[i]= trigger_data[k-1] + vcorr;
            k--;								
        }
        std::memcpy((char*) trigger_data.data(), (char*) wave_tmp,1024*sizeof(float));
    }
}
