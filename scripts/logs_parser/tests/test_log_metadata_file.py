import io
import json

from src.log_metadata_file import LogMetadata

def test_log_metadata_file_parsed_correctly():
    with open("tests/logs_metadata/log_metadata_b67001c5.elm", "rb") as f:
        with open("cbor_schemas/sensor_config.cddl", "r") as cddl_file:
            cddl = cddl_file.read()
            metadata = LogMetadata.create(f, cddl)

            assert metadata is not None

            assert metadata.header is not None
            assert metadata.header.file_type == "ELLM"
            assert metadata.header.version == 1
            assert metadata.header.reserved == b"\x00\x00\x00\x00\x00\x00\x00\x00"


            assert metadata.sensors is not None
            assert len(metadata.sensors.sensors) == 3

            assert metadata.sensors.sensors[0].id == "sensor_1"
            assert metadata.sensors.sensors[0].id_hash == 2348664336
            assert metadata.sensors.sensors[0].metadata.name == "Sensor 1"
            assert metadata.sensors.sensors[0].metadata.unit == "km/h"
            assert metadata.sensors.sensors[0].metadata.description == "Test Sensor 1"
            assert metadata.sensors.sensors[0].configuration.type == 0
            assert metadata.sensors.sensors[0].configuration.sampling_rate_ms == 10
            assert metadata.sensors.sensors[0].configuration.interpolation_method == 1
            assert metadata.sensors.sensors[0].configuration.channel == [0]
            assert metadata.sensors.sensors[0].configuration.calibration_table == [
                {
                    "float32float": [
                        {
                            "float32float_key": 0.0,
                            "float32float": 0.0
                        },
                        {
                            "float32float_key": 5.0,
                            "float32float": 100.0
                        }
                    ]
                }
            ]
            assert metadata.sensors.sensors[0].configuration.expression == []

            assert metadata.sensors.sensors[1].id == "sensor_2"
            assert metadata.sensors.sensors[1].id_hash == 1409275229
            assert metadata.sensors.sensors[1].metadata.name == "Sensor 2"
            assert metadata.sensors.sensors[1].metadata.unit == "km/h"
            assert metadata.sensors.sensors[1].metadata.description == "Test Sensor 2"
            assert metadata.sensors.sensors[1].configuration.type == 0
            assert metadata.sensors.sensors[1].configuration.sampling_rate_ms == 500
            assert metadata.sensors.sensors[1].configuration.interpolation_method == 2
            assert metadata.sensors.sensors[1].configuration.channel == [1]
            assert metadata.sensors.sensors[1].configuration.calibration_table == [
                {
                    "float32float": [
                        {
                            "float32float_key": 0.0,
                            "float32float": 0.0
                        },
                        {
                            "float32float_key": 1.0,
                            "float32float": 29.0
                        },
                        {
                            "float32float_key": 2.0,
                            "float32float": 111.0
                        },
                        {
                            "float32float_key": 2.5,
                            "float32float": 162.0
                        },
                        {
                            "float32float_key": 3.299999952316284,
                            "float32float": 200.0
                        }
                    ]
                }
            ]
            assert metadata.sensors.sensors[1].configuration.expression == ["x * 4 + 1.6"]

            assert metadata.sensors.sensors[2].id == "sensor_3"
            assert metadata.sensors.sensors[2].id_hash == 811985203
            assert metadata.sensors.sensors[2].metadata.name == "Sensor 3"
            assert metadata.sensors.sensors[2].metadata.unit == "km/h"
            assert metadata.sensors.sensors[2].metadata.description == "Test Sensor 3"
            assert metadata.sensors.sensors[2].configuration.type == 1
            assert metadata.sensors.sensors[2].configuration.sampling_rate_ms == 2000
            assert metadata.sensors.sensors[2].configuration.interpolation_method == 0
            assert metadata.sensors.sensors[2].configuration.channel == []
            assert metadata.sensors.sensors[2].configuration.calibration_table == []
            assert metadata.sensors.sensors[2].configuration.expression == ["{sensor_1} + 8.34"]


