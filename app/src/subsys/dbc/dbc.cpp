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

DbcMessage* Dbc::AddMessage(uint32_t id, std::string name, uint8_t message_size) {
   if(messages_.contains(id))
      throw std::runtime_error("Duplicate Frame ID.");

   messages_.emplace(id, DbcMessage(id, std::move(name), message_size));

   return &messages_.at(id);
}

DbcMessage* Dbc::GetOrRegisterMessage(uint32_t frame_id) {
   if(messages_.contains(frame_id))
      return &messages_.at(frame_id);

   const dbcppp::IMessage* message = GetDbcMessage(frame_id);
   if(message == nullptr)
      throw std::runtime_error("Invalid Frame ID.");

   messages_.emplace(frame_id, DbcMessage(message));

   return &messages_.at(frame_id);
}

const dbcppp::IMessage* Dbc::GetDbcMessage(uint32_t frame_id) const {
   if(!is_loaded_)
      return nullptr;

   const dbcppp::IMessage* message = nullptr;
   for(const dbcppp::IMessage& msg : net_->Messages()) {
      if(msg.Id() == frame_id) {
         message = &msg;
         break;
      }
   }

   return message;
}

bool Dbc::HasMessage(uint32_t frame_id) const {
   if(messages_.contains(frame_id))
      return true;

   return GetDbcMessage(frame_id) != nullptr;
}

} // namespace eerie_leap::subsys::dbc
