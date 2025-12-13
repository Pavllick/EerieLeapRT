#include <iterator>
#include <optional>
#include <variant>
#include <unordered_map>
#include <cassert>

#include <boost/variant.hpp>

#include "dbcppp/Network.h"

#include "dbc_logger.h"

#include "DBCX3.h"

using namespace dbcppp;
using namespace dbcppp::DBCX3::AST;

LOG_MODULE_DECLARE(dbc_logger);

namespace
{
using AttributeList = std::vector<variant_attribute_t const*>;
using Description = boost::variant<G_ValueDescriptionSignal, G_ValueDescriptionEnvVar>;

struct SignalCache
{
    AttributeList Attributes;
    Description const* Description_ = nullptr;
    variant_comment_t const* Comment = nullptr;
};

struct MessageCache
{
    std::unordered_map<std::string_view, SignalCache> Signals;
    AttributeList Attributes;
    variant_comment_t const* Comment = nullptr;
};

struct EnvVarCache
{
    AttributeList Attributes;
    Description const* Description_;
    variant_comment_t const* Comment = nullptr;
};

struct NodeCache
{
    AttributeList Attributes;
    variant_comment_t const* Comment = nullptr;
};

struct Cache
{
    AttributeList NetworkAttributes;
    variant_comment_t const* NetworkComment = nullptr;
    std::unordered_map<std::string_view, EnvVarCache> EnvVars;
    std::unordered_map<std::string_view, NodeCache> Nodes;
    std::unordered_map<uint64_t, MessageCache> Messages;
};

} // anon

static auto getVersion(std::pmr::memory_resource* mr, const G_Network& gnet)
{
    return std::pmr::string(gnet.version.version, mr);
}
static auto getNewSymbols(std::pmr::memory_resource* mr, const G_Network& gnet)
{
    std::pmr::vector<std::pmr::string> nodes(mr);
    for (const auto& ns : gnet.new_symbols)
    {
        nodes.emplace_back(std::pmr::string(ns, mr));
    }
    return nodes;
}
static std::optional<SignalType> getSignalType(std::pmr::memory_resource* mr, const G_Network& gnet, const G_ValueTable& vt)
{
    auto iter = std::ranges::find_if(gnet.signal_types.begin(), gnet.signal_types.end(),
        [&](const auto& st)
        {
            return st.value_table_name == vt.name;
        });
    if (iter != gnet.signal_types.end())
    {
        auto& st = *iter;
        return SignalType(
              std::allocator_arg
            , mr
            , std::pmr::string(st.name, mr)
            , st.size
            , st.byte_order == '0' ? Signal::EByteOrder::BigEndian : Signal::EByteOrder::LittleEndian
            , st.value_type == '+' ? Signal::EValueType::Unsigned : Signal::EValueType::Signed
            , st.factor
            , st.offset
            , st.minimum
            , st.maximum
            , std::pmr::string(st.unit, mr)
            , st.default_value
            , std::pmr::string(st.value_table_name, mr));
    }
    return std::nullopt;
}
static auto getValueTables(std::pmr::memory_resource* mr, const G_Network& gnet)
{
    std::pmr::vector<ValueTable> value_tables(mr);
    for (const auto& vt : gnet.value_tables)
    {
        auto sig_type = getSignalType(mr, gnet, vt);
        std::pmr::vector<ValueEncodingDescription> copy_ved(mr);
        for (const auto& ved : vt.value_encoding_descriptions)
        {
            auto desc = std::pmr::string(ved.description, mr);
            copy_ved.emplace_back(ved.value, std::move(desc));
        }
        value_tables.emplace_back(std::pmr::string(vt.name), std::move(sig_type), std::move(copy_ved));
    }
    return value_tables;
}
static BitTiming getBitTiming(const G_Network& gnet)
{
    if (gnet.bit_timing)
    {
        return {gnet.bit_timing->baudrate, gnet.bit_timing->BTR1, gnet.bit_timing->BTR2};
    }

    return {};
}

template <class Variant>
class Visitor
    : public boost::static_visitor<void>
{
public:
    Visitor(Variant& var)
        : _var(var)
    {}
    template <class T>
    void operator()(const T& v)
    {
        _var = v;
    }

private:
    Variant& _var;
};

