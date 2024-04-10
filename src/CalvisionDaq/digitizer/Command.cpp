#include "Command.h"

#include "CaenEnums.h"

#include <string>
#include <string_view>


template <typename Indexer>
using CommandTableType = EnumTable<Indexer, CommandValueIndexer, std::string_view>;

constexpr static auto CommonCommandTable = CommandTableType<CommonCommandIndexer>::make_table(
    std::pair(CommonCommand::Open,                  std::tuple("OPEN"                               )),
    std::pair(CommonCommand::RecordLength,          std::tuple("RECORD_LENGTH"                      )),
    std::pair(CommonCommand::PostTrigger,           std::tuple("POST_TRIGGER"                       )),
    std::pair(CommonCommand::TriggerPolarity,       std::tuple("PULSE_POLARITY"                     )),
    std::pair(CommonCommand::EnableExternalTrigger, std::tuple("EXTERNAL_TRIGGER"                   )),
    std::pair(CommonCommand::FastTrigger,           std::tuple("FAST_TRIGGER"                       )),
    std::pair(CommonCommand::CorrectionLevel,       std::tuple("CORRECTION_LEVEL"                   )),
    std::pair(CommonCommand::Frequency,             std::tuple("DRS4_FREQUENCY"                     )),
    std::pair(CommonCommand::FPIOLevel,             std::tuple("FPIO_LEVEL"                         )),
    std::pair(CommonCommand::EnableInput,           std::tuple("ENABLE_INPUT"                       )),
    std::pair(CommonCommand::DigitizeFastTrigger,   std::tuple("ENABLED_FAST_TRIGGER_DIGITIZING"    ))
);

constexpr static auto GroupCommandTable = CommandTableType<GroupCommandIndexer>::make_table(
    std::pair(GroupCommand::EnableInput,                std::tuple("ENABLE_INPUT"       )),
    std::pair(GroupCommand::ChannelDcOffsets,           std::tuple("GRP_CH_DC_OFFSET"   ))
);

constexpr static auto TriggerCommandTable = CommandTableType<TriggerCommandIndexer>::make_table(
    std::pair(TriggerCommand::DcOffset,     std::tuple("DC_OFFSET"          )),
    std::pair(TriggerCommand::Threshold,    std::tuple("TRIGGER_THRESHOLD"  ))
);

// ----- Parse


template <typename T>
T parse_string(const std::string& s) = delete;

template <>
std::string parse_string<std::string>(const std::string& s) { return s; }

template <>
UIntType parse_string<UIntType>(const std::string& s) { return std::stol(s); }

template <typename Table>
typename Table::EnumType parse_enum_string(const std::string& s, const Table& table) {
    auto lookup_result = table.template lookup<CaenEnumValue::Name>(s);
    if (lookup_result)
        return lookup_result->first;
    else
        throw std::runtime_error("Failed to parse CAEN enum");
}

template <>
CAEN_DGTZ_TriggerMode_t parse_string<CAEN_DGTZ_TriggerMode_t>(const std::string& s) {
    if (s == "DISABLED") return CAEN_DGTZ_TRGMODE_DISABLED;
    if (s == "ACQUISITION_ONLY") return CAEN_DGTZ_TRGMODE_ACQ_ONLY;
    return parse_enum_string(s, TriggerModeTable);
}

template <>
CAEN_DGTZ_TriggerPolarity_t parse_string<CAEN_DGTZ_TriggerPolarity_t>(const std::string& s) {
    if (s == "POSITIVE") return CAEN_DGTZ_TriggerOnRisingEdge;
    if (s == "NEGATIVE") return CAEN_DGTZ_TriggerOnFallingEdge;
    return parse_enum_string(s, TriggerPolarityTable);
}

template <>
CAEN_DGTZ_DRS4Frequency_t parse_string<CAEN_DGTZ_DRS4Frequency_t>(const std::string& s) {
    if (s == "0") return CAEN_DGTZ_DRS4_5GHz;
    if (s == "1") return CAEN_DGTZ_DRS4_2_5GHz;
    if (s == "2") return CAEN_DGTZ_DRS4_1GHz;
    if (s == "3") return CAEN_DGTZ_DRS4_750MHz;
    return parse_enum_string(s, DRS4FrequencyTable);
}

template <>
CAEN_DGTZ_IOLevel_t parse_string<CAEN_DGTZ_IOLevel_t>(const std::string& s) {
    if (s == "NIM") return CAEN_DGTZ_IOLevel_NIM;
    if (s == "TTL") return CAEN_DGTZ_IOLevel_TTL;
    return parse_enum_string(s, IOLevelTable);
}

