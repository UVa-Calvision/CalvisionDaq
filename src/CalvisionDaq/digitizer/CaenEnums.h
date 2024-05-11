#pragma once

#include "CAENDigitizer.h"

#include "CalvisionDaq/common/Forward.h"
#include "CppUtils/c_util/Enum.h"

#include <string>

namespace detail {

template <typename EnumType, typename Seq> struct CaenEnumIndexerHelper;

template <typename EnumType, size_t... Is>
struct CaenEnumIndexerHelper<EnumType, std::index_sequence<Is...> > {
    using type = EnumIndexer<EnumType, static_cast<EnumType>(Is)...>;
};

}

template <typename EnumType, size_t N>
using CaenEnumIndexer = typename detail::CaenEnumIndexerHelper<EnumType, std::make_index_sequence<N>>::type;

using BoardModelIndexer         = CaenEnumIndexer<CAEN_DGTZ_BoardModel_t        , 43>;
using TriggerModeIndexer        = CaenEnumIndexer<CAEN_DGTZ_TriggerMode_t       , 4>;
using ReadModeIndexer           = CaenEnumIndexer<CAEN_DGTZ_ReadMode_t          , 6>;
using DRS4FrequencyIndexer      = CaenEnumIndexer<CAEN_DGTZ_DRS4Frequency_t     , 4>;
using RunSyncModeIndexer        = CaenEnumIndexer<CAEN_DGTZ_RunSyncMode_t       , 5>;
using IOLevelIndexer            = CaenEnumIndexer<CAEN_DGTZ_IOLevel_t           , 2>;
using EnaDisIndexer             = CaenEnumIndexer<CAEN_DGTZ_EnaDis_t            , 2>;
using OutputSignalModeIndexer   = CaenEnumIndexer<CAEN_DGTZ_OutputSignalMode_t  , 8>;
using AcqModeIndexer            = CaenEnumIndexer<CAEN_DGTZ_AcqMode_t           , 4>;
using ZS_ModeIndexer            = CaenEnumIndexer<CAEN_DGTZ_ZS_Mode_t           , 4>;
using ThresholdWeightIndexer    = CaenEnumIndexer<CAEN_DGTZ_ThresholdWeight_t   , 2>;
using TriggerPolarityIndexer    = CaenEnumIndexer<CAEN_DGTZ_TriggerPolarity_t   , 2>;
using ConnectionTypeIndexer     = CaenEnumIndexer<CAEN_DGTZ_ConnectionType      , 8>;
using BoardFormFactorIndexer    = CaenEnumIndexer<CAEN_DGTZ_BoardFormFactor_t   , 4>;

using BoardFamilyCodeIndexer = EnumIndexer<CAEN_DGTZ_BoardFamilyCode_t,
    CAEN_DGTZ_XX724_FAMILY_CODE,
    CAEN_DGTZ_XX721_FAMILY_CODE,
    CAEN_DGTZ_XX731_FAMILY_CODE,
    CAEN_DGTZ_XX720_FAMILY_CODE,
    CAEN_DGTZ_XX740_FAMILY_CODE,
    CAEN_DGTZ_XX751_FAMILY_CODE,
    CAEN_DGTZ_XX742_FAMILY_CODE, 
    CAEN_DGTZ_XX780_FAMILY_CODE,
    CAEN_DGTZ_XX761_FAMILY_CODE,
    CAEN_DGTZ_XX743_FAMILY_CODE,
    CAEN_DGTZ_XX730_FAMILY_CODE,
    CAEN_DGTZ_XX790_FAMILY_CODE,
    CAEN_DGTZ_XX781_FAMILY_CODE,
    CAEN_DGTZ_XX725_FAMILY_CODE,
	CAEN_DGTZ_XX782_FAMILY_CODE
>;


