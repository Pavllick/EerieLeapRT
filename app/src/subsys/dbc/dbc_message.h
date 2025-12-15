#pragma once

#include <zephyr/kernel.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

#include <dbcppp/Network.h>

#include <eerie_memory.hpp>

namespace eerie_leap::subsys::dbc {

using namespace eerie_memory;

class DbcMessage {
public:
   using allocator_type = std::pmr::polymorphic_allocator<>;

private:
   dbcppp::Message* message_;
   int frame_id_;

   std::pmr::unordered_map<size_t, const dbcppp::Signal*> signals_;

   allocator_type allocator_;

   void RegisterSignal(const dbcppp::Signal* signal);
   dbcppp::Signal* GetDbcSignal(size_t signal_name_hash);

   // HACK: Every time a new signal is added to the DBC file, the signal address
   // might change, due to container resizing. This function updates the signal references
   // to the new addresses.
   void UpdateSignalReferences();

public:
   using SignalReader = std::function<float (size_t)>;

   explicit DbcMessage(std::allocator_arg_t, allocator_type alloc, dbcppp::Message* message);

   DbcMessage(const DbcMessage&) = delete;
   DbcMessage& operator=(const DbcMessage&) noexcept = default;
   DbcMessage& operator=(DbcMessage&&) noexcept = default;
   DbcMessage(DbcMessage&&) noexcept = default;
   ~DbcMessage() = default;

   DbcMessage(DbcMessage&& other, allocator_type alloc)
      : message_(other.message_),
      signals_(std::move(other.signals_), alloc),
      allocator_(alloc) {}

   // HACK: Every time a new message is added to the DBC file, the message address
   // might change, due to container resizing. This function updates the message reference
   // to the new address.
   void UpdateReference(dbcppp::Message* message);

   uint32_t Id() const;
   std::string_view Name() const;
   uint32_t MessageSize() const;

   void AddSignal(std::pmr::string name, uint32_t start_bit, uint32_t size_bits, float factor, float offset, std::pmr::string unit);
   bool HasSignal(size_t signal_name_hash);
   bool HasSignal(std::string_view signal_name);

   double GetSignalValue(size_t signal_name_hash, const void* bytes);
   std::vector<uint8_t> EncodeMessage(const SignalReader& signal_reader);

   const std::pmr::unordered_map<size_t, const dbcppp::Signal*>& GetSignals() const { return signals_; }
};

} // namespace eerie_leap::subsys::dbc
