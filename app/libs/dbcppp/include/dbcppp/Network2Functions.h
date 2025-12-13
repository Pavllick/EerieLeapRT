
#pragma once

#include <boost/format.hpp>
#include <ostream>
#include <tuple>
#include <ostream>

#include "Network.h"

namespace dbcppp
{
    namespace Network2DBC
    {
        using na_t = std::tuple<const Network&, const Attribute&>;
        std::ostream& operator<<(std::ostream& os, const na_t& na);
        std::ostream& operator<<(std::ostream& os, const AttributeDefinition& ad);
        std::ostream& operator<<(std::ostream& os, const BitTiming& bt);
        std::ostream& operator<<(std::ostream& os, const EnvironmentVariable& ev);
        std::ostream& operator<<(std::ostream& os, const Message& m);
        std::ostream& operator<<(std::ostream& os, const Network& net);
        std::ostream& operator<<(std::ostream& os, const Node& n);
        std::ostream& operator<<(std::ostream& os, const Signal& s);
        std::ostream& operator<<(std::ostream& os, const SignalType& st);
        std::ostream& operator<<(std::ostream& os, const ValueTable& vt);
    }
    namespace Network2Human
    {
        std::ostream& operator<<(std::ostream& os, const Network& net);
        std::ostream& operator<<(std::ostream& os, const Message& msg);
    }
}