using ErrorCodeIndexer = EnumIndexer<CAEN_DGTZ_ErrorCode,
    CAEN_DGTZ_Success                       ,
    CAEN_DGTZ_CommError                     ,
    CAEN_DGTZ_GenericError                  ,
    CAEN_DGTZ_InvalidParam                  ,
    CAEN_DGTZ_InvalidLinkType               ,
    CAEN_DGTZ_InvalidHandle                 ,
    CAEN_DGTZ_MaxDevicesError               ,
    CAEN_DGTZ_BadBoardType                  ,
    CAEN_DGTZ_BadInterruptLev               ,
    CAEN_DGTZ_BadEventNumber                ,
    CAEN_DGTZ_ReadDeviceRegisterFail        ,
    CAEN_DGTZ_WriteDeviceRegisterFail       ,
    CAEN_DGTZ_InvalidChannelNumber          ,
    CAEN_DGTZ_ChannelBusy                   ,
    CAEN_DGTZ_FPIOModeInvalid               ,
    CAEN_DGTZ_WrongAcqMode                  ,
    CAEN_DGTZ_FunctionNotAllowed            ,
    CAEN_DGTZ_Timeout                       ,
    CAEN_DGTZ_InvalidBuffer                 ,
    CAEN_DGTZ_EventNotFound                 ,
    CAEN_DGTZ_InvalidEvent                  ,
    CAEN_DGTZ_OutOfMemory                   ,
    CAEN_DGTZ_CalibrationError              ,
    CAEN_DGTZ_DigitizerNotFound             ,
    CAEN_DGTZ_DigitizerAlreadyOpen          ,
    CAEN_DGTZ_DigitizerNotReady             ,
    CAEN_DGTZ_InterruptNotConfigured        ,
    CAEN_DGTZ_DigitizerMemoryCorrupted      ,
    CAEN_DGTZ_DPPFirmwareNotSupported       ,
    CAEN_DGTZ_InvalidLicense                ,
    CAEN_DGTZ_InvalidDigitizerStatus        ,
    CAEN_DGTZ_UnsupportedTrace              ,
    CAEN_DGTZ_InvalidProbe                  ,
    CAEN_DGTZ_UnsupportedBaseAddress		,
    CAEN_DGTZ_NotYetImplemented             
>;


INDEXED_ENUM(CaenEnumValue,
    Name,
    Code
);

template <typename EnumIndexer>
using CaenEnumTable = EnumTable<EnumIndexer, CaenEnumValueIndexer, std::string_view, UIntType>;