def test_log_metadata_file_parsed_json_correctly():
    with open("tests/logs_metadata/log_metadata_b67001c5.elm", "rb") as f:
        with open("cbor_schemas/sensor_config.cddl", "r") as cddl_file:
            cddl = cddl_file.read()
            metadata = LogMetadata.create(f, cddl)

            assert metadata is not None

            with io.StringIO() as output_json_file:
                res = metadata.to_json(output_json_file)
                assert res

                json_data = json.loads(output_json_file.getvalue())
                assert json_data is not None

                assert json_data["header"] is not None
                assert json_data["header"]["file_type"] == "ELLM"
                assert json_data["header"]["version"] == 1
                assert json_data["header"]["reserved"] == "0000000000000000"

                assert json_data["sensors"] is not None
                assert len(json_data["sensors"]) == 3

                assert json_data["sensors"][0]["id"] == "sensor_1"
                assert json_data["sensors"][0]["id_hash"] == 2348664336
                assert json_data["sensors"][0]["metadata"]["name"] == "Sensor 1"
                assert json_data["sensors"][0]["metadata"]["unit"] == "km/h"
                assert json_data["sensors"][0]["metadata"]["description"] == "Test Sensor 1"
                assert json_data["sensors"][0]["configuration"]["type"] == 0
                assert json_data["sensors"][0]["configuration"]["sampling_rate_ms"] == 10
                assert json_data["sensors"][0]["configuration"]["interpolation_method"] == 1
                assert json_data["sensors"][0]["configuration"]["channel"] == [0]
                assert json_data["sensors"][0]["configuration"]["calibration_table"] == [
                {
                    "float32float": [
                        {
                            "float32float_key": 0.0,
                            "float32float": 0.0
                        },
                        {
                            "float32float_key": 5.0,
                            "float32float": 100.0
                        }
                    ]
                }
            ]
            assert json_data["sensors"][0]["configuration"]["expression"] == []

            assert json_data["sensors"][1]["id"] == "sensor_2"
            assert json_data["sensors"][1]["id_hash"] == 1409275229
            assert json_data["sensors"][1]["metadata"]["name"] == "Sensor 2"
            assert json_data["sensors"][1]["metadata"]["unit"] == "km/h"
            assert json_data["sensors"][1]["metadata"]["description"] == "Test Sensor 2"
            assert json_data["sensors"][1]["configuration"]["type"] == 0
            assert json_data["sensors"][1]["configuration"]["sampling_rate_ms"] == 500
            assert json_data["sensors"][1]["configuration"]["interpolation_method"] == 2
            assert json_data["sensors"][1]["configuration"]["channel"] == [1]
            assert json_data["sensors"][1]["configuration"]["calibration_table"] == [
                {
                    "float32float": [
                        {
                            "float32float_key": 0.0,
                            "float32float": 0.0
                        },
                        {
                            "float32float_key": 1.0,
                            "float32float": 29.0
                        },
                        {
                            "float32float_key": 2.0,
                            "float32float": 111.0
                        },
                        {
                            "float32float_key": 2.5,
                            "float32float": 162.0
                        },
                        {
                            "float32float_key": 3.299999952316284,
                            "float32float": 200.0
                        }
                    ]
                }
            ]
            assert json_data["sensors"][1]["configuration"]["expression"] == ["x * 4 + 1.6"]

            assert json_data["sensors"][2]["id"] == "sensor_3"
            assert json_data["sensors"][2]["id_hash"] == 811985203
            assert json_data["sensors"][2]["metadata"]["name"] == "Sensor 3"
            assert json_data["sensors"][2]["metadata"]["unit"] == "km/h"
            assert json_data["sensors"][2]["metadata"]["description"] == "Test Sensor 3"
            assert json_data["sensors"][2]["configuration"]["type"] == 1
            assert json_data["sensors"][2]["configuration"]["sampling_rate_ms"] == 2000
            assert json_data["sensors"][2]["configuration"]["interpolation_method"] == 0
            assert json_data["sensors"][2]["configuration"]["channel"] == []
            assert json_data["sensors"][2]["configuration"]["calibration_table"] == []
            assert json_data["sensors"][2]["configuration"]["expression"] == ["{sensor_1} + 8.34"]
