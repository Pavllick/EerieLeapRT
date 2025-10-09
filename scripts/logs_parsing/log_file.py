#!/usr/bin/env python3

import os
import io
import json

from log_data_header import LogDataHeader
from log_data_record import LogDataRecordAnalog
from log_metadata_file import LogMetadata

class LogFile:
    def __init__(self, metadata: LogMetadata):
        self.metadata = metadata

    def to_json(self, input: io.IOBase, output: io.IOBase):
        output.write("{")
        output.write("\"header\": ")

        header = LogDataHeader.create(input)
        if not header:
            return False

        output.write(json.dumps(header.to_dictionary()))
        output.write(",")
        output.write("\"records\": [")

        first_record = True

        while record := LogDataRecordAnalog.create(input):
            if not first_record:
                output.write(",")
            first_record = False

            output.write(json.dumps(record.to_dictionary()))

        output.write("]}")

        return True

    def to_csv(self, input: io.IOBase, output: io.IOBase):
        sensor_ids = []

        if self.metadata:
            sensors = self.metadata.sensors.to_dictionary()
            header: str = "\"Time (sec)\""

            for i in range(len(sensors)):
                sensor_ids.append(sensors[i].id_hash)

                header += ",\"" + sensors[i].metadata.name
                header += " (" + sensors[i].metadata.unit + ")\""

            output.write(header + "\n")
        else:
            output.write("\"Time (sec)\",\"Value\"\n")

        header = LogDataHeader.create(input)
        if not header:
            return False

        while record := LogDataRecordAnalog.create(input):
            record_line: str = str(record.time_delta_ms / 1000)

            if sensor_ids:
                for i in range(len(sensor_ids)):
                    record_line += ","
                    record_line += str(record.get_data()) if record.sensor_id == sensor_ids[i] else ""
            else:
                record_line += "," + str(record.get_data())

            output.write(record_line + "\n")

        return True

# if __name__ == "__main__":
    # os.makedirs("output", exist_ok=True)

    # if os.path.exists("output/output.json"):
    #     os.remove("output/output.json")

    # with open("data_logs/log_data_6777_39439.ell", "rb") as f:
    #     with open("output/output.json", "w") as output_json_file:
    #         LogFile.to_json(f, output_json_file)

# ====================================================

    # with open("data_logs/log_data_6777_39439.ell", "rb") as f:
    #     with io.StringIO() as output_json_file:
    #         res = LogFile.to_json(f, output_json_file)

    #         if res:
    #             formatted = json.dumps(json.loads(output_json_file.getvalue()), indent=2)
    #             print(formatted)
    #         else:
    #             print("Failed to parse log data.")

# ====================================================

    # if os.path.exists("output/output.csv"):
    #     os.remove("output/output.csv")

    # with open("data_logs/log_data_6777_39439.ell", "rb") as f:
    #     with open("output/output.csv", "w") as output_csv_file:
    #         LogFile.to_csv(f, output_csv_file)

# ====================================================

    # with open("data_logs/log_data_6777_39439.ell", "rb") as f:
    #     with io.StringIO() as output_csv_file:
    #         LogFile.to_csv(f, output_csv_file)

    #         print(output_csv_file.getvalue())

# ====================================================

def print_test_json():
    metadata = None

    with open("logs_metadata/log_metadata_b67001c5.elm", "rb") as f:
        with open("cbor_schemas/sensor_config.cddl", "r") as cddl_file:
            cddl = cddl_file.read()

            metadata = LogMetadata.create(f, cddl)
            if not metadata:
                return False

    log_file = LogFile(metadata)

    with open("data_logs/log_data_6777_39439.ell", "rb") as f:
        with io.StringIO() as output_json_file:
            res = log_file.to_json(f, output_json_file)

            if res:
                formatted = json.dumps(json.loads(output_json_file.getvalue()), indent=2)
                print(formatted)
            else:
                print("Failed to parse log data.")

    return True

def print_test_csv():
    metadata = None

    with open("logs_metadata/log_metadata_b67001c5.elm", "rb") as f:
        with open("cbor_schemas/sensor_config.cddl", "r") as cddl_file:
            cddl = cddl_file.read()

            metadata = LogMetadata.create(f, cddl)
            if not metadata:
                return False

    log_file = LogFile(metadata)

    with open("data_logs/log_data_6777_39439.ell", "rb") as f:
        with io.StringIO() as output_csv_file:
            res = log_file.to_csv(f, output_csv_file)

            if res:
                print(output_csv_file.getvalue())
            else:
                print("Failed to parse log data.")

    return True

# if __name__ == "__main__":
#     print_test_csv()
