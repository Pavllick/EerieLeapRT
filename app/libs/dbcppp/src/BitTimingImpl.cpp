#include "BitTimingImpl.h"

using namespace dbcppp;

pmr_unique_ptr<IBitTiming> IBitTiming::Create(
      std::pmr::memory_resource* mr
    , uint64_t baudrate
    , uint64_t BTR1
    , uint64_t BTR2)
{
    return make_unique_pmr<BitTimingImpl>(mr, baudrate, BTR1, BTR2);
}
BitTimingImpl::BitTimingImpl(std::allocator_arg_t, allocator_type alloc)
    : _baudrate(0)
    , _BTR1(0)
    , _BTR2(0)
    , _allocator(alloc)
{}
BitTimingImpl::BitTimingImpl(
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
pmr_unique_ptr<IBitTiming> BitTimingImpl::Clone() const
{
    return make_unique_pmr<BitTimingImpl>(_allocator, *this);
}
uint64_t BitTimingImpl::Baudrate() const
{
    return _baudrate;
}
uint64_t BitTimingImpl::BTR1() const
{
    return _BTR1;
}
uint64_t BitTimingImpl::BTR2() const
{
    return _BTR2;
}
bool BitTimingImpl::operator==(const IBitTiming& rhs) const
{
    bool result = true;
    result &= _baudrate == rhs.Baudrate();
    result &= _BTR1 == rhs.BTR1();
    result &= _BTR2 == rhs.BTR2();
    return result;
}
bool BitTimingImpl::operator!=(const IBitTiming& rhs) const
{
    return !(*this == rhs);
}
