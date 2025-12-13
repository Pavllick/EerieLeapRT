#include "dbcppp/BitTiming.h"

using namespace dbcppp;

BitTiming::BitTiming()
    : _baudrate(0)
    , _BTR1(0)
    , _BTR2(0)
{}
BitTiming::BitTiming(
      uint64_t baudrate
    , uint64_t BTR1
    , uint64_t BTR2)

    : _baudrate(baudrate)
    , _BTR1(BTR1)
    , _BTR2(BTR2)
{}
BitTiming BitTiming::Clone() const
{
    return {*this};
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
