#include "dbcppp/Network2Functions.h"
#include "dbcppp/Network.h"

using namespace dbcppp;
using namespace dbcppp::Network2DBC;

std::ostream& dbcppp::Network2DBC::operator<<(std::ostream& os, const na_t& na)
{
    const auto& net = std::get<0>(na);
    const auto& iattr = std::get<1>(na);
    struct Visitor
    {
        Visitor(std::ostream& os)
            : _os(os)
        {}
        void operator()(int64_t i) const
        {
            _os << " " << i;
        }
        void operator()(double d) const
        {
            _os << " " << d;
        }
        void operator()(const std::pmr::string& s) const
        {
            _os << " \"" << s << "\"";
        }

        std::ostream& _os;
    };
    std::string cmd = "BA_";
    {
        auto beg = net.AttributeDefaults().begin();
        auto end = net.AttributeDefaults().end();
        if (std::find_if(beg, end, [&](const Attribute& attr) { return &attr == &iattr; }) != end)
        {
            cmd = "BA_DEF_DEF_";
        }
    }
    os << cmd << " \"" << iattr.Name() << "\"";
    switch (iattr.ObjectType())
    {
    case AttributeDefinition::EObjectType::Network:
    {
        std::visit(Visitor(os), iattr.Value());
        break;
    }
    case AttributeDefinition::EObjectType::Node:
    {
        auto find_node_name =
            [&]()
            {
                auto beg_node = net.Nodes().begin();
                auto end_node = net.Nodes().end();
                auto iter = std::find_if(beg_node, end_node,
                    [&](const Node& n)
                    {
                        auto beg_attr = n.AttributeValues().begin();
                        auto end_attr = n.AttributeValues().end();
                        auto iter = std::find_if(beg_attr, end_attr, [&](const Attribute& other) { return &other == &iattr; });
                        return iter != end_attr;
                    });
                return iter != end_node ? iter->Name() : "";
            };
        os << " BU_ " << find_node_name();
        std::visit(Visitor(os), iattr.Value());
        break;
    }
    case AttributeDefinition::EObjectType::Message:
    {
        auto find_message_id =
            [&]()
            {
                auto beg_msg = net.Messages().begin();
                auto end_msg = net.Messages().end();
                auto iter = std::find_if(beg_msg, end_msg,
                    [&](const Message& msg)
                    {
                        auto beg_attr = msg.AttributeValues().begin();
                        auto end_attr = msg.AttributeValues().end();
                        auto iter = std::find_if(beg_attr, end_attr, [&](const Attribute& other) { return &other == &iattr; });
                        return iter != end_attr;
                    });
                return iter != end_msg ? iter->Id() : uint64_t(-1);
            };
        os << " BO_ " << find_message_id();
        std::visit(Visitor(os), iattr.Value());
        break;
    }
    case AttributeDefinition::EObjectType::Signal:
    {
        auto find_signal =
            [&]() -> const Signal*
            {
                const Signal* osig = nullptr;
                auto beg_msg = net.Messages().begin();
                auto end_msg = net.Messages().end();
                auto iter = std::find_if(beg_msg, end_msg,
                    [&](const Message& msg)
                    {
                        auto beg_sig = msg.Signals().begin();
                        auto end_sig = msg.Signals().end();
                        auto iter = std::find_if(beg_sig, end_sig,
                            [&](const Signal& sig)
                            {
                                auto beg_attr = sig.AttributeValues().begin();
                                auto end_attr = sig.AttributeValues().end();
                                auto iter = std::find_if(beg_attr, end_attr, [&](const Attribute& other) { return &other == &iattr; });
                                if (iter != end_attr)
                                {
                                    osig = &sig;
                                }
                                return iter != end_attr;
                            });
                        return iter != end_sig;
                    });
                return osig;
            };
        const Signal* sig = find_signal();
        os << " SG_ " << net.ParentMessage(sig)->Id();
        os << " " << sig->Name();
        std::visit(Visitor(os), iattr.Value());
        break;
    }
    case AttributeDefinition::EObjectType::EnvironmentVariable:
    {
        auto find_environment_variable_name =
            [&]()
            {
                auto beg_env_var = net.EnvironmentVariables().begin();
                auto end_env_var = net.EnvironmentVariables().end();
                auto iter = std::find_if(beg_env_var, end_env_var,
                    [&](const EnvironmentVariable& ev)
                    {
                        auto beg_attr = ev.AttributeValues().begin();
                        auto end_attr = ev.AttributeValues().end();
                        auto iter = std::find_if(beg_attr, end_attr, [&](const Attribute& other) { return &other == &iattr; });
                        return iter != end_attr;
                    });
                return iter != end_env_var ? iter->Name() : "";

            };
        os << " EV_ " << find_environment_variable_name();
        std::visit(Visitor(os), iattr.Value());
        break;
    }
    }
    os << ";\n";
    return os;
}
std::ostream& dbcppp::Network2DBC::operator<<(std::ostream& os, const AttributeDefinition& ad)
{
    struct VisitorValueType
    {
        VisitorValueType(std::ostream& os)
            : _os(os)
        {}
        void operator()(const AttributeDefinition::ValueTypeInt& vt) const
        {
            _os << " INT" << " " << vt.minimum << " " << vt.maximum;
        }
        void operator()(const AttributeDefinition::ValueTypeHex& vt) const
        {
            _os << " HEX" << " " << vt.minimum << " " << vt.maximum;
        }
        void operator()(const AttributeDefinition::ValueTypeFloat& vt) const
        {
            _os << " FLOAT" << " " << vt.minimum << " " << vt.maximum;
        }
        void operator()(const AttributeDefinition::ValueTypeString& vt) const
        {
            _os << " STRING";
        }
        void operator()(const AttributeDefinition::ValueTypeEnum& vt) const
        {
            _os << " ENUM";
            const auto& values = vt.values;
            if (values.size())
            {
                auto iter = values.begin();
                _os << " \"" << *iter << "\"";
                for (iter++; iter != values.end(); iter++)
                {
                    _os << ", \"" << *iter << "\"";
                }
            }
        }

    private:
        std::ostream& _os;
    };

    std::string object_type = "";
    switch (ad.ObjectType())
    {
    case AttributeDefinition::EObjectType::Network: break;
    case AttributeDefinition::EObjectType::Node: object_type = "BU_"; break;
    case AttributeDefinition::EObjectType::Message: object_type = "BO_"; break;
    case AttributeDefinition::EObjectType::Signal: object_type = "SG_"; break;
    case AttributeDefinition::EObjectType::EnvironmentVariable: object_type = "EV_"; break;
    }
    os << "BA_DEF_ ";
    if (object_type != "")
    {
        os << object_type << " ";
    }
    os << "\"" << ad.Name() << "\"";
    std::visit(VisitorValueType(os), ad.ValueType());
    os << ";\n";
    return os;
}
std::ostream& dbcppp::Network2DBC::operator<<(std::ostream& os, const BitTiming& bt)
{
    os << "BS_:";
    if (bt.Baudrate() != 0 && bt.BTR1() != 0 && bt.BTR2() != 0)
    {
        os << " " << bt.Baudrate() << " : " << bt.BTR1() << ", " << bt.BTR2();
    }
    os << "\n";
    return os;
}
std::ostream& dbcppp::Network2DBC::operator<<(std::ostream& os, const EnvironmentVariable& ev)
{
    os << "EV_ " << ev.Name() << ": ";
    switch (ev.VarType())
    {
    case EnvironmentVariable::EVarType::Integer: os << "0"; break;
    case EnvironmentVariable::EVarType::Float: os << "1"; break;
    case EnvironmentVariable::EVarType::String: os << "2"; break;
    case EnvironmentVariable::EVarType::Data: os << "0"; break;
    }
    os << " [" << ev.Minimum() << "|" << ev.Maximum() << "]" << " \"" << ev.Unit() << "\" "
        << ev.InitialValue() << " " << ev.EvId() << " ";
    switch (ev.AccessType())
    {
    case EnvironmentVariable::EAccessType::Unrestricted: os << "DUMMY_NODE_VECTOR0"; break;
    case EnvironmentVariable::EAccessType::Read: os << "DUMMY_NODE_VECTOR1"; break;
    case EnvironmentVariable::EAccessType::Write: os << "DUMMY_NODE_VECTOR2"; break;
    case EnvironmentVariable::EAccessType::ReadWrite: os << "DUMMY_NODE_VECTOR3"; break;
    case EnvironmentVariable::EAccessType::Unrestricted_: os << "DUMMY_NODE_VECTOR8000"; break;
    case EnvironmentVariable::EAccessType::Read_: os << "DUMMY_NODE_VECTOR8001"; break;
    case EnvironmentVariable::EAccessType::Write_: os << "DUMMY_NODE_VECTOR8002"; break;
    case EnvironmentVariable::EAccessType::ReadWrite_: os << "DUMMY_NODE_VECTOR8003"; break;
    }
    bool first = true;
    for (const auto& n : ev.AccessNodes())
    {
        if (first)
        {
            os << " " << n;
            first = false;
        }
        else
        {
            os << ", " << n;
        }
    }
    os << ";\n";
    return os;
}
std::ostream& dbcppp::Network2DBC::operator<<(std::ostream& os, const Message& m)
{
    os << "BO_ " << m.Id() << " " << m.Name() << ": " << m.MessageSize() << " " << m.Transmitter() << "\n";
    for (const auto& s : m.Signals())
    {
        os << s;
    };
    return os;
}
std::ostream& dbcppp::Network2DBC::operator<<(std::ostream& os, const Network& net)
{
    os << "VERSION \"";
    if (net.Version() != "")
    {
        os << net.Version();
    }
    os << "\"\n";
    os << "NS_:\n";
    for (const auto& ns : net.NewSymbols())
    {
        os << "\t" << ns << "\n";
    };
    os << net.GetBitTiming();
    os << "BU_:";
    for (const auto& n : net.Nodes())
    {
            os << " " << n.Name();
    }
    os << "\n";
    for (const auto& vt : net.ValueTables())
    {
            os << vt;
    }
    for (const auto& m : net.Messages())
    {
        os << m;
    }
    for (const auto& m : net.Messages())
    {
        if (m.MessageTransmitters_Size())
        {
            const auto& n = m.MessageTransmitters_Get(0);
            os << "BO_TX_BU_ " << m.Id() << " :";
            os << " " << n;
            for (std::size_t i = 1; i < m.MessageTransmitters_Size(); i++)
            {
                const auto& n = m.MessageTransmitters_Get(i);
                os << ", " << n;
            }
            os << ";\n";
        }
    }
    for (const auto& ev : net.EnvironmentVariables())
    {
        os << ev;
    }
    for (const auto& ev : net.EnvironmentVariables())
    {
        if (ev.VarType() == EnvironmentVariable::EVarType::Data)
        {
            os << "ENVVAR_DATA_ " << ev.Name() << " : " << ev.DataSize() << ";\n";
        }
    }
    for (const auto& vt : net.ValueTables())
    {
        if (vt.GetSignalType())
        {
            os << vt;
        }
    }
    if (net.Comment() != "")
    {
        os << "CM_ \"" << net.Comment() << "\";\n";
    }
    for (const auto& n : net.Nodes())
    {
        if (n.Comment() != "")
        {
            os << "CM_ BU_ " << n.Name() << " \"" << n.Comment() << "\"" << ";\n";
        }
    }
    for (const auto& m : net.Messages())
    {
        if (m.Comment() != "")
        {
            os << "CM_ BO_ " << m.Id() << " \"" << m.Comment() << "\"" << ";\n";
        }
    }
    for (const auto& m : net.Messages())
    {
        for (const auto& s : m.Signals())
        {
            if (s.Comment() != "")
            {
                os << "CM_ SG_ " << m.Id() << " " << s.Name() << " \"" << s.Comment() << "\"" << ";\n";
            }
        }
    }
    for (const auto& ev : net.EnvironmentVariables())
    {
        if (ev.Comment() != "")
        {
            os << "CM_ EV_ " << ev.Name() << " \"" << ev.Comment() << "\"" << ";\n";
        }
    }
    for (const auto& ad : net.AttributeDefinitions())
    {
        os << "\n";
        os << ad;
    }
    for (const auto& ad : net.AttributeDefaults())
    {
        os << "\n";
        os << na_t{net, ad};
    }
    for (const auto& av : net.AttributeValues())
    {
        os << "\n";
        os << na_t{net, av};
    }
    for (const auto& n : net.Nodes())
    {
        for (const auto& av : n.AttributeValues())
        {
            os << "\n";
            os << na_t{net, av};
        }
    }
    for (const auto& m : net.Messages())
    {
        for (const auto& av : m.AttributeValues())
        {
            os << "\n";
            os << na_t{net, av};
        }
    }
    for (const auto& m : net.Messages())
    {
        for (const auto& s : m.Signals())
        {
            for (const auto& av : s.AttributeValues())
            {
                os << "\n";
                os << na_t{net, av};
            }
        }
    }
    for (const auto& ev : net.EnvironmentVariables())
    {
        for (const auto& av : ev.AttributeValues())
        {
            os << "\n";
            os << na_t{net, av};
        }
    }
    for (const auto& m : net.Messages())
    {
        for (const auto& s : m.Signals())
        {
            if (s.ValueEncodingDescriptions_Size())
            {
                os << "VAL_ " << m.Id() << " " << s.Name();
                for (const auto& ved : s.ValueEncodingDescriptions())
                {
                    os << " " << ved.Value() << " \"" << ved.Description() << "\"";
                }
                os << ";\n";
            }
        }
    }
    for (const auto& ev : net.EnvironmentVariables())
    {
        if (ev.ValueEncodingDescriptions_Size())
        {
            os << "VAL_ " << ev.Name();
            for (const auto& ved : ev.ValueEncodingDescriptions())
            {
                os << " " << ved.Value() << " \"" << ved.Description() << "\"";
            }
            os << ";\n";
        }
    }
    for (const auto& m : net.Messages())
    {
        for (const auto& sg : m.SignalGroups())
        {
            os << "SIG_GROUP_ " << sg.MessageId() << " " << sg.Name() << " " << sg.Repetitions() << " :";
            for (const auto& name : sg.SignalNames())
            {
                os << " " << name;
            }
            os << ";\n";
        }
    }
    for (const auto& m : net.Messages())
    {
        for (const auto& s : m.Signals())
        {
            if (s.ExtendedValueType() != Signal::EExtendedValueType::Integer)
            {
                uint64_t type = 0;
                switch (s.ExtendedValueType())
                {
                case Signal::EExtendedValueType::Float: type = 1; break;
                case Signal::EExtendedValueType::Double: type = 2; break;
                }
                os << "SIG_VALTYPE_ " << m.Id() << " " << s.Name() << " : " << type << ";\n";
            }
        }
    }
    for (const auto& m : net.Messages())
    {
        for (const auto& s : m.Signals())
        {
            for (const auto& smv : s.SignalMultiplexerValues())
            {
                os << "SG_MUL_VAL_ " << m.Id() << " " << s.Name() << " " << smv.SwitchName();
                std::string ssr;
                for (const auto& range : smv.ValueRanges())
                {
                    ssr += " " + std::to_string(range.from) + "-" + std::to_string(range.to) + ",";
                }
                if (!ssr.empty())
                {
                    ssr.erase(ssr.end() - 1);
                    os << ssr;
                }
                os <<  ";\n";
            }
        }
    }
    return os;
}
std::ostream& dbcppp::Network2DBC::operator<<(std::ostream& os, const Node& n)
{
    os << n.Name();
    return os;
}
std::ostream& dbcppp::Network2DBC::operator<<(std::ostream& os, const Signal& s)
{
    os << "\tSG_ " << s.Name() << " ";
    switch (s.MultiplexerIndicator())
    {
    case Signal::EMultiplexer::MuxSwitch: os << "M "; break;
    case Signal::EMultiplexer::MuxValue: os << "m" << s.MultiplexerSwitchValue() << " "; break;
    }
    os << ": " << s.StartBit() << "|" << s.BitSize() << "@";
    switch (s.ByteOrder())
    {
    case Signal::EByteOrder::BigEndian: os << "0"; break;
    case Signal::EByteOrder::LittleEndian: os << "1"; break;
    }
    switch (s.ValueType())
    {
    case Signal::EValueType::Unsigned: os << "+ "; break;
    case Signal::EValueType::Signed: os << "- "; break;
    }
    os << "(" << s.Factor() << "," << s.Offset() << ") ";
    os << "[" << s.Minimum() << "|" << s.Maximum() << "] ";
    os << "\"" << s.Unit() << "\"";
    std::pmr::string receivers;
    for (const auto& n : s.Receivers())
    {
        receivers += n + ", ";
    }
    if (!receivers.empty())
    {
        receivers.erase(receivers.end() - 1);
        receivers.erase(receivers.end() - 1);
        os << " ";
    }
    os << receivers;
    os << "\n";
    return os;
}
std::ostream& dbcppp::Network2DBC::operator<<(std::ostream& os, const SignalType& st)
{
    os << "SGTYPE_ " << st.Name() << " : " << st.SignalSize() << "@";
    switch (st.ByteOrder())
    {
    case Signal::EByteOrder::BigEndian: os << "0"; break;
    case Signal::EByteOrder::LittleEndian: os << "1"; break;
    }
    switch (st.ValueType())
    {
    case Signal::EValueType::Unsigned: os << "+ "; break;
    case Signal::EValueType::Signed: os << "- "; break;
    }
    os << "(" << st.Factor() << "," << st.Offset() << ") ";
    os << "[" << st.Minimum() << "|" << st.Maximum() << "] ";
    os << "\"" << st.Unit() << "\" " << st.DefaultValue();
    os << ", " << st.ValueTable();
    os << ";";
    return os;
}
std::ostream& dbcppp::Network2DBC::operator<<(std::ostream& os, const ValueTable& vt)
{
    if (vt.ValueEncodingDescriptions_Size())
    {
        os << "VAL_TABLE_ " << vt.Name();
        for (const auto& ved : vt.ValueEncodingDescriptions())
        {
            os << " " << ved.Value() << " \"" << ved.Description() << "\"";
        }
        os << ";\n";
    }
    return os;
}
