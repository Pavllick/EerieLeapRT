#!/usr/bin/env python3

import io
import struct
import json

class LogMetadataHeader:
    LOG_METADATA_FILE_TYPE = bytes.fromhex('4D4C4C45')[::-1].decode("ascii")

    def __init__(self):
        self.file_type: str     # 4 bytes
        self.version: int       # 4 bytes
        self.reserved: bytes    # 8 bytes

    @staticmethod
    def create(stream: io.IOBase):
        instance = LogMetadataHeader()

        instance.file_type = bytes(stream.read(4)).decode("ascii")
        if not instance.file_type or instance.file_type != LogMetadataHeader.LOG_METADATA_FILE_TYPE:
            return None

        instance.version = struct.unpack('<I', bytes(stream.read(4)))[0]
        if not instance.version:
            return None

        instance.reserved = bytes(stream.read(8))
        if not instance.reserved:
            return None

        return instance

    def to_dictionary(self): # type: ignore
        return {
            'file_type': self.file_type,
            'version': self.version,
            'reserved': self.reserved.hex(),
        } # type: ignore

class LogMetadata:
    @staticmethod
    def to_json(path: str, output: io.IOBase):
        with open(path, "rb") as f:
            output.write("{")
            output.write("\"header\": ")

            header = LogMetadataHeader.create(f)
            if not header:
                return False

            output.write(json.dumps(header.to_dictionary()))

            output.write("}")

        return True

with io.StringIO() as output_json_file:
    LogMetadata.to_json("logs_metadata/log_metadata_b67001c5.elm", output_json_file)

    formatted = json.dumps(json.loads(output_json_file.getvalue()), indent=2)
    print(formatted)
