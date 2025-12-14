#include <stdexcept>

#include "dbc.h"
#include "dbcppp/Message.h"

namespace eerie_leap::subsys::dbc {

Dbc::Dbc(std::allocator_arg_t, allocator_type alloc)
   : messages_(alloc),
   is_loaded_(false),
   allocator_(alloc) {}

dbcppp::Network* Dbc::GetOrCreateDbcNetwork() {
   if(net_ != nullptr)
      return net_.get();

   net_ = make_shared_pmr<dbcppp::Network>(allocator_);

   return net_.get();
}

bool Dbc::LoadDbcFile(std::streambuf& dbc_content) {
   messages_.clear();

   std::istream stream(&dbc_content);
   net_ = dbcppp::Network::LoadDBCFromIs(allocator_.resource(), stream);

   is_loaded_ = net_ != nullptr;

   return is_loaded_;
}

DbcMessage* Dbc::AddMessage(uint32_t frame_id, std::pmr::string name, uint8_t message_size) {
   if(messages_.contains(frame_id))
      throw std::runtime_error("Duplicate Frame ID.");

   auto net = GetOrCreateDbcNetwork();

   net->AddMessage(dbcppp::Message(
      std::allocator_arg,
      allocator_,
      frame_id,
      std::move(name),
      message_size,
      "",
      std::pmr::vector<std::pmr::string>{},
      std::pmr::vector<dbcppp::Signal>{},
      std::pmr::vector<dbcppp::Attribute>{},
      "",
      std::pmr::vector<dbcppp::SignalGroup>{}));

   return GetMessage(frame_id);
}

bool Dbc::HasMessage(uint32_t frame_id) {
   if(messages_.contains(frame_id))
      return true;

   return GetDbcMessage(frame_id) != nullptr;
}

DbcMessage* Dbc::GetMessage(uint32_t frame_id) {
   if(messages_.contains(frame_id))
      return &messages_.at(frame_id);

   dbcppp::Message* message = GetDbcMessage(frame_id);
   if(message == nullptr)
      throw std::runtime_error("Invalid Frame ID.");

   messages_.emplace(frame_id, message);
   UpdateMessageReferences();

   return &messages_.at(frame_id);
}

dbcppp::Message* Dbc::GetDbcMessage(uint32_t frame_id) {
   auto* net = GetOrCreateDbcNetwork();

   dbcppp::Message* message = nullptr;
   for(dbcppp::Message& msg : net->GetMessages()) {
      if(msg.Id() == frame_id) {
         message = &msg;
         break;
      }
   }

   return message;
}

void Dbc::UpdateMessageReferences() {
   auto* net = GetOrCreateDbcNetwork();

   for(auto& [frame_id, message] : messages_)
      message.UpdateReference(GetDbcMessage(frame_id));
}

} // namespace eerie_leap::subsys::dbc
