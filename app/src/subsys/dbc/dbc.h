#pragma once

#include <zephyr/kernel.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <streambuf>

#include <dbcppp/Network.h>

#include "dbc_message.h"

namespace eerie_leap::subsys::dbc {

class Dbc {
private:
   std::unique_ptr<dbcppp::INetwork> net_;
   std::unordered_map<uint64_t, DbcMessage> messages_;
   bool is_loaded_;

public:
   Dbc();
   virtual ~Dbc() = default;

   bool IsLoaded() const { return is_loaded_; }
   bool LoadDbcFile(std::streambuf& dbc_content);

   DbcMessage* AddMessage(uint32_t id, std::string name, uint8_t message_size);
   DbcMessage* GetOrRegisterMessage(uint32_t frame_id);
};

} // namespace eerie_leap::subsys::dbc
