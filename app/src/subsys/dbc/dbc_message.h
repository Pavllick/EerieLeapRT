#pragma once

#include <zephyr/kernel.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

#include <dbcppp/Network.h>

#include "utilities/memory/memory_resource_manager.h"

namespace eerie_leap::subsys::dbc {

using namespace eerie_leap::utilities::memory;

class DbcMessage {
public:
   using allocator_type = std::pmr::polymorphic_allocator<>;

private:
   pmr_unique_ptr<dbcppp::IMessage> message_container_;
   const dbcppp::IMessage* message_;

   std::pmr::vector<pmr_unique_ptr<dbcppp::ISignal>> signals_container_;
   std::pmr::unordered_map<size_t, const dbcppp::ISignal*> signals_;

   allocator_type allocator_;

   void RegisterSignal(const dbcppp::ISignal* signal);

public:
   using SignalReader = std::function<float (size_t)>;

   explicit DbcMessage(
      std::allocator_arg_t,
      allocator_type alloc,
      const dbcppp::IMessage* message);
   DbcMessage(
      std::allocator_arg_t,
      allocator_type alloc,
      uint32_t id,
      std::pmr::string name,
      uint32_t message_size);

   DbcMessage(const DbcMessage&) = delete;
   DbcMessage& operator=(const DbcMessage&) noexcept = default;
   DbcMessage& operator=(DbcMessage&&) noexcept = default;
   DbcMessage(DbcMessage&&) noexcept = default;
   ~DbcMessage() = default;

   DbcMessage(DbcMessage&& other, allocator_type alloc)
      : message_container_(std::move(other.message_container_)),
      message_(std::move(other.message_)),
      signals_container_(std::move(other.signals_container_), alloc),
      signals_(std::move(other.signals_), alloc),
      allocator_(alloc) {}

   uint32_t Id() const;
   std::string_view Name() const;
   uint32_t MessageSize() const;

   void AddSignal(std::pmr::string name, uint32_t start_bit, uint32_t size_bits, float factor, float offset, std::pmr::string unit);
   bool HasSignal(size_t signal_name_hash) const;
   bool HasSignal(const std::string_view signal_name) const;

   double GetSignalValue(size_t signal_name_hash, const void* bytes) const;
   std::vector<uint8_t> EncodeMessage(const SignalReader& signal_reader);
};

} // namespace eerie_leap::subsys::dbc
