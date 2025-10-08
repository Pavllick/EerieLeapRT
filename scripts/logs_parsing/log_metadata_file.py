#!/usr/bin/env python3

import io
import json

from log_metadata_header import LogMetadataHeader
from log_metadata_sensors import LogMetadataSensors

class LogMetadata:
    def __init__(self, header: LogMetadataHeader, sensors: LogMetadataSensors, cddl: str):
        self.header = header
        self.sensors = sensors
        self.cddl = cddl

    @classmethod
    def create(cls, data_stream: io.IOBase, cddl: str):
        header = LogMetadataHeader.create(data_stream)
        if not header:
            return None

        sensors = LogMetadataSensors.create(data_stream, cddl)
        if not sensors:
            return None

        return cls(header, sensors, cddl)

    def to_json(self, output: io.IOBase):
        output.write("{")
        output.write("\"header\": ")

        output.write(json.dumps(self.header.to_dictionary()))
        output.write(",")

        output.write("\"sensors\": ")
        output.write(json.dumps(self.sensors.to_dictionary()))

        output.write("}")

        return True

if __name__ == "__main__":
    with open("logs_metadata/log_metadata_b67001c5.elm", "rb") as f:
        with open("cbor_schemas/sensor_config.cddl", "r") as cddl_file:
            cddl = cddl_file.read()

            metadata = LogMetadata.create(f, cddl)

            with io.StringIO() as output_json_file:
                res = metadata.to_json(output_json_file)

                if res:
                    formatted = json.dumps(json.loads(output_json_file.getvalue()), indent=2)
                    print(formatted)
                else:
                    print("Failed to parse log data.")
