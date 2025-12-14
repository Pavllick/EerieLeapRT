#pragma once

#include <memory_resource>
#include <memory>
#include <zephyr/kernel.h>
#include <string>
#include <unordered_map>
#include <streambuf>

#include <dbcppp/Network.h>
#include <dbcppp/Message.h>

#include "dbc_message.h"

namespace eerie_leap::subsys::dbc {

class Dbc {
public:
   using allocator_type = std::pmr::polymorphic_allocator<>;

private:
   std::shared_ptr<dbcppp::Network> net_;
   std::pmr::unordered_map<uint32_t, DbcMessage> messages_;
   bool is_loaded_;

   allocator_type allocator_;

   dbcppp::Network* GetOrCreateDbcNetwork();
   dbcppp::Message* GetDbcMessage(uint32_t frame_id);

   // HACK: Every time a new message is added to the DBC file, the message address
   // might change, due to container resizing. This function updates the message references
   // to the new addresses.
   void UpdateMessageReferences();

public:
   Dbc(std::allocator_arg_t, allocator_type alloc);

   Dbc(const Dbc&) = delete;
   Dbc& operator=(const Dbc&) noexcept = default;
   Dbc& operator=(Dbc&&) noexcept = default;
   Dbc(Dbc&&) noexcept = default;
   ~Dbc() = default;

   Dbc(Dbc&& other, allocator_type alloc)
      : net_(std::move(other.net_)),
      messages_(std::move(other.messages_)),
      is_loaded_(other.is_loaded_),
      allocator_(alloc) {}

   bool IsLoaded() const { return is_loaded_; }
   bool LoadDbcFile(std::streambuf& dbc_content);

   DbcMessage* AddMessage(uint32_t frame_id, std::pmr::string name, uint8_t message_size);
   DbcMessage* GetMessage(uint32_t frame_id);
   bool HasMessage(uint32_t frame_id);
};

} // namespace eerie_leap::subsys::dbc