template <>
bool parse_string<bool>(const std::string& s) {
    if (s == "YES")
        return true;
    else if (s == "NO")
        return false;
    throw std::runtime_error("Failed to parse bool");
}

template <>
CAEN_DGTZ_EnaDis_t parse_string<CAEN_DGTZ_EnaDis_t>(const std::string& s) {
    if (s == "YES") return CAEN_DGTZ_ENABLE;
    if (s == "NO") return CAEN_DGTZ_DISABLE;
    return parse_enum_string(s, EnaDisTable);
}

template <>
CAEN_DGTZ_ConnectionType parse_string<CAEN_DGTZ_ConnectionType>(const std::string& s) {
    return parse_enum_string(s, ConnectionTypeTable);
}



template <typename TupleType, size_t... Is>
TupleType parse_arguments(const std::vector<std::string>& tokens, size_t start, std::index_sequence<Is...>) {
    return std::make_tuple(parse_string<typename std::tuple_element<Is, TupleType>::type>(tokens[start + Is])...);
}

template <typename... Args>
std::tuple<Args...> parse_arguments(const std::vector<std::string>& tokens, size_t start) {
    if (start + sizeof...(Args) != tokens.size()) {
        throw std::runtime_error("Incorrect number of arguments");
    }
    return parse_arguments<std::tuple<Args...> >(tokens, start, std::make_index_sequence<sizeof...(Args)>{});
}

std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    size_t last = 0;
    for (size_t i = 0; i < line.length(); i++) {
        if (line[i] == ' ') {
            tokens.push_back(line.substr(last, i - last));
            last = i+1;
        }
    }
    if (last < line.length()) {
        tokens.push_back(line.substr(last));
    }
    return tokens;
}

// ----- Run Commands

enum class CommandType {
    Common,
    Group,
    Trigger
};

void run_common(Digitizer& digi, const std::vector<std::string>& tokens) {
    auto lookup_result = CommonCommandTable.lookup<CommandValue::Name>(tokens[0]);
    if (!lookup_result) throw std::runtime_error("Unrecognized common command " + tokens[0]);
    
    switch (lookup_result->first) {
        case CommonCommand::Open: {
            const auto& [device_type, device_id, unused] = parse_arguments<CAEN_DGTZ_ConnectionType, UIntType, UIntType>(tokens, 1);
            digi.open(device_type, device_id);
            } break;
        case CommonCommand::RecordLength: {
            const auto& [length] = parse_arguments<UIntType>(tokens, 1);
            check(CAEN_DGTZ_SetRecordLength(digi.handle(), length));
            } break;
        case CommonCommand::PostTrigger: {
            const auto& [percent] = parse_arguments<UIntType>(tokens, 1);
            check(CAEN_DGTZ_SetPostTriggerSize(digi.handle(), percent));
            } break;
        case CommonCommand::TriggerPolarity: {
            const auto& [polarity] = parse_arguments<CAEN_DGTZ_TriggerPolarity_t>(tokens, 1);
            // Channel argument might be ignored?
            for (UIntType channel = 0; channel < N_Channels; channel++) {
                check(CAEN_DGTZ_SetTriggerPolarity(digi.handle(), channel, polarity));
            }
            } break;
        case CommonCommand::EnableExternalTrigger: {
            const auto& [mode] = parse_arguments<CAEN_DGTZ_TriggerMode_t>(tokens, 1);
            check(CAEN_DGTZ_SetExtTriggerInputMode(digi.handle(), mode));
            } break;
        case CommonCommand::FastTrigger: {
            const auto& [mode] = parse_arguments<CAEN_DGTZ_TriggerMode_t>(tokens, 1);
            check(CAEN_DGTZ_SetFastTriggerMode(digi.handle(), mode));
            } break;
        case CommonCommand::CorrectionLevel: {
            // SAM correction level?
            } break;
        case CommonCommand::Frequency: {
            const auto& [freq] = parse_arguments<CAEN_DGTZ_DRS4Frequency_t>(tokens, 1);
            check(CAEN_DGTZ_SetDRS4SamplingFrequency(digi.handle(), freq));
            } break;
        case CommonCommand::FPIOLevel: {
            const auto& [level] = parse_arguments<CAEN_DGTZ_IOLevel_t>(tokens, 1);
            check(CAEN_DGTZ_SetIOLevel(digi.handle(), level));
            } break;
        case CommonCommand::EnableInput: {
            const auto& [enable] = parse_arguments<bool>(tokens, 1);
            if (enable) {
                check(CAEN_DGTZ_SetGroupEnableMask(digi.handle(), 0b11));
            } else {
                check(CAEN_DGTZ_SetGroupEnableMask(digi.handle(), 0b00));
            }
            } break;
        case CommonCommand::DigitizeFastTrigger: {
            const auto& [enadis] = parse_arguments<CAEN_DGTZ_EnaDis_t>(tokens, 1);
            check(CAEN_DGTZ_SetFastTriggerDigitizing(digi.handle(), enadis));
            } break;
    }
}

