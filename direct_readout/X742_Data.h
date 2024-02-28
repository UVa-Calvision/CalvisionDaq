#pragma once

#include "CaenError.h"
#include "forward.h"
#include "bitmanip.h"

CAEN_DGTZ_ErrorCode decode(UIntType*& buffer, CAEN_DGTZ_X742_GROUP_t& group);
CAEN_DGTZ_ErrorCode decode(UIntType* buffer, CAEN_DGTZ_X742_EVENT_t& event);


// static constexpr std::array<FloatingType, 4> sampling_period = {1.0 / 2.5, 1.0 / 1.0, 1.0 / 5.0, 1.0 / 5.0};
// 
// struct x742GroupData {
//     ChannelMatrix<FloatingType> channel_data;
//     SampleArray<FloatingType> trigger_data;
//     UIntType trigger_time_tag;
//     UIntType start_index_cell;
//     UIntType frequency;
// 
//     CAEN_DGTZ_CaenError decode(UIntType*& datain) {
//         UIntType trigger_digitized, size;
//         std::tie(std::ignore,
//                  start_index_cell,
//                  std::ignore,
//                  frequency,
//                  std::ignore,
//                  trigger_digitized,
//                  size)
//                     = read<2,10,2,2,3,1,12>(*datain++);
// 
//         if (size != 3 * N_Samples) return CAEN_DGTZ_InvalidEvent;
// 
//         for (UIntType i = 0; i < N_Samples; i++) {
//             UIntType value = *datain++;
// 		    channel_data[0][i]  =  (float) ((value & 0x00000FFF)      );    /* S0[11:0] - CH0 */
// 		    channel_data[1][i]  =  (float) ((value & 0x00FFF000) >> 12);    /* S0[11:0] - CH1 */
// 		    channel_data[2][i]  =  (float) ((value & 0xFF000000) >> 24);    /* S0[ 7:0] - CH2 */
// 
//             value = *datain++;
// 		    channel_data[2][i] +=  (float) ((value & 0x0000000F) << 8);
// 		    channel_data[3][i]  =  (float) ((value & 0x0000FFF0) >> 4);     /* S0[11:0] - CH3 */
// 		    channel_data[4][i]  =  (float) ((value & 0x0FFF0000) >> 16);    /* S0[11:0] - CH4 */
// 		    channel_data[5][i]  =  (float) ((value & 0xF0000000) >> 28);    /* S0[3:0]  - CH5 */
// 
//             value = *datain++;
// 		    channel_data[5][i] +=  (float) ((value & 0x000000FF) << 4);
// 		    channel_data[6][i]  =  (float) ((value & 0x000FFF00) >> 8) ;    /* S0[11:0] - CH6 */
// 		    channel_data[7][i]  =  (float) ((value & 0xFFF00000) >> 20);    /* S0[11:0] - CH7 */		
//         }
// 
//         if (trigger_digitized) {
//             for (UIntType i = 0; i < N_Samples; i++) {
//                 UIntType value = *datain++;
//                 trigger_data[  i]  =  (float) ((value & 0x00000FFF)      );    /* S0[11:0] - CH0 */
//                 trigger_data[++i]  =  (float) ((value & 0x00FFF000) >> 12);    /* S0[11:0] - CH1 */
//                 trigger_data[++i]  =  (float) ((value & 0xFF000000) >> 24);    /* S0[ 7:0] - CH2 */
// 
//                 value = *datain++;
//                 trigger_data[  i] +=  (float) ((value & 0x0000000F) << 8);
//                 trigger_data[++i]  =  (float) ((value & 0x0000FFF0) >> 4);     /* S0[11:0] - CH3 */
//                 trigger_data[++i]  =  (float) ((value & 0x0FFF0000) >> 16);    /* S0[11:0] - CH4 */
//                 trigger_data[++i]  =  (float) ((value & 0xF0000000) >> 28);    /* S0[3:0]  - CH5 */
// 
//                 value = *datain++;
//                 trigger_data[  i] +=  (float) ((value & 0x000000FF) << 4);
//                 trigger_data[++i]  =  (float) ((value & 0x000FFF00) >> 8) ;    /* S0[11:0] - CH6 */
//                 trigger_data[++i]  =  (float) ((value & 0xFFF00000) >> 20);    /* S0[11:0] - CH7 */		
//             }
//         }
// 
//         trigger_time_tag = *datain++;
//     }
// };
// 
// struct x742EventData {
//     GroupArray<bool> group_present;
//     GroupArray<x742GroupData> group_data;
// 
//     void clear() const {
//         for (bool& g : group_present)
//             g = false;
//     }
// 
//     CAEN_DGTZ_ErrorCode decode(UIntType* eventPtr) {
//         clear();
// 
//         const UIntType event_size = (*eventPtr) & 0x0FFF'FFFF;
//         const UIntType channel_mask = (*(eventPtr + 1)) & 0x0000'000F;
//         eventPtr += 4; // event header is 4 words
// 
//         if (event_size == 0) return CAEN_DGTZ_InvalidEvent;
// 
//         for (UIntType g = 0; g < N_Groups; g++) {
//             if ((channel_mask >> g) & 0x1) {
//                 check(group_data[g].decode(eventPtr));
//                 group_present[g] = true;
//             }
//         }
// 
//         return CAEN_DGTZ_Success;
//     }
// };
