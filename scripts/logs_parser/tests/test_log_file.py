import io
import json

from src.log_metadata_file import LogMetadata
from src.log_data_record import LogDataRecordAnalog
from src.log_data_header import LogDataHeader
from src.log_file import LogFile


def test_log_data_record_parsed_correctly():
    with open("tests/data_logs/log_data_6777_39439.ell", "rb") as f:
        header = LogDataHeader.create(f)
        assert header is not None

        prev_time_delta_ms = -1
        while record := LogDataRecordAnalog.create(f):
            assert record is not None

            assert record.time_delta_ms >= prev_time_delta_ms
            assert record.sensor_id != 0
            assert record.crc != 0

            prev_time_delta_ms = record.time_delta_ms


def helper_get_logs_metadata():
    with open("tests/logs_metadata/log_metadata_b67001c5.elm", "rb") as f:
        with open("cbor_schemas/sensor_config.cddl", "r") as cddl_file:
            cddl = cddl_file.read()
            metadata = LogMetadata.create(f, cddl)

            return metadata


def test_log_file_parsed_json_correctly():
    metadata = helper_get_logs_metadata()
    assert metadata is not None

    log_file = LogFile(metadata)

    with open("tests/data_logs/log_data_6777_39439.ell", "rb") as f:
        with io.StringIO() as output_json_file:
            res = log_file.to_json(f, output_json_file)
            assert res

            json_data = json.loads(output_json_file.getvalue())
            assert json_data is not None

            header = json_data["header"]
            assert header is not None

            assert header["file_type"] == "ELLD"
            assert header["version"] == 1
            assert header["log_metadata_file_version"] == "b67001c5"
            assert header["start_timestamp"] == 6777000000
            assert header["reserved"] == "0000000000000000"
            assert header["crc"] == 1810016660

            records = json_data["records"]
            assert records is not None

            assert len(records) == 646

            assert records[0]["timestamp_delta"] == 276
            assert records[0]["sensor_id"] == 2348664336
            assert records[0]["data"] == 90.030517578125
            assert records[0]["crc"] == 265681066

            assert records[1]["timestamp_delta"] == 287
            assert records[1]["sensor_id"] == 2348664336
            assert records[1]["data"] == 17.49030876159668
            assert records[1]["crc"] == 4074888436

            assert records[2]["timestamp_delta"] == 351
            assert records[2]["sensor_id"] == 2348664336
            assert records[2]["data"] == 63.895774841308594
            assert records[2]["crc"] == 2114976741


def test_log_file_parsed_csv_correctly():
    metadata = helper_get_logs_metadata()
    assert metadata is not None

    log_file = LogFile(metadata)

    with open("tests/data_logs/log_data_6777_39439.ell", "rb") as f:
        with io.StringIO() as output_csv_file:
            res = log_file.to_csv(f, output_csv_file)
            assert res

            csv_data_lines = output_csv_file.getvalue().split('\n')
            assert len(csv_data_lines) == 647

            assert csv_data_lines[0] == "\"Time (sec)\",\"Sensor 1 (km/h)\",\"Sensor 2 (km/h)\",\"Sensor 3 (km/h)\""

            assert csv_data_lines[1] == "0.276,90.030517578125,,"
            assert csv_data_lines[2] == "0.287,17.49030876159668,,"
            assert csv_data_lines[3] == "0.351,63.895774841308594,,"
            assert csv_data_lines[4] == "0.362,24.90485191345215,,"
            assert csv_data_lines[5] == "0.373,93.38219451904297,,"
            assert csv_data_lines[6] == "0.395,72.91986846923828,,"
            assert csv_data_lines[7] == "0.406,6.309096336364746,,"
