#pragma once

#include <zephyr/kernel.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <streambuf>
#include <fstream>

#include <dbcppp/Network.h>

namespace eerie_leap::subsys::dbc {

class Dbc {
private:
   struct DbcMessage {
      const dbcppp::IMessage* message;
      std::unordered_map<std::string, const dbcppp::ISignal*> signals;
   };

   std::unique_ptr<dbcppp::INetwork> net_;
   std::unordered_map<uint64_t, DbcMessage> messages_;
   bool is_loaded_;

public:
   Dbc();
   virtual ~Dbc() = default;

   bool LoadDbcFile(std::streambuf& dbc_content);
   double GetSignalValue(uint64_t frame_id, const std::string& signal_name, const void* bytes);
};

} // namespace eerie_leap::subsys::dbc
