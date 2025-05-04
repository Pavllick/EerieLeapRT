#include <set>
#include <zephyr/ztest.h>

#include "utilities/guid/guid.hpp"
#include "utilities/guid/guid_generator.h"

using namespace eerie_leap::utilities::guid;

ZTEST_SUITE(guid, NULL, NULL, NULL, NULL, NULL);

ZTEST(guid, test_guid_is_nonzero) {
    GuidGenerator generator;
    Guid guid = generator.Generate();

    zassert_equal(guid.counter, 0);
    zassert_not_equal(guid.device_hash, 0);
    zassert_not_equal(guid.AsUint64(), 0);
}

ZTEST(guid, test_guids_are_unique) {
    GuidGenerator generator;
    Guid guid0 = generator.Generate();

    uint16_t device_hash = guid0.device_hash;
    std::set<uint16_t> counters;
    counters.insert(guid0.counter);
    std::set<uint32_t> timestamps;
    timestamps.insert(guid0.timestamp);
    std::set<uint64_t> guid_nums;
    guid_nums.insert(guid0.AsUint64());

    for(int i = 1; i < 100; ++i) {
        Guid guid = generator.Generate();

        zassert_false(counters.count(guid.counter));
        counters.insert(guid.counter);
        zassert_false(guid_nums.count(guid.AsUint64()));
        guid_nums.insert(guid.AsUint64());

        zassert_equal(guid.device_hash, device_hash);
    }
}