template <class... Args>
auto boost_variant_to_std_variant(const boost::variant<Args...>& old)
{
    using var_t = std::variant<Args...>;
    var_t new_;
    Visitor<var_t> visitor(new_);
    old.apply_visitor(visitor);
    return new_;
}

inline auto boost_variant_to_std_variant(variant_attr_value_t const& attr)
{
    Attribute::value_t value;

    switch (attr.which()) {
    case 0: {
        Attribute::value_t v(std::in_place_type<int64_t>, boost::get<int64_t>(attr));
        value.swap(v);
    } break;
    case 1: {
        Attribute::value_t v(std::in_place_type<double>, boost::get<double>(attr));
        value.swap(v);
    } break;
    case 2: {
        Attribute::value_t v(std::in_place_type<std::pmr::string>, boost::get<std::pmr::string>(attr));
        value.swap(v);
    } break;
    default:
        assert(false && "Unhandled variant member");
        break;
    }

    return value;
}

static auto getAttributeValues(std::pmr::memory_resource* mr, const G_Network& gnet, const G_Node& n, Cache const& cache)
{
    std::pmr::vector<Attribute> attribute_values(mr);

    auto node_it = cache.Nodes.find(n.name);

    if (node_it != cache.Nodes.end()) {
        attribute_values.reserve(node_it->second.Attributes.size());

        for (auto av : node_it->second.Attributes) {
            auto const& attr = boost::get<G_AttributeNode>(*av);
            auto name = attr.attribute_name;
            auto value{boost_variant_to_std_variant(attr.value)};
            attribute_values.emplace_back(
                  std::pmr::string(name, mr)
                , AttributeDefinition::EObjectType::Node
                , std::move(value));
        }
    }

    return attribute_values;
}
static auto getComment(std::pmr::memory_resource* mr, const G_Network& gnet, const G_Node& n, Cache const& cache)
{
    std::pmr::string comment(mr);
    auto node_it = cache.Nodes.find(n.name);

    if (node_it != cache.Nodes.end()) {
        if (node_it->second.Comment) {
            comment = boost::get<G_CommentNode>(*node_it->second.Comment).comment;
        }
    }

    return comment;
}
static auto getNodes(std::pmr::memory_resource* mr, const G_Network& gnet, Cache const& cache)
{
    std::pmr::vector<Node> nodes(mr);
    for (const auto& n : gnet.nodes)
    {
        auto comment = getComment(mr, gnet, n, cache);
        auto attribute_values = getAttributeValues(mr, gnet, n, cache);
        nodes.emplace_back(
              std::pmr::string(n.name, mr)
            , std::move(comment)
            , std::move(attribute_values));
    }
    return nodes;
}
static auto getAttributeValues(std::pmr::memory_resource* mr, const G_Network& gnet, const G_Message& m, const G_Signal& s, Cache const& cache)
{
    std::pmr::vector<Attribute> attribute_values(mr);
    auto const message_it = cache.Messages.find(m.id);

    if (message_it != cache.Messages.end()) {
        auto const signal_it = message_it->second.Signals.find(s.name);

        if (signal_it != message_it->second.Signals.end()) {
            attribute_values.reserve(signal_it->second.Attributes.size());

            for (auto av : signal_it->second.Attributes)
            {
                auto const& attr = boost::get<G_AttributeSignal>(*av);
                auto value{boost_variant_to_std_variant(attr.value)};
                attribute_values.emplace_back(
                      std::pmr::string(attr.attribute_name, mr)
                    , AttributeDefinition::EObjectType::Signal
                    , std::move(value));
            }
        }

    }

    return attribute_values;
}
static auto getValueDescriptions(std::pmr::memory_resource* mr, const G_Network& gnet, const G_Message& m, const G_Signal& s, Cache const& cache)
{
    std::pmr::vector<ValueEncodingDescription> value_descriptions(mr);
    auto const message_it = cache.Messages.find(m.id);

    if (message_it != cache.Messages.end()) {
        auto const signal_it = message_it->second.Signals.find(s.name);

        if (signal_it != message_it->second.Signals.end()) {
            if (signal_it->second.Description_) {
                auto const& vds = boost::get<G_ValueDescriptionSignal>(*signal_it->second.Description_).value_descriptions;

                value_descriptions.reserve(vds.size());

                for (const auto& vd : vds)
                {
                    auto desc = std::pmr::string(vd.description, mr);
                    value_descriptions.emplace_back(vd.value, std::move(desc));
                }
            }
        }
    }
    return value_descriptions;
}
static auto getComment(std::pmr::memory_resource* mr, const G_Network& gnet, const G_Message& m, const G_Signal& s, Cache const& cache)
{
    std::pmr::string comment(mr);
    auto message_it = cache.Messages.find(m.id);

    if (message_it != cache.Messages.end()) {
        auto signal_it = message_it->second.Signals.find(s.name);

        if (signal_it != message_it->second.Signals.end()) {
            if (signal_it->second.Comment) {
                comment = boost::get<G_CommentSignal>(*signal_it->second.Comment).comment;
            }
        }

    }
    return comment;
}
static auto getSignalExtendedValueType(const G_Network& gnet, const G_Message& m, const G_Signal& s)
{
    Signal::EExtendedValueType extended_value_type = Signal::EExtendedValueType::Integer;
    auto iter = std::ranges::find_if(gnet.signal_extended_value_types.begin(), gnet.signal_extended_value_types.end(),
        [&](const G_SignalExtendedValueType& sev)
        {
            return sev.message_id == m.id && sev.signal_name == s.name;
        });
    if (iter != gnet.signal_extended_value_types.end())
    {
        switch (iter->value)
        {
        case 1: extended_value_type = Signal::EExtendedValueType::Float; break;
        case 2: extended_value_type = Signal::EExtendedValueType::Double; break;
        }
    }
    return extended_value_type;
}
static auto getSignalMultiplexerValues(std::pmr::memory_resource* mr, const G_Network& gnet, const std::pmr::string& s, const uint64_t m)
{
    std::pmr::vector<SignalMultiplexerValue> signal_multiplexer_values(mr);
    for (const auto& gsmv : gnet.signal_multiplexer_values)
    {
        if (gsmv.signal_name == s && gsmv.message_id == m)
        {
            auto switch_name = gsmv.switch_name;
            std::pmr::vector<SignalMultiplexerValue::Range> value_ranges(mr);
            for (const auto& r : gsmv.value_ranges)
            {
                value_ranges.push_back({r.from, r.to});
            }
            signal_multiplexer_values.emplace_back(
                  std::pmr::string(switch_name, mr)
                , std::move(value_ranges));
        }
    }
    return signal_multiplexer_values;
}
static auto getSignals(std::pmr::memory_resource* mr, const G_Network& gnet, const G_Message& m, Cache const& cache)
{
    std::pmr::vector<Signal> signals(mr);

    signals.reserve(m.signals.size());

    for (const G_Signal& s : m.signals)
    {
        std::pmr::vector<std::pmr::string> receivers(mr);
        auto attribute_values = getAttributeValues(mr, gnet, m, s, cache);
        auto value_descriptions = getValueDescriptions(mr, gnet, m, s, cache);
        auto extended_value_type = getSignalExtendedValueType(gnet, m, s);
        auto multiplexer_indicator = Signal::EMultiplexer::NoMux;
        auto comment = getComment(mr, gnet, m, s, cache);
        auto signal_multiplexer_values = getSignalMultiplexerValues(
            mr, gnet, std::pmr::string(s.name, mr), m.id);
        uint64_t multiplexer_switch_value = 0;
        if (s.multiplexer_indicator)
        {
            auto m = *s.multiplexer_indicator;
            if (m.substr(0, 1) == "M")
            {
                multiplexer_indicator = Signal::EMultiplexer::MuxSwitch;
            }
            else
            {
                multiplexer_indicator = Signal::EMultiplexer::MuxValue;
                std::pmr::string value(m.substr(1, m.size()), mr);
                multiplexer_switch_value = std::atoi(value.c_str());
            }
        }

        receivers.reserve(s.receivers.size());

        for (const auto& n : s.receivers)
        {
            receivers.emplace_back(std::pmr::string(n, mr));
        }

        signals.emplace_back(
              m.size
            , std::pmr::string(s.name, mr)
            , multiplexer_indicator
            , multiplexer_switch_value
            , s.start_bit
            , s.signal_size
            , s.byte_order == '0' ? Signal::EByteOrder::BigEndian : Signal::EByteOrder::LittleEndian
            , s.value_type == '+' ? Signal::EValueType::Unsigned : Signal::EValueType::Signed
            , s.factor
            , s.offset
            , s.minimum
            , s.maximum
            , std::pmr::string(s.unit, mr)
            , std::move(receivers)
            , std::move(attribute_values)
            , std::move(value_descriptions)
            , std::move(comment)
            , extended_value_type
            , std::move(signal_multiplexer_values));
        if (signals.back().Error(Signal::EErrorCode::SignalExceedsMessageSize))
        {
            LOG_DBG("The signals '%s::%s' start_bit + bit_size exceeds the byte size of the message! Ignoring this error will lead to garbage data when using the decode function of this signal.", m.name.c_str(), s.name.c_str());
        }
        if (signals.back().Error(Signal::EErrorCode::WrongBitSizeForExtendedDataType))
        {
            LOG_DBG("The signals '%s::%s' bit_size does not fit the bit size of the specified ExtendedValueType.", m.name.c_str(), s.name.c_str());
        }
        if (signals.back().Error(Signal::EErrorCode::MaschinesFloatEncodingNotSupported))
        {
            LOG_DBG("Signal '%s::%s' This warning appears when a signal uses type float but the system this programm is running on does not uses IEEE 754 encoding for floats.", m.name.c_str(), s.name.c_str());
        }
        if (signals.back().Error(Signal::EErrorCode::MaschinesDoubleEncodingNotSupported))
        {
            LOG_DBG("Signal '%s::%s' This warning appears when a signal uses type double but the system this programm is running on does not uses IEEE 754 encoding for doubles.", m.name.c_str(), s.name.c_str());
        }
    }
    return signals;
}
static auto getMessageTransmitters(std::pmr::memory_resource* mr, const G_Network& gnet, const G_Message& m)
{
    std::pmr::vector<std::pmr::string> message_transmitters(mr);
    auto iter_mt = std::ranges::find_if(gnet.message_transmitters.begin(), gnet.message_transmitters.end(),
        [&](const G_MessageTransmitter& mt)
        {
            return mt.id == m.id;
        });
    if (iter_mt != gnet.message_transmitters.end())
    {
        for (const auto& t : iter_mt->transmitters)
        {
            message_transmitters.emplace_back(std::pmr::string(t, mr));
        }
    }
    return message_transmitters;
}
static auto getAttributeValues(std::pmr::memory_resource* mr, const G_Network& gnet, const G_Message& m, Cache const& cache)
{
    std::pmr::vector<Attribute> attribute_values(mr);

    auto message_it = cache.Messages.find(m.id);

    if (message_it != cache.Messages.end()) {
        attribute_values.reserve(message_it->second.Attributes.size());

        for (auto av: message_it->second.Attributes) {
            auto const& attr = boost::get<G_AttributeMessage>(*av);
            auto value{boost_variant_to_std_variant(attr.value)};
            attribute_values.emplace_back(
                  std::pmr::string(attr.attribute_name, mr)
                , AttributeDefinition::EObjectType::Message
                , std::move(value));
        }
    }
    return attribute_values;
}
static auto getComment(std::pmr::memory_resource* mr, const G_Network& gnet, const G_Message& m, Cache const& cache)
{
    std::pmr::string comment(mr);
    auto message_it = cache.Messages.find(m.id);

    if (message_it != cache.Messages.end()) {
        if (message_it->second.Comment) {
            comment = boost::get<G_CommentMessage>(*message_it->second.Comment).comment;
        }
    }
    return comment;
}
static auto getSignalGroups(std::pmr::memory_resource* mr, const G_Network& gnet, const G_Message& m)
{
    std::pmr::vector<SignalGroup> signal_groups(mr);
    for (const auto& sg : gnet.signal_groups)
    {
        if (sg.message_id == m.id)
        {
            auto name = sg.signal_group_name;
            auto signal_names = sg.signal_names;
            signal_groups.emplace_back(
                  sg.message_id
                , std::move(std::pmr::string(name, mr))
                , sg.repetitions
                , std::move(std::pmr::vector<std::pmr::string>(signal_names.begin(), signal_names.end(), mr)));
        }
    }
    return signal_groups;
}
static auto getMessages(std::pmr::memory_resource* mr, const G_Network& gnet, Cache const& cache)
{
    std::pmr::vector<Message> messages(mr);

    messages.reserve(cache.Messages.size());

    for (const auto& m : gnet.messages)
    {
        auto message_transmitters = getMessageTransmitters(mr, gnet, m);
        auto signals = getSignals(mr, gnet, m, cache);
        auto attribute_values = getAttributeValues(mr, gnet, m, cache);
        auto comment = getComment(mr, gnet, m, cache);
        auto signal_groups = getSignalGroups(mr, gnet, m);
        messages.emplace_back(
              m.id
            , std::pmr::string(m.name, mr)
            , m.size
            , std::pmr::string(m.transmitter, mr)
            , std::move(message_transmitters)
            , std::move(signals)
            , std::move(attribute_values)
            , std::move(comment)
            , std::move(signal_groups));
        if (messages.back().Error() == Message::EErrorCode::MuxValeWithoutMuxSignal)
        {
            LOG_DBG("Message '%s' does have mux value but no mux signal!", messages.back().Name());
        }
    }
    return messages;
}
static auto getValueDescriptions(std::pmr::memory_resource* mr, const G_Network& gnet, const G_EnvironmentVariable& ev, Cache const& cache)
{
    std::pmr::vector<ValueEncodingDescription> value_descriptions(mr);
    auto env_it = cache.EnvVars.find(ev.name);

    if (env_it != cache.EnvVars.end()) {
        if (env_it->second.Description_) {
            auto const& vds = boost::get<G_ValueDescriptionEnvVar>(*env_it->second.Description_).value_descriptions;

            value_descriptions.reserve(vds.size());

            for (const auto& vd : vds)
            {
                auto desc = std::pmr::string(vd.description, mr);
                value_descriptions.emplace_back(vd.value, std::move(desc));
            }
        }
    }
    return value_descriptions;
}
static auto getAttributeValues(std::pmr::memory_resource* mr, const G_Network& gnet, const G_EnvironmentVariable& ev, const Cache& cache)
{
    std::pmr::vector<Attribute> attribute_values(mr);

    auto env_it = cache.EnvVars.find(ev.name);

    if (env_it != cache.EnvVars.end()) {
        attribute_values.reserve(env_it->second.Attributes.size());

        for (auto av : env_it->second.Attributes) {
            auto const& attr = boost::get<G_AttributeEnvVar>(*av);
            auto value = boost_variant_to_std_variant(attr.value);
            attribute_values.emplace_back(
                  std::pmr::string(attr.attribute_name, mr)
                , AttributeDefinition::EObjectType::EnvironmentVariable
                , std::move(value));
        }
    }

    return attribute_values;
}
static auto getComment(std::pmr::memory_resource* mr, const G_Network& gnet, const G_EnvironmentVariable& ev, Cache const& cache)
{
    std::pmr::string comment(mr);
    auto envvar_it = cache.EnvVars.find(ev.name);

    if (envvar_it != cache.EnvVars.end()) {
        if (envvar_it->second.Comment) {
            comment = boost::get<G_CommentEnvVar>(*envvar_it->second.Comment).comment;
        }
    }
    return comment;
}
static auto getEnvironmentVariables(std::pmr::memory_resource* mr, const G_Network& gnet, Cache const& cache)
{
    std::pmr::vector<EnvironmentVariable> environment_variables(mr);
    for (const auto& ev : gnet.environment_variables)
    {
        EnvironmentVariable::EVarType var_type;
        EnvironmentVariable::EAccessType access_type;
        std::pmr::vector<std::pmr::string> access_nodes = std::pmr::vector<std::pmr::string>(ev.access_nodes.begin(), ev.access_nodes.end(), mr);
        auto value_descriptions = getValueDescriptions(mr, gnet, ev, cache);
        auto attribute_values = getAttributeValues(mr, gnet, ev, cache);
        auto comment = getComment(mr, gnet, ev, cache);
        uint64_t data_size = 0;
        switch (ev.var_type)
        {
        case 0: var_type = EnvironmentVariable::EVarType::Integer; break;
        case 1: var_type = EnvironmentVariable::EVarType::Float; break;
        case 2: var_type = EnvironmentVariable::EVarType::String; break;
        }
        access_type = EnvironmentVariable::EAccessType::Unrestricted;
        if (ev.access_type == "DUMMY_NODE_VECTOR0")         access_type = EnvironmentVariable::EAccessType::Unrestricted;
        else if (ev.access_type == "DUMMY_NODE_VECTOR1")    access_type = EnvironmentVariable::EAccessType::Read;
        else if (ev.access_type == "DUMMY_NODE_VECTOR2")    access_type = EnvironmentVariable::EAccessType::Write;
        else if (ev.access_type == "DUMMY_NODE_VECTOR3")    access_type = EnvironmentVariable::EAccessType::ReadWrite;
        else if (ev.access_type == "DUMMY_NODE_VECTOR8000") access_type = EnvironmentVariable::EAccessType::Unrestricted_;
        else if (ev.access_type == "DUMMY_NODE_VECTOR8001") access_type = EnvironmentVariable::EAccessType::Read_;
        else if (ev.access_type == "DUMMY_NODE_VECTOR8002") access_type = EnvironmentVariable::EAccessType::Write_;
        else if (ev.access_type == "DUMMY_NODE_VECTOR8003") access_type = EnvironmentVariable::EAccessType::ReadWrite_;
        for (auto& evd : gnet.environment_variable_datas)
        {
            if (evd.name == ev.name)
            {
                var_type = EnvironmentVariable::EVarType::Data;
                data_size = evd.size;
                break;
            }
        }
        environment_variables.emplace_back(
              std::pmr::string(ev.name, mr)
            , var_type
            , ev.minimum
            , ev.maximum
            , std::pmr::string(ev.unit, mr)
            , ev.initial_value
            , ev.id
            , access_type
            , std::move(access_nodes)
            , std::move(value_descriptions)
            , data_size
            , std::move(attribute_values)
            , std::move(comment));
    }
    return environment_variables;
}
static auto getAttributeDefinitions(std::pmr::memory_resource* mr, const G_Network& gnet)
{
    std::pmr::vector<AttributeDefinition> attribute_definitions(mr);
    struct VisitorValueType
    {
        AttributeDefinition::value_type_t operator()(const G_AttributeValueTypeInt& cn)
        {
            AttributeDefinition::ValueTypeInt vt;
            vt.minimum = cn.minimum;
            vt.maximum = cn.maximum;
            return vt;
        }
        AttributeDefinition::value_type_t operator()(const G_AttributeValueTypeHex& cn)
        {
            AttributeDefinition::ValueTypeHex vt;
            vt.minimum = cn.minimum;
            vt.maximum = cn.maximum;
            return vt;
        }
        AttributeDefinition::value_type_t operator()(const G_AttributeValueTypeFloat& cn)
        {
            AttributeDefinition::ValueTypeFloat vt;
            vt.minimum = cn.minimum;
            vt.maximum = cn.maximum;
            return vt;
        }
        AttributeDefinition::value_type_t operator()(const G_AttributeValueTypeString& cn)
        {
            return AttributeDefinition::ValueTypeString();
        }
        AttributeDefinition::value_type_t operator()(const G_AttributeValueTypeEnum& cn)
        {
            AttributeDefinition::ValueTypeEnum vt;
            for (auto& e : cn.values)
            {
                vt.values.emplace_back(e);
            }
            return vt;
        }
    };

    for (const auto& ad : gnet.attribute_definitions)
    {
        AttributeDefinition::EObjectType object_type;
        auto cvt = ad.value_type;
        if (!ad.object_type)
        {
            object_type = AttributeDefinition::EObjectType::Network;
        }
        else if (*ad.object_type == "BU_")
        {
            object_type = AttributeDefinition::EObjectType::Node;
        }
        else if (*ad.object_type == "BO_")
        {
            object_type = AttributeDefinition::EObjectType::Message;
        }
        else if (*ad.object_type == "SG_")
        {
            object_type = AttributeDefinition::EObjectType::Signal;
        }
        else
        {
            object_type = AttributeDefinition::EObjectType::EnvironmentVariable;
        }
        VisitorValueType vvt;
        auto value = boost_variant_to_std_variant(cvt.value);
        std::visit(vvt, value);
        attribute_definitions.emplace_back(
            std::move(std::pmr::string(ad.name, mr))
            , object_type
            , std::visit(vvt, value));
    }
    return attribute_definitions;
}
static auto getAttributeDefaults(std::pmr::memory_resource* mr, const G_Network& gnet)
{
    std::pmr::vector<Attribute> attribute_defaults(mr);
    for (auto& ad : gnet.attribute_defaults)
    {
        auto value = boost_variant_to_std_variant(ad.value);
        attribute_defaults.emplace_back(
              std::pmr::string(ad.name, mr)
            , AttributeDefinition::EObjectType::Network
            , std::move(value));
    }
    return attribute_defaults;
}
static auto getAttributeValues(std::pmr::memory_resource* mr, const G_Network& gnet, Cache const& cache)
{
    std::pmr::vector<Attribute> attribute_values(mr);

    attribute_values.reserve(cache.NetworkAttributes.size());

    for (auto av : cache.NetworkAttributes)
    {
        auto const& attr = boost::get<G_AttributeNetwork>(*av);
        auto value{boost_variant_to_std_variant(attr.value)};
        attribute_values.emplace_back(
              std::pmr::string(attr.attribute_name)
            , AttributeDefinition::EObjectType::Network
            , std::move(value));
    }
    return attribute_values;
}
static auto getComment(std::pmr::memory_resource* mr, const G_Network& gnet, Cache const& cache)
{
    std::pmr::string comment(mr);
    if (cache.NetworkComment) {
        comment = boost::get<G_CommentNetwork>(*cache.NetworkComment).comment;
    }
    return comment;
}