#define CAEN_ENUM_ENTRY(name) \
    std::make_pair(CAEN_DGTZ_##name, std::tuple(#name, static_cast<UIntType>(CAEN_DGTZ_##name)))


constexpr static auto BoardModelTable = CaenEnumTable<BoardModelIndexer>::make_table(
    CAEN_ENUM_ENTRY(V1724 ),
    CAEN_ENUM_ENTRY(V1721 ),
    CAEN_ENUM_ENTRY(V1731 ),
    CAEN_ENUM_ENTRY(V1720 ),
    CAEN_ENUM_ENTRY(V1740 ),
    CAEN_ENUM_ENTRY(V1751 ),
    CAEN_ENUM_ENTRY(DT5724),
    CAEN_ENUM_ENTRY(DT5721),
    CAEN_ENUM_ENTRY(DT5731),
    CAEN_ENUM_ENTRY(DT5720),
    CAEN_ENUM_ENTRY(DT5740),
    CAEN_ENUM_ENTRY(DT5751),
    CAEN_ENUM_ENTRY(N6724 ),
    CAEN_ENUM_ENTRY(N6721 ),
    CAEN_ENUM_ENTRY(N6731 ),
    CAEN_ENUM_ENTRY(N6720 ),
    CAEN_ENUM_ENTRY(N6740 ),
    CAEN_ENUM_ENTRY(N6751 ),
    CAEN_ENUM_ENTRY(DT5742),
    CAEN_ENUM_ENTRY(N6742 ),
    CAEN_ENUM_ENTRY(V1742 ),
    CAEN_ENUM_ENTRY(DT5780),
    CAEN_ENUM_ENTRY(N6780 ),
    CAEN_ENUM_ENTRY(V1780 ),
    CAEN_ENUM_ENTRY(DT5761),
    CAEN_ENUM_ENTRY(N6761 ),
    CAEN_ENUM_ENTRY(V1761 ),
    CAEN_ENUM_ENTRY(DT5743),
    CAEN_ENUM_ENTRY(N6743 ),
    CAEN_ENUM_ENTRY(V1743 ),
    CAEN_ENUM_ENTRY(DT5730),
    CAEN_ENUM_ENTRY(N6730 ),
    CAEN_ENUM_ENTRY(V1730 ),
    CAEN_ENUM_ENTRY(DT5790),
    CAEN_ENUM_ENTRY(N6790 ),
    CAEN_ENUM_ENTRY(V1790 ),
    CAEN_ENUM_ENTRY(DT5781),
    CAEN_ENUM_ENTRY(N6781 ),
    CAEN_ENUM_ENTRY(V1781 ),
    CAEN_ENUM_ENTRY(DT5725),
    CAEN_ENUM_ENTRY(N6725 ),
    CAEN_ENUM_ENTRY(V1725 ),
	CAEN_ENUM_ENTRY(V1782 )
);

constexpr static auto BoardFamilyCodeTable = CaenEnumTable<BoardFamilyCodeIndexer>::make_table(
    CAEN_ENUM_ENTRY(XX724_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX721_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX731_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX720_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX740_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX751_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX742_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX780_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX761_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX743_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX730_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX790_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX781_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX725_FAMILY_CODE),
    CAEN_ENUM_ENTRY(XX782_FAMILY_CODE)
);

constexpr static auto TriggerModeTable      = CaenEnumTable<TriggerModeIndexer      >::make_table(
    CAEN_ENUM_ENTRY(TRGMODE_DISABLED      ),
    CAEN_ENUM_ENTRY(TRGMODE_EXTOUT_ONLY   ),
    CAEN_ENUM_ENTRY(TRGMODE_ACQ_ONLY      ),
    CAEN_ENUM_ENTRY(TRGMODE_ACQ_AND_EXTOUT)
);

constexpr static auto ReadModeTable         = CaenEnumTable<ReadModeIndexer         >::make_table(
    CAEN_ENUM_ENTRY(SLAVE_TERMINATED_READOUT_MBLT ),
    CAEN_ENUM_ENTRY(SLAVE_TERMINATED_READOUT_2eVME),
    CAEN_ENUM_ENTRY(SLAVE_TERMINATED_READOUT_2eSST),
    CAEN_ENUM_ENTRY(POLLING_MBLT                  ),
    CAEN_ENUM_ENTRY(POLLING_2eVME                 ),
    CAEN_ENUM_ENTRY(POLLING_2eSST                 )
);

constexpr static auto DRS4FrequencyTable    = CaenEnumTable<DRS4FrequencyIndexer    >::make_table(
    CAEN_ENUM_ENTRY(DRS4_5GHz  ),
    CAEN_ENUM_ENTRY(DRS4_2_5GHz),
    CAEN_ENUM_ENTRY(DRS4_1GHz  ),
    CAEN_ENUM_ENTRY(DRS4_750MHz) 
);

constexpr static auto RunSyncModeTable      = CaenEnumTable<RunSyncModeIndexer      >::make_table(
    CAEN_ENUM_ENTRY(RUN_SYNC_Disabled               ),
    CAEN_ENUM_ENTRY(RUN_SYNC_TrgOutTrgInDaisyChain  ),
    CAEN_ENUM_ENTRY(RUN_SYNC_TrgOutSinDaisyChain    ),
    CAEN_ENUM_ENTRY(RUN_SYNC_SinFanout              ),
    CAEN_ENUM_ENTRY(RUN_SYNC_GpioGpioDaisyChain     )
);


constexpr static auto IOLevelTable          = CaenEnumTable<IOLevelIndexer          >::make_table(
    CAEN_ENUM_ENTRY(IOLevel_NIM),
    CAEN_ENUM_ENTRY(IOLevel_TTL)
);

constexpr static auto EnaDisTable           = CaenEnumTable<EnaDisIndexer           >::make_table(
    CAEN_ENUM_ENTRY(ENABLE ),
    CAEN_ENUM_ENTRY(DISABLE)
);

constexpr static auto OutputSignalModeTable = CaenEnumTable<OutputSignalModeIndexer >::make_table(
    CAEN_ENUM_ENTRY(TRIGGER         ),
    CAEN_ENUM_ENTRY(FASTTRG_ALL     ),
    CAEN_ENUM_ENTRY(FASTTRG_ACCEPTED),
    CAEN_ENUM_ENTRY(BUSY            ),
    CAEN_ENUM_ENTRY(CLK_OUT         ),
    CAEN_ENUM_ENTRY(RUN             ),
    CAEN_ENUM_ENTRY(TRGPULSE        ),    
    CAEN_ENUM_ENTRY(OVERTHRESHOLD   )
);

constexpr static auto AcqModeTable          = CaenEnumTable<AcqModeIndexer          >::make_table(
    CAEN_ENUM_ENTRY(SW_CONTROLLED       ),
    CAEN_ENUM_ENTRY(S_IN_CONTROLLED     ),
    CAEN_ENUM_ENTRY(FIRST_TRG_CONTROLLED),
	CAEN_ENUM_ENTRY(LVDS_CONTROLLED     )
);

constexpr static auto ZS_ModeTable          = CaenEnumTable<ZS_ModeIndexer          >::make_table(
    CAEN_ENUM_ENTRY(ZS_NO ),
    CAEN_ENUM_ENTRY(ZS_INT),
    CAEN_ENUM_ENTRY(ZS_ZLE),
    CAEN_ENUM_ENTRY(ZS_AMP)
);

constexpr static auto ThresholdWeightTable  = CaenEnumTable<ThresholdWeightIndexer  >::make_table(
    CAEN_ENUM_ENTRY(ZS_FINE  ),
    CAEN_ENUM_ENTRY(ZS_COARSE)
);

constexpr static auto TriggerPolarityTable  = CaenEnumTable<TriggerPolarityIndexer  >::make_table(
    CAEN_ENUM_ENTRY(TriggerOnRisingEdge ),
    CAEN_ENUM_ENTRY(TriggerOnFallingEdge)
);

constexpr static auto ErrorCodeTable = CaenEnumTable<ErrorCodeIndexer>::make_table(
    CAEN_ENUM_ENTRY(Success                     ),
    CAEN_ENUM_ENTRY(CommError                   ),
    CAEN_ENUM_ENTRY(GenericError                ),
    CAEN_ENUM_ENTRY(InvalidParam                ),
    CAEN_ENUM_ENTRY(InvalidLinkType             ),
    CAEN_ENUM_ENTRY(InvalidHandle               ),
    CAEN_ENUM_ENTRY(MaxDevicesError             ),
    CAEN_ENUM_ENTRY(BadBoardType                ),
    CAEN_ENUM_ENTRY(BadInterruptLev             ),
    CAEN_ENUM_ENTRY(BadEventNumber              ),
    CAEN_ENUM_ENTRY(ReadDeviceRegisterFail      ),
    CAEN_ENUM_ENTRY(WriteDeviceRegisterFail     ),
    CAEN_ENUM_ENTRY(InvalidChannelNumber        ),
    CAEN_ENUM_ENTRY(ChannelBusy                 ),
    CAEN_ENUM_ENTRY(FPIOModeInvalid             ),
    CAEN_ENUM_ENTRY(WrongAcqMode                ),
    CAEN_ENUM_ENTRY(FunctionNotAllowed          ),
    CAEN_ENUM_ENTRY(Timeout                     ),
    CAEN_ENUM_ENTRY(InvalidBuffer               ),
    CAEN_ENUM_ENTRY(EventNotFound               ),
    CAEN_ENUM_ENTRY(InvalidEvent                ),
    CAEN_ENUM_ENTRY(OutOfMemory                 ),
    CAEN_ENUM_ENTRY(CalibrationError            ),
    CAEN_ENUM_ENTRY(DigitizerNotFound           ),
    CAEN_ENUM_ENTRY(DigitizerAlreadyOpen        ),
    CAEN_ENUM_ENTRY(DigitizerNotReady           ),
    CAEN_ENUM_ENTRY(InterruptNotConfigured      ),
    CAEN_ENUM_ENTRY(DigitizerMemoryCorrupted    ),
    CAEN_ENUM_ENTRY(DPPFirmwareNotSupported     ),
    CAEN_ENUM_ENTRY(InvalidLicense              ),
    CAEN_ENUM_ENTRY(InvalidDigitizerStatus      ),
    CAEN_ENUM_ENTRY(UnsupportedTrace            ),
    CAEN_ENUM_ENTRY(InvalidProbe                ),
    CAEN_ENUM_ENTRY(UnsupportedBaseAddress		),
    CAEN_ENUM_ENTRY(NotYetImplemented           )
);

constexpr static auto ConnectionTypeTable = CaenEnumTable<ConnectionTypeIndexer>::make_table(
    CAEN_ENUM_ENTRY(USB            ),
    CAEN_ENUM_ENTRY(OpticalLink    ),
    CAEN_ENUM_ENTRY(USB_A4818_V2718),
    CAEN_ENUM_ENTRY(USB_A4818_V3718),
    CAEN_ENUM_ENTRY(USB_A4818_V4718),
    CAEN_ENUM_ENTRY(USB_A4818      ),
    CAEN_ENUM_ENTRY(ETH_V4718      ),
    CAEN_ENUM_ENTRY(USB_V4718      )
);

constexpr static auto BoardFormFactorTable = CaenEnumTable<BoardFormFactorIndexer>::make_table(
    CAEN_ENUM_ENTRY(VME64_FORM_FACTOR),
    CAEN_ENUM_ENTRY(VME64X_FORM_FACTOR),
    CAEN_ENUM_ENTRY(DESKTOP_FORM_FACTOR),
    CAEN_ENUM_ENTRY(NIM_FORM_FACTOR)
);


#undef CAEN_ENUM_ENTRY
