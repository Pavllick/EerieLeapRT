#!/usr/bin/env python3

import io
import json

from log_metadata_header import LogMetadataHeader
from log_metadata_sensors import LogMetadataSensors

class LogMetadata:
    @staticmethod
    def to_json(data_stream: io.IOBase, cddl: str, output: io.IOBase):
        output.write("{")
        output.write("\"header\": ")

        header = LogMetadataHeader.create(data_stream)
        if not header:
            return False

        output.write(json.dumps(header.to_dictionary()))
        output.write(",")
        output.write("\"sensors\": ")

        sensors = LogMetadataSensors.create(data_stream, cddl)
        if not sensors:
            return False

        output.write(json.dumps(sensors.to_dictionary()))

        output.write("}")

        return True

# if __name__ == "__main__":
#     with open("logs_metadata/log_metadata_b67001c5.elm", "rb") as f:
#         with open("cbor_schemas/sensor_config.cddl", "r") as cddl_file:
#             cddl = cddl_file.read()

#             with io.StringIO() as output_json_file:
#                 res = LogMetadata.to_json(f, cddl, output_json_file)

#                 if res:
#                     formatted = json.dumps(json.loads(output_json_file.getvalue()), indent=2)
#                     print(formatted)
#                 else:
#                     print("Failed to parse log data.")
