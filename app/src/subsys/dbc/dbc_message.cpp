#include "subsys/dbc/dbcppp/src/SignalImpl.h"
#include "subsys/dbc/dbcppp/src/MessageImpl.h"

#include "utilities/string/string_helpers.h"

#include "dbc_message.h"

namespace eerie_leap::subsys::dbc {

using namespace eerie_leap::utilities::string;

void DbcMessage::RegisterSignal(const dbcppp::ISignal* signal) {
    size_t signal_name_hash = StringHelpers::GetHash(signal->Name());

    if(!signals_.emplace(signal_name_hash, signal).second)
        throw std::runtime_error("Failed to register signal. Signal with name '" + std::string(signal->Name()) + "' already exists.");
}

DbcMessage::DbcMessage(const dbcppp::IMessage* message) : message_(message) {
    for(const dbcppp::ISignal& signal : message_->Signals())
        RegisterSignal(&signal);
}

DbcMessage::DbcMessage(uint32_t id, std::string name, uint32_t message_size) {
    message_container_ = dbcppp::IMessage::Create(
        id,
        std::move(name),
        message_size,
        "",
        {},
        {},
        {},
        "",
        {});

    message_ = message_container_.get();
}

uint32_t DbcMessage::Id() const {
    return message_->Id();
}

std::string_view DbcMessage::Name() const {
    return message_->Name();
}

uint32_t DbcMessage::MessageSize() const {
    return message_->MessageSize();
}

void DbcMessage::AddSignal(std::string name, uint32_t start_bit, uint32_t size_bits, float factor, float offset, std::string unit) {
    auto signal = dbcppp::ISignal::Create(
        message_->MessageSize(),
        std::move(name),
        dbcppp::ISignal::EMultiplexer::NoMux,
        0,
        start_bit,
        size_bits,
        dbcppp::ISignal::EByteOrder::LittleEndian,
        dbcppp::ISignal::EValueType::Signed,
        static_cast<double>(factor),
        static_cast<double>(offset),
        0.0,
        0.0,
        std::move(unit),
        {},
        {},
        {},
        "",
        dbcppp::ISignal::EExtendedValueType::Integer,
        {});

    RegisterSignal(signal.get());
    signals_container_.push_back(std::move(signal));
}

bool DbcMessage::HasSignal(size_t signal_name_hash) const {
    return signals_.contains(signal_name_hash);
}

bool DbcMessage::HasSignal(const std::string_view signal_name) const {
    return HasSignal(StringHelpers::GetHash(signal_name));
}

double DbcMessage::GetSignalValue(size_t signal_name_hash, const void* bytes) const {
   if(!HasSignal(signal_name_hash))
      throw std::runtime_error("DBC Signal name not found.");

   const dbcppp::ISignal* signal = signals_.at(signal_name_hash);
   auto decoded_value = signal->Decode(bytes);

   return signal->RawToPhys(decoded_value);
}

std::vector<uint8_t> DbcMessage::EncodeMessage(const SignalReader& signal_reader) {
   std::vector<uint8_t> bytes(MessageSize(), 0);

   for(const auto& [signal_name_hash, signal] : signals_) {
      auto value = static_cast<double>(signal_reader(signal_name_hash));
      auto value_raw = signal->PhysToRaw(value);
      signal->Encode(value_raw, bytes.data());
   }

   return bytes;
}

} // namespace eerie_leap::subsys::dbc
