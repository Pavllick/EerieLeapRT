#!/usr/bin/env python3

import io
import json
import struct
import zlib

from src.log_metadata_header import LogMetadataHeader
from src.log_metadata_sensors import LogMetadataSensors

class LogMetadata:
    def __init__(self):
        self.header: LogMetadataHeader      # LogMetadataHeader.LENGTH_BYTES bytes
        self.sensors: LogMetadataSensors    # variable
        self.crc: int                       # 4 bytes
        self.cddl: str

    @classmethod
    def create(cls, data_stream: io.IOBase, cddl: str):
        instance = cls()

        data = data_stream.read()
        if not data or len(data) < LogMetadataHeader.LENGTH_BYTES + 4:
            return None

        header = LogMetadataHeader.create(data)
        if not header:
            return None
        instance.header = header

        sensors_data = data[LogMetadataHeader.LENGTH_BYTES:-4]
        crc = data[-4:]

        sensors = LogMetadataSensors.create(sensors_data, cddl)
        if not sensors:
            return None

        instance.sensors = sensors
        instance.crc = struct.unpack('<I', crc)[0]
        instance.cddl = cddl

        if instance.crc != zlib.crc32(data[:-4]):
            return None

        return instance

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
    with open("tests/logs_metadata/log_metadata_b67001c5.elm", "rb") as f:
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