std::shared_ptr<Network> DBCAST2Network(std::pmr::memory_resource* mr, const G_Network& gnet)
{
    Cache cache;

    for (const auto& av : gnet.attribute_values)
    {
        switch (av.which()) {
        case 0: {
            auto const& attr = boost::get<G_AttributeNetwork>(av);
            cache.NetworkAttributes.emplace_back(&av);
        } break;
        case 1: {
            auto const& attr = boost::get<G_AttributeNode>(av);
            auto node_it = cache.Nodes.find(attr.node_name);
            if (node_it == cache.Nodes.end()) {
                node_it = cache.Nodes.emplace(attr.node_name, NodeCache()).first;
            }
            node_it->second.Attributes.emplace_back(&av);
        } break;
        case 2: {
            auto const& attr = boost::get<G_AttributeMessage>(av);
            auto it = cache.Messages.find(attr.message_id);
            if (it == cache.Messages.end()) {
                it = cache.Messages.emplace(attr.message_id, MessageCache()).first;
            }

            it->second.Attributes.emplace_back(&av);
        } break;
        case 3: {
            auto const& attr = boost::get<G_AttributeSignal>(av);
            auto message_it = cache.Messages.find(attr.message_id);
            if (message_it == cache.Messages.end()) {
                message_it = cache.Messages.emplace(attr.message_id, MessageCache()).first;
            }

            auto& signals = message_it->second.Signals;
            auto signal_it = signals.find(attr.signal_name);
            if (signal_it == signals.end()) {
                signal_it = signals.emplace(attr.signal_name, SignalCache()).first;
            }

            signal_it->second.Attributes.emplace_back(&av);
        } break;
        case 4: {
            auto const& attr = boost::get<G_AttributeEnvVar>(av);
            auto envvar_it = cache.EnvVars.find(attr.env_var_name);
            if (envvar_it == cache.EnvVars.end()) {
                envvar_it = cache.EnvVars.emplace(attr.env_var_name, EnvVarCache()).first;
            }
            envvar_it->second.Attributes.emplace_back(&av);
        } break;
        default:
            assert(false && "Unhandled variant member");
            break;
        }
    }

    for (const auto& vd : gnet.value_descriptions_sig_env_var)
    {
        switch (vd.description.which()) {
        case 0: {
            auto const& desc = boost::get<G_ValueDescriptionSignal>(vd.description);
            auto message_it = cache.Messages.find(desc.message_id);
            if (message_it == cache.Messages.end()) {
                message_it = cache.Messages.emplace(desc.message_id, MessageCache()).first;
            }

            auto& signals = message_it->second.Signals;
            auto signal_it = signals.find(desc.signal_name);
            if (signal_it == signals.end()) {
                signal_it = signals.emplace(desc.signal_name, SignalCache()).first;
            }

            signal_it->second.Description_ = &vd.description;
        } break;
        case 1: {
            auto const& desc = boost::get<G_ValueDescriptionEnvVar>(vd.description);
            auto envvar_it = cache.EnvVars.find(desc.env_var_name);
            if (envvar_it == cache.EnvVars.end()) {
                envvar_it = cache.EnvVars.emplace(desc.env_var_name, EnvVarCache()).first;
            }
            envvar_it->second.Description_ = &vd.description;
        } break;
        default:
            assert(false && "Unhandled variant member");
            break;
        }
    }

    for (const auto& comment : gnet.comments)
    {
        switch (comment.comment.which()) {
        case 0: {
            auto const& c = boost::get<G_CommentNetwork>(comment.comment);
            cache.NetworkComment = &comment.comment;
        } break;
        case 1: {
            auto const& c = boost::get<G_CommentNode>(comment.comment);
            auto node_it = cache.Nodes.find(c.node_name);
            if (node_it == cache.Nodes.end()) {
                node_it = cache.Nodes.emplace(c.node_name, NodeCache()).first;
            }
            node_it->second.Comment = &comment.comment;
        } break;
        case 2: {
            auto const& c = boost::get<G_CommentMessage>(comment.comment);
            auto message_it = cache.Messages.find(c.message_id);
            if (message_it == cache.Messages.end()) {
                message_it = cache.Messages.emplace(c.message_id, MessageCache()).first;
            }
            message_it->second.Comment = &comment.comment;
        } break;
        case 3: {
            auto const& c = boost::get<G_CommentSignal>(comment.comment);
            auto message_it = cache.Messages.find(c.message_id);
            if (message_it == cache.Messages.end()) {
                message_it = cache.Messages.emplace(c.message_id, MessageCache()).first;
            }

            auto signal_it = message_it->second.Signals.find(c.signal_name);
            if (signal_it == message_it->second.Signals.end()) {
                signal_it = message_it->second.Signals.emplace(c.signal_name, SignalCache()).first;
            }
            signal_it->second.Comment = &comment.comment;
        } break;
        case 4: {
            auto const& c = boost::get<G_CommentEnvVar>(comment.comment);
            auto envvar_it = cache.EnvVars.find(c.env_var_name);
            if (envvar_it == cache.EnvVars.end()) {
                envvar_it = cache.EnvVars.emplace(c.env_var_name, EnvVarCache()).first;
            }
            envvar_it->second.Comment = &comment.comment;
        } break;
        default:
            assert(false && "Unhandled variant member");
            break;
        }
    }

    return std::allocate_shared<Network>(
        std::pmr::polymorphic_allocator<Network>(mr)
        , getVersion(mr, gnet)
        , getNewSymbols(mr, gnet)
        , getBitTiming(gnet)
        , getNodes(mr, gnet, cache)
        , getValueTables(mr, gnet)
        , getMessages(mr, gnet, cache)
        , getEnvironmentVariables(mr, gnet, cache)
        , getAttributeDefinitions(mr, gnet)
        , getAttributeDefaults(mr, gnet)
        , getAttributeValues(mr, gnet, cache)
        , getComment(mr, gnet, cache));
}

std::shared_ptr<Network> Network::LoadDBCFromIs(std::pmr::memory_resource* mr, std::istream& is)
{
    std::pmr::string str(mr);
    str.assign((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    if (auto gnet = dbcppp::DBCX3::ParseFromMemory(mr, str.c_str(), str.c_str() + str.size()))
    {
        return DBCAST2Network(mr, *gnet);
    }
    return nullptr;
}