void run_group(Digitizer& digi, UIntType group, const std::vector<std::string>& tokens) {
    auto lookup_result = GroupCommandTable.lookup<CommandValue::Name>(tokens[0]);
    if (!lookup_result) throw std::runtime_error("Unrecognized group command " + tokens[0]);
    
    switch (lookup_result->first) {
        case GroupCommand::EnableInput: {
            const auto& [enable] = parse_arguments<bool>(tokens, 1);
            UIntType group_mask;
            check(CAEN_DGTZ_GetGroupEnableMask(digi.handle(), &group_mask));
            if (enable) {
                group_mask |= 0b1 << group;
            } else {
                group_mask &= ~(0b1 << group);
            }
            check(CAEN_DGTZ_SetGroupEnableMask(digi.handle(), group_mask));
            } break;
        case GroupCommand::ChannelDcOffsets: {
            const auto& [c0, c1, c2, c3, c4, c5, c6, c7] = 
                parse_arguments<UIntType, UIntType, UIntType, UIntType,
                                UIntType, UIntType, UIntType, UIntType>(tokens, 1);
            check(CAEN_DGTZ_SetChannelDCOffset(digi.handle(), group * N_Channels + 0, c0));
            check(CAEN_DGTZ_SetChannelDCOffset(digi.handle(), group * N_Channels + 1, c1));
            check(CAEN_DGTZ_SetChannelDCOffset(digi.handle(), group * N_Channels + 2, c2));
            check(CAEN_DGTZ_SetChannelDCOffset(digi.handle(), group * N_Channels + 3, c3));
            check(CAEN_DGTZ_SetChannelDCOffset(digi.handle(), group * N_Channels + 4, c4));
            check(CAEN_DGTZ_SetChannelDCOffset(digi.handle(), group * N_Channels + 5, c5));
            check(CAEN_DGTZ_SetChannelDCOffset(digi.handle(), group * N_Channels + 6, c6));
            check(CAEN_DGTZ_SetChannelDCOffset(digi.handle(), group * N_Channels + 7, c7));
            } break;
    }
}

void run_trigger(Digitizer& digi, const std::vector<std::string>& tokens) {
    auto lookup_result = TriggerCommandTable.lookup<CommandValue::Name>(tokens[0]);
    if (!lookup_result) throw std::runtime_error("Unrecognized trigger command " + tokens[0]);
    
    switch (lookup_result->first) {
        case TriggerCommand::DcOffset: {
            const auto& [offset] = parse_arguments<UIntType>(tokens, 1);
            check(CAEN_DGTZ_SetGroupFastTriggerDCOffset(digi.handle(), 0, offset));
            check(CAEN_DGTZ_SetGroupFastTriggerDCOffset(digi.handle(), 1, offset));
            } break;
        case TriggerCommand::Threshold: {
            const auto& [threshold] = parse_arguments<UIntType>(tokens, 1);
            check(CAEN_DGTZ_SetGroupFastTriggerThreshold(digi.handle(), 0, threshold));
            check(CAEN_DGTZ_SetGroupFastTriggerThreshold(digi.handle(), 0, threshold));
            } break;
    }   
}

void run_setup(std::istream& input, Digitizer& digi) {
    CommandType command_type = CommandType::Common;

    std::optional<UIntType> group = std::nullopt;

    std::string line;
    while (std::getline(input, line)) {
        std::vector<std::string> tokens = tokenize(line);
        if (tokens.empty()) {
            continue;
        } else if (tokens[0] == "[COMMON]") {
            command_type = CommandType::Common;
            group = std::nullopt;
        } else if (tokens[0] == "[0]") {
            command_type = CommandType::Group;
            group = 0;
        } else if (tokens[0] == "[1]") {
            command_type = CommandType::Group;
            group = 1;
        } else if (tokens[0] == "[TR0]") {
            command_type = CommandType::Trigger;
            group = std::nullopt;
        } else {

            try {
                switch (command_type) {
                    case CommandType::Common:
                        run_common(digi, tokens);
                        break;
                    case CommandType::Group:
                        run_group(digi, *group, tokens);
                        break;
                    case CommandType::Trigger:
                        run_trigger(digi, tokens);
                        break;
                }
            } catch(const std::runtime_error& e) {
                digi.log() << "[ERROR]: While parsing argument \'" << line << "\": " << e.what() << "\n";
            }
        }
    }
}
