#include <stdexcept>

#include "dbc.h"

namespace eerie_leap::subsys::dbc {

Dbc::Dbc() : is_loaded_(false) {}

bool Dbc::LoadDbcFile(std::streambuf& dbc_content) {
   messages_.clear();

   std::istream stream(&dbc_content);
   net_ = dbcppp::INetwork::LoadDBCFromIs(stream);

   is_loaded_ = net_ != nullptr;

   return is_loaded_;
}

double Dbc::GetSignalValue(uint64_t frame_id, const std::string& signal_name, const void* bytes) {
   if(!is_loaded_)
      throw std::runtime_error("DBC not loaded.");

   DbcMessage* dbc_message = nullptr;
   if(messages_.contains(frame_id)) {
      dbc_message = &messages_.at(frame_id);
   } else {
      const dbcppp::IMessage* message = nullptr;
      for(const dbcppp::IMessage& msg : net_->Messages()) {
         if(msg.Id() == frame_id) {
            message = &msg;
            break;
         }
      }

      if(message == nullptr)
         throw std::runtime_error("DBC Frame ID not found.");

      messages_[frame_id] = {
         .message = message
      };

      dbc_message = &messages_.at(frame_id);
   }

   const dbcppp::ISignal* signal = nullptr;
   if(dbc_message->signals.contains(signal_name)) {
      signal = dbc_message->signals.at(signal_name);
   } else {
      for(const dbcppp::ISignal& sig : dbc_message->message->Signals()) {
         if(sig.Name() == signal_name) {
            signal = &sig;
            break;
         }
      }

      if(signal == nullptr)
         throw std::runtime_error("DBC Signal name not found.");

      dbc_message->signals.emplace(signal_name, signal);
   }

   auto decoded_value = signal->Decode(bytes);

   return signal->RawToPhys(decoded_value);
}

} // namespace eerie_leap::subsys::dbc
