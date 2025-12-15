#include "utilities/string/string_helpers.h"

#include "dbc_message.h"

namespace eerie_leap::subsys::dbc {

using namespace eerie_leap::utilities::string;

DbcMessage::DbcMessage(
    std::allocator_arg_t,
    allocator_type alloc,
    dbcppp::Message* message)
        : message_(message),
        frame_id_(message->Id()),
        signals_(alloc),
        allocator_(alloc) {

    for(const auto& signal : message_->Signals())
        RegisterSignal(&signal);
}

void DbcMessage::UpdateReference(dbcppp::Message* message) {
    message_ = message;
}

void DbcMessage::RegisterSignal(const dbcppp::Signal* signal) {
    size_t signal_name_hash = StringHelpers::GetHash(signal->Name());

    if(!signals_.emplace(signal_name_hash, signal).second)
        throw std::runtime_error("Failed to register signal. Signal with name '" + std::string(signal->Name()) + "' already exists.");

    UpdateSignalReferences();
}

dbcppp::Signal* DbcMessage::GetDbcSignal(size_t signal_name_hash) {
    dbcppp::Signal* signal = nullptr;
    for(auto& sig : message_->GetSignals()) {
        if(StringHelpers::GetHash(sig.Name()) == signal_name_hash) {
            signal = &sig;
            break;
        }
    }

    if(signal != nullptr)
        RegisterSignal(signal);

    return signal;
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

void DbcMessage::AddSignal(std::pmr::string name, uint32_t start_bit, uint32_t size_bits, float factor, float offset, std::pmr::string unit) {
    message_->AddSignal(dbcppp::Signal(
        std::allocator_arg,
        allocator_,
        message_->MessageSize(),
        std::move(name),
        dbcppp::Signal::EMultiplexer::NoMux,
        0,
        start_bit,
        size_bits,
        dbcppp::Signal::EByteOrder::LittleEndian,
        dbcppp::Signal::EValueType::Signed,
        factor,
        offset,
        0.0,
        0.0,
        std::move(unit),
        std::pmr::vector<std::pmr::string>{},
        std::pmr::vector<dbcppp::Attribute>{},
        std::pmr::vector<dbcppp::ValueEncodingDescription>{},
        "",
        dbcppp::Signal::EExtendedValueType::Integer,
        std::pmr::vector<dbcppp::SignalMultiplexerValue>{}));

    RegisterSignal(&message_->GetSignals().back());
}

bool DbcMessage::HasSignal(size_t signal_name_hash) {
    if(signals_.contains(signal_name_hash))
        return true;

    return GetDbcSignal(signal_name_hash) != nullptr;
}

bool DbcMessage::HasSignal(std::string_view signal_name) {
    return HasSignal(StringHelpers::GetHash(signal_name));
}

double DbcMessage::GetSignalValue(size_t signal_name_hash, const void* bytes) {
   if(!HasSignal(signal_name_hash))
      throw std::runtime_error("DBC Signal name not found.");

   const dbcppp::Signal* signal = signals_.at(signal_name_hash);
   auto decoded_value = signal->Decode(bytes);

   return signal->RawToPhys(decoded_value);
}

std::vector<uint8_t> DbcMessage::EncodeMessage(const SignalReader& signal_reader) {
   std::vector<uint8_t> bytes(MessageSize(), 0);

   for(const auto& [signal_name_hash, signal] : signals_) {
      auto value = signal_reader(signal_name_hash);
      auto value_raw = signal->PhysToRaw(value);
      signal->Encode(value_raw, bytes.data());
   }

   return bytes;
}

void DbcMessage::UpdateSignalReferences() {
    for(auto& signal : message_->GetSignals()) {
        size_t signal_name_hash = StringHelpers::GetHash(signal.Name());
        if(signals_.contains(signal_name_hash))
            signals_.at(signal_name_hash) = &signal;
    }
}

} // namespace eerie_leap::subsys::dbc
