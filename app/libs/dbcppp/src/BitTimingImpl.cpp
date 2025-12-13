#include "dbcppp/BitTiming.h"

using namespace dbcppp;

BitTiming BitTiming::Create(
      std::pmr::memory_resource* mr
    , uint64_t baudrate
    , uint64_t BTR1
    , uint64_t BTR2)
{
    return {std::allocator_arg, mr, baudrate, BTR1, BTR2};
}
BitTiming::BitTiming(std::allocator_arg_t, allocator_type alloc)
    : _baudrate(0)
    , _BTR1(0)
    , _BTR2(0)
    , _allocator(alloc)
{}
BitTiming::BitTiming(
      std::allocator_arg_t
    , allocator_type alloc
    , uint64_t baudrate
    , uint64_t BTR1
    , uint64_t BTR2)

    : _baudrate(baudrate)
    , _BTR1(BTR1)
    , _BTR2(BTR2)
    , _allocator(alloc)
{}
BitTiming BitTiming::Clone() const
{
    return {*this, _allocator};
}
uint64_t BitTiming::Baudrate() const
{
    return _baudrate;
}
uint64_t BitTiming::BTR1() const
{
    return _BTR1;
}
uint64_t BitTiming::BTR2() const
{
    return _BTR2;
}
bool BitTiming::operator==(const BitTiming& rhs) const
{
    bool result = true;
    result &= _baudrate == rhs.Baudrate();
    result &= _BTR1 == rhs.BTR1();
    result &= _BTR2 == rhs.BTR2();
    return result;
}
bool BitTiming::operator!=(const BitTiming& rhs) const
{
    return !(*this == rhs);
}
