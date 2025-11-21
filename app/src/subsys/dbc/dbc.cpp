#include <stdexcept>

#include "utilities/string/string_helpers.h"

#include "dbc.h"

namespace eerie_leap::subsys::dbc {

using namespace eerie_leap::utilities::string;

Dbc::Dbc() : is_loaded_(false) {}

bool Dbc::LoadDbcFile(std::streambuf& dbc_content) {
   messages_.clear();

   std::istream stream(&dbc_content);
   net_ = dbcppp::INetwork::LoadDBCFromIs(stream);

   is_loaded_ = net_ != nullptr;

   return is_loaded_;
}

DbcMessage* Dbc::GetOrRegisterMessage(uint64_t frame_id) {
   if(!is_loaded_)
      throw std::runtime_error("DBC not loaded.");

   if(messages_.contains(frame_id))
      return &messages_.at(frame_id);

   const dbcppp::IMessage* message = nullptr;
   for(const dbcppp::IMessage& msg : net_->Messages()) {
      if(msg.Id() == frame_id) {
         message = &msg;
         break;
      }
   }

   if(message == nullptr)
      throw std::runtime_error("Invalid DBC Frame ID.");

   messages_.emplace(frame_id, DbcMessage{
      .message = message
   });

   return &messages_.at(frame_id);
}

bool Dbc::TryRegisterSignal(uint64_t frame_id, const std::string& signal_name) {
   DbcMessage* dbc_message = GetOrRegisterMessage(frame_id);

   const dbcppp::ISignal* signal = nullptr;
   size_t signal_name_hash = StringHelpers::GetHash(signal_name);

   if(dbc_message->signals.contains(signal_name_hash))
      return false;

   for(const dbcppp::ISignal& sig : dbc_message->message->Signals()) {
      if(sig.Name() == signal_name) {
         signal = &sig;
         break;
      }
   }

   if(signal == nullptr)
      return false;

   dbc_message->signals.emplace(signal_name_hash, signal);

   return true;
}

void Dbc::RegisterAllSignalsForFrame(uint64_t frame_id) {
   DbcMessage* dbc_message = GetOrRegisterMessage(frame_id);

   for(const dbcppp::ISignal& sig : dbc_message->message->Signals())
      TryRegisterSignal(frame_id, sig.Name());
}

double Dbc::GetSignalValue(uint64_t frame_id, size_t signal_name_hash, const void* bytes) {
   DbcMessage* dbc_message = GetOrRegisterMessage(frame_id);

   if(!dbc_message->signals.contains(signal_name_hash))
      throw std::runtime_error("DBC Signal name not found.");

   const dbcppp::ISignal* signal = dbc_message->signals.at(signal_name_hash);

   auto decoded_value = signal->Decode(bytes);

   return signal->RawToPhys(decoded_value);
}

std::vector<uint8_t> Dbc::EncodeMessage(uint64_t frame_id, const SignalReader& signal_reader) {
   DbcMessage* dbc_message = GetOrRegisterMessage(frame_id);

   std::vector<uint8_t> bytes(dbc_message->message->MessageSize(), 0);

   for(const auto& [signal_name_hash, signal] : dbc_message->signals) {
      auto value = static_cast<double>(signal_reader(signal_name_hash));
      auto value_raw = signal->PhysToRaw(value);
      signal->Encode(value_raw, bytes.data());
   }

   return bytes;
}

} // namespace eerie_leap::subsys::dbc
