#pragma once

#include <zephyr/kernel.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <streambuf>

#include <dbcppp/Network.h>

namespace eerie_leap::subsys::dbc {

struct DbcMessage {
   const dbcppp::IMessage* message;
   std::unordered_map<size_t, const dbcppp::ISignal*> signals;
};

class Dbc {
private:
   std::unique_ptr<dbcppp::INetwork> net_;
   std::unordered_map<uint64_t, DbcMessage> messages_;
   bool is_loaded_;

   DbcMessage* GetOrRegisterMessage(uint64_t frame_id);

public:
   Dbc();
   virtual ~Dbc() = default;

   using SignalReader = std::function<float (size_t)>;

   bool IsLoaded() const { return is_loaded_; }
   bool LoadDbcFile(std::streambuf& dbc_content);
   bool TryRegisterSignal(uint64_t frame_id, const std::string& signal_name);
   void RegisterAllSignalsForFrame(uint64_t frame_id);
   double GetSignalValue(uint64_t frame_id, size_t signal_name_hash, const void* bytes);
   std::vector<uint8_t> EncodeMessage(uint64_t frame_id, const SignalReader& signal_reader);
};

} // namespace eerie_leap::subsys::dbc
