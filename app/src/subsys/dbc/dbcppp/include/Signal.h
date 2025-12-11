#pragma once

#include <set>
#include <map>
#include <memory>
#include <string>
#include <cstddef>

#include "subsys/dbc/dbcppp/include/Iterator.h"
#include "subsys/dbc/dbcppp/include/Node.h"
#include "subsys/dbc/dbcppp/include/Attribute.h"
#include "subsys/dbc/dbcppp/include/SignalMultiplexerValue.h"
#include "subsys/dbc/dbcppp/include/ValueEncodingDescription.h"

#include "utilities/memory/memory_resource_manager.h"

using namespace eerie_leap::utilities::memory;

namespace dbcppp
{
    class ISignal
    {
    public:
        enum class EErrorCode : uint8_t
        {
            NoError,
            MaschinesFloatEncodingNotSupported = 1,
            MaschinesDoubleEncodingNotSupported = 2,
            SignalExceedsMessageSize = 4,
            WrongBitSizeForExtendedDataType = 8
        };
        enum class EMultiplexer : uint8_t
        {
            NoMux, MuxSwitch, MuxValue
        };
        enum class EByteOrder : uint8_t
        {
            BigEndian = 0,
            LittleEndian = 1
        };
        enum class EValueType : uint8_t
        {
            Signed, Unsigned
        };
        enum class EExtendedValueType : uint8_t
        {
            Integer, Float, Double
        };

        static pmr_unique_ptr<ISignal> Create(
              std::pmr::memory_resource* mr
            , uint16_t message_size
            , std::pmr::string name
            , EMultiplexer multiplexer_indicator
            , uint64_t multiplexer_switch_value
            , uint16_t start_bit
            , uint16_t bit_size
            , EByteOrder byte_order
            , EValueType value_type
            , float factor
            , float offset
            , float minimum
            , float maximum
            , std::pmr::string unit
            , std::pmr::vector<std::pmr::string>&& receivers
            , std::pmr::vector<std::unique_ptr<IAttribute>>&& attribute_values
            , std::pmr::vector<std::unique_ptr<IValueEncodingDescription>>&& value_encoding_description
            , std::pmr::string comment
            , EExtendedValueType extended_value_type
            , std::pmr::vector<std::unique_ptr<ISignalMultiplexerValue>>&& signal_multiplexer_values);

        virtual ~ISignal() = default;
        virtual const std::string_view Name() const = 0;
        virtual EMultiplexer MultiplexerIndicator() const = 0;
        virtual uint64_t MultiplexerSwitchValue() const = 0;
        virtual uint16_t StartBit() const = 0;
        virtual uint16_t BitSize() const = 0;
        virtual EByteOrder ByteOrder() const = 0;
        virtual EValueType ValueType() const = 0;
        virtual float Factor() const = 0;
        virtual float Offset() const = 0;
        virtual float Minimum() const = 0;
        virtual float Maximum() const = 0;
        virtual const std::string_view Unit() const = 0;
        virtual const std::pmr::string& Receivers_Get(std::size_t i) const = 0;
        virtual uint64_t Receivers_Size() const = 0;
        virtual const IValueEncodingDescription& ValueEncodingDescriptions_Get(std::size_t i) const = 0;
        virtual uint64_t ValueEncodingDescriptions_Size() const = 0;
        virtual const IAttribute& AttributeValues_Get(std::size_t i) const = 0;
        virtual uint64_t AttributeValues_Size() const = 0;
        virtual const std::string_view Comment() const = 0;
        virtual EExtendedValueType ExtendedValueType() const = 0;
        virtual const ISignalMultiplexerValue& SignalMultiplexerValues_Get(std::size_t i) const = 0;
        virtual uint64_t SignalMultiplexerValues_Size() const = 0;

        virtual bool Error(EErrorCode code) const = 0;

        virtual bool operator==(const ISignal& rhs) const = 0;
        virtual bool operator!=(const ISignal& rhs) const = 0;

        /// \brief Extracts the raw value from a given n byte array
        ///
        /// This function uses a optimized method of reversing the byte order and extracting
        /// the value from the given data.
        /// !!! Note: This function takes at least 8 bytes and at least as many as the signal needs to be represented,
        ///     if you pass less, the program ends up in undefined behaviour! !!!
        ///
        /// @param nbyte a n byte array (!!! at least 8 bytes !!!) which is representing the can data.
        ///               the data must be in this order:
        ///               bit_0  - bit_7:  bytes[0]
        ///               bit_8  - bit_15: bytes[1]
        ///               ...
        ///               bit_n-7 - bit_n: bytes[n / 8]
        ///               (like the Unix CAN frame does store the data)
        using raw_t = uint64_t;
        inline raw_t Decode(const void* bytes) const noexcept { return _decode(this, bytes); }
        inline void Encode(raw_t raw, void* buffer) const noexcept { return _encode(this, raw, buffer); }

        inline double RawToPhys(raw_t raw) const noexcept { return _raw_to_phys(this, raw); }
        inline raw_t PhysToRaw(double phys) const noexcept { return _phys_to_raw(this, phys); }

        DBCPPP_MAKE_ITERABLE(ISignal, Receivers, std::pmr::string);
        DBCPPP_MAKE_ITERABLE(ISignal, ValueEncodingDescriptions, IValueEncodingDescription);
        DBCPPP_MAKE_ITERABLE(ISignal, AttributeValues, IAttribute);
        DBCPPP_MAKE_ITERABLE(ISignal, SignalMultiplexerValues, ISignalMultiplexerValue);

    protected:
        // instead of using virtuals dynamic dispatching use function pointers
        raw_t (*_decode)(const ISignal* sig, const void* bytes) noexcept {nullptr};
        void (*_encode)(const ISignal* sig, raw_t raw, void* buffer) noexcept {nullptr};
        double (*_raw_to_phys)(const ISignal* sig, raw_t raw) noexcept {nullptr};
        raw_t (*_phys_to_raw)(const ISignal* sig, double phys) noexcept {nullptr};
    };
}
