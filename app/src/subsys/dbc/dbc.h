#pragma once

#include <zephyr/kernel.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <streambuf>

#include <dbcppp/Network.h>

#include "dbc_message.h"
#include "dbcppp/Message.h"

namespace eerie_leap::subsys::dbc {

class Dbc {
private:
   std::unique_ptr<dbcppp::INetwork> net_;
   std::unordered_map<uint32_t, DbcMessage> messages_;
   bool is_loaded_;

   const dbcppp::IMessage* GetDbcMessage(uint32_t frame_id) const;

public:
   Dbc();
   virtual ~Dbc() = default;

   bool IsLoaded() const { return is_loaded_; }
   bool LoadDbcFile(std::streambuf& dbc_content);

   DbcMessage* AddMessage(uint32_t id, std::string name, uint8_t message_size);
   DbcMessage* GetOrRegisterMessage(uint32_t frame_id);
   bool HasMessage(uint32_t frame_id) const;
};

} // namespace eerie_leap::subsys::dbc
