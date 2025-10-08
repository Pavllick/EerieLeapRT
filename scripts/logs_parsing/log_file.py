#!/usr/bin/env python3

import os
import io
import json

from log_data_header import LogDataHeader
from log_data_record import LogDataRecordAnalog

class LogFile:
    @staticmethod
    def to_json(input: io.IOBase, output: io.IOBase):
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

    @staticmethod
    def to_csv(input: io.IOBase, output: io.IOBase):
        output.write("Time (sec),sensor_1\n")

        header = LogDataHeader.create(input)
        if not header:
            return False

        while record := LogDataRecordAnalog.create(input):
            output.write(str(record.time_delta_ms / 1000)
                            + ","
                            + str(record.get_data())
                            + "\n")

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
