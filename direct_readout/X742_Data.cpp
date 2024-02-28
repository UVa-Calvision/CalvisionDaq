#include "X742_Data.h"

#include "bitmanip.h"

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
