#!/usr/bin/env python3

import io
import struct

from datetime import datetime, timezone

class LogDataHeader:
    LOG_DATA_FILE_TYPE = bytes.fromhex('444C4C45')[::-1].decode("ascii")

    def __init__(self):
        self.file_type: str                     # 4 bytes
        self.version: int                       # 4 bytes
        self.log_metadata_file_version: bytes   # 4 bytes
        self.start_time_ns: int                 # 8 bytes
        self.reserved: bytes                    # 8 bytes
        self.crc: bytes                         # 4 bytes

    @staticmethod
    def create(stream: io.IOBase):
        instance = LogDataHeader()

        instance.file_type = bytes(stream.read(4)).decode("ascii")
        if not instance.file_type or instance.file_type != LogDataHeader.LOG_DATA_FILE_TYPE:
            return None

        instance.version = struct.unpack('<I', bytes(stream.read(4)))[0]
        if not instance.version:
            return None

        instance.log_metadata_file_version = bytes(stream.read(4))[::-1]
        if not instance.log_metadata_file_version:
            return None

        start_timestamp = bytes(stream.read(8))
        if not start_timestamp:
            return None
        instance.start_time_ns = struct.unpack('<Q', start_timestamp)[0]

        instance.reserved = bytes(stream.read(8))
        if not instance.reserved:
            return None

        instance.crc = bytes(stream.read(4))[::-1]
        if not instance.crc:
            return None

        return instance

    def get_start_time(self):
        return datetime.fromtimestamp(self.start_time_ns / 1e9, tz=timezone.utc)

    def to_dictionary(self): # type: ignore
        return {
            'file_type': self.file_type,
            'version': self.version,
            'log_metadata_file_version': self.log_metadata_file_version.hex(),
            'start_timestamp': self.start_time_ns,
            'reserved': self.reserved.hex(),
            'crc': self.crc.hex().upper()
        } # type: ignore
