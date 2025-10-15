#!/usr/bin/env python3

import struct

class LogMetadataHeader:
    LOG_METADATA_FILE_TYPE = bytes.fromhex('4D4C4C45')[::-1].decode("ascii")
    LENGTH_BYTES = 16

    def __init__(self):
        self.file_type: str     # 4 bytes
        self.version: int       # 4 bytes
        self.reserved: bytes    # 8 bytes

    @staticmethod
    def create(data: bytes):
        instance = LogMetadataHeader()

        instance.file_type = bytes(data[:4]).decode("ascii")
        if not instance.file_type or instance.file_type != LogMetadataHeader.LOG_METADATA_FILE_TYPE:
            return None

        instance.version = struct.unpack('<I', bytes(data[4:8]))[0]
        if not instance.version:
            return None

        instance.reserved = bytes(data[8:16])
        if not instance.reserved:
            return None

        return instance

    def to_dictionary(self): # type: ignore
        return {
            'file_type': self.file_type,
            'version': self.version,
            'reserved': self.reserved.hex(),
        } # type: ignore
