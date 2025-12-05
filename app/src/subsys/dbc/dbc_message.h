#pragma once

#include <zephyr/kernel.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

#include <dbcppp/Network.h>

namespace eerie_leap::subsys::dbc {

class DbcMessage {
private:
   std::unique_ptr<dbcppp::IMessage> message_container_;
   const dbcppp::IMessage* message_;

   std::vector<std::unique_ptr<dbcppp::ISignal>> signals_container_;
   std::unordered_map<size_t, const dbcppp::ISignal*> signals_;

   bool RegisterSignal(const dbcppp::ISignal* signal);

public:
   using SignalReader = std::function<float (size_t)>;

   explicit DbcMessage(const dbcppp::IMessage* message);
   DbcMessage(uint32_t id, std::string name, uint32_t message_size);

   uint32_t Id() const;
   std::string_view Name() const;
   uint32_t MessageSize() const;

   void AddSignal(std::string name, uint32_t start_bit, uint32_t size_bits, float factor, float offset, std::string unit);
   bool HasSignal(size_t signal_name_hash) const;
   bool HasSignal(const std::string& signal_name) const;

   double GetSignalValue(size_t signal_name_hash, const void* bytes) const;
   std::vector<uint8_t> EncodeMessage(const SignalReader& signal_reader);
};

} // namespace eerie_leap::subsys::dbc
