#include <zephyr/ztest.h>

#include "domain/canbus_domain/configuration/parsers/canbus_configuration_cbor_parser.h"
#include "domain/canbus_domain/configuration/parsers/canbus_configuration_json_parser.h"

using namespace eerie_leap::domain::canbus_domain::configuration::parsers;

ZTEST_SUITE(canbus_configuration_parser, NULL, NULL, NULL, NULL, NULL);

CanbusConfiguration canbus_configuration_parser_GetTestConfiguration() {
    CanbusConfiguration canbus_configuration(std::allocator_arg, Mrm::GetDefaultPmr());
    CanChannelConfiguration channel_configuration_0(std::allocator_arg, Mrm::GetDefaultPmr());
    channel_configuration_0.type = CanbusType::CLASSICAL_CAN;
    channel_configuration_0.bus_channel = 0;
    channel_configuration_0.bitrate = 500000;
    channel_configuration_0.data_bitrate = 0;

    auto message_configuration_0_0 = std::make_shared<CanMessageConfiguration>(std::allocator_arg, Mrm::GetDefaultPmr());
    message_configuration_0_0->frame_id = 0x123;
    message_configuration_0_0->name = "EL_FRAME_0";
    message_configuration_0_0->message_size = 8;
    message_configuration_0_0->send_interval_ms = 100;
    channel_configuration_0.message_configurations.emplace_back(std::move(message_configuration_0_0));

    auto message_configuration_0_1 = std::make_shared<CanMessageConfiguration>(std::allocator_arg, Mrm::GetDefaultPmr());
    message_configuration_0_1->frame_id = 0x456;
    message_configuration_0_1->name = "EL_FRAME_1";
    message_configuration_0_1->message_size = 8;
    message_configuration_0_1->send_interval_ms = 200;
    channel_configuration_0.message_configurations.emplace_back(std::move(message_configuration_0_1));

    CanChannelConfiguration channel_configuration_1(std::allocator_arg, Mrm::GetDefaultPmr());
    channel_configuration_1.type = CanbusType::CLASSICAL_CAN;
    channel_configuration_1.bus_channel = 1;
    channel_configuration_1.bitrate = 250000;
    channel_configuration_1.data_bitrate = 0;

    auto message_configuration_1_0 = std::make_shared<CanMessageConfiguration>(std::allocator_arg, Mrm::GetDefaultPmr());
    message_configuration_1_0->frame_id = 0x123;
    message_configuration_1_0->name = "EL_FRAME_0";
    message_configuration_1_0->message_size = 8;
    message_configuration_1_0->send_interval_ms = 100;
    channel_configuration_1.message_configurations.emplace_back(std::move(message_configuration_1_0));

    auto message_configuration_1_1 = std::make_shared<CanMessageConfiguration>(std::allocator_arg, Mrm::GetDefaultPmr());
    message_configuration_1_1->frame_id = 0x456;
    message_configuration_1_1->name = "EL_FRAME_1";
    message_configuration_1_1->message_size = 8;
    message_configuration_1_1->send_interval_ms = 200;
    channel_configuration_1.message_configurations.emplace_back(std::move(message_configuration_1_1));

    auto message_configuration_1_2 = std::make_shared<CanMessageConfiguration>(std::allocator_arg, Mrm::GetDefaultPmr());
    message_configuration_1_2->frame_id = 0x789;
    message_configuration_1_2->name = "EL_FRAME_2";
    message_configuration_1_2->message_size = 8;
    message_configuration_1_2->send_interval_ms = 300;
    channel_configuration_1.message_configurations.emplace_back(std::move(message_configuration_1_2));

    CanChannelConfiguration channel_configuration_2(std::allocator_arg, Mrm::GetDefaultPmr());
    channel_configuration_2.type = CanbusType::CANFD;
    channel_configuration_2.bus_channel = 2;
    channel_configuration_2.bitrate = 125000;
    channel_configuration_2.data_bitrate = 0;

    canbus_configuration.channel_configurations.emplace(channel_configuration_0.bus_channel, std::move(channel_configuration_0));
    canbus_configuration.channel_configurations.emplace(channel_configuration_1.bus_channel, std::move(channel_configuration_1));
    canbus_configuration.channel_configurations.emplace(channel_configuration_2.bus_channel, std::move(channel_configuration_2));

    return canbus_configuration;
}

void canbus_configuration_parser_CompareCanbusConfigurations(
    CanbusConfiguration& canbus_configuration,
    CanbusConfiguration& deserialized_configuration) {

    zassert_equal(deserialized_configuration.channel_configurations.size(), canbus_configuration.channel_configurations.size());

    for(auto& [bus_channel, channel_configuration] : canbus_configuration.channel_configurations) {
        zassert_equal(deserialized_configuration.channel_configurations[bus_channel].bus_channel, channel_configuration.bus_channel);
        zassert_equal(deserialized_configuration.channel_configurations[bus_channel].bitrate, channel_configuration.bitrate);

        zassert_equal(deserialized_configuration.channel_configurations[bus_channel].message_configurations.size(), channel_configuration.message_configurations.size());

        for(int j = 0; j < channel_configuration.message_configurations.size(); j++) {
            zassert_equal(deserialized_configuration.channel_configurations[bus_channel].message_configurations[j]->frame_id, channel_configuration.message_configurations[j]->frame_id);
            zassert_equal(deserialized_configuration.channel_configurations[bus_channel].message_configurations[j]->send_interval_ms, channel_configuration.message_configurations[j]->send_interval_ms);
        }
    }
}

ZTEST(canbus_configuration_parser, test_CborSerializeDeserialize) {
    CanbusConfigurationCborParser canbus_configuration_cbor_parser(nullptr);

    auto canbus_configuration = canbus_configuration_parser_GetTestConfiguration();

    auto serialized_canbus_configuration = canbus_configuration_cbor_parser.Serialize(canbus_configuration);
    auto deserialized_canbus_configuration = canbus_configuration_cbor_parser.Deserialize(Mrm::GetDefaultPmr(), *serialized_canbus_configuration);

    canbus_configuration_parser_CompareCanbusConfigurations(canbus_configuration, *deserialized_canbus_configuration);
}

ZTEST(canbus_configuration_parser, test_JsonSerializeDeserialize) {
    CanbusConfigurationJsonParser canbus_configuration_json_parser(nullptr);

    auto canbus_configuration = canbus_configuration_parser_GetTestConfiguration();

    auto json_canbus_configuration = canbus_configuration_json_parser.Serialize(canbus_configuration);
    auto deserialized_canbus_configuration = canbus_configuration_json_parser.Deserialize(Mrm::GetDefaultPmr(), *json_canbus_configuration.get());

    canbus_configuration_parser_CompareCanbusConfigurations(canbus_configuration, *deserialized_canbus_configuration);
}
