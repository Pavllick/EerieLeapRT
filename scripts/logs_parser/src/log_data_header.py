#!/usr/bin/env python3

import io
import struct
import zlib

from datetime import datetime, timezone

class LogDataHeader:
    LOG_DATA_FILE_TYPE = bytes.fromhex('444C4C45')[::-1].decode("ascii")

    def __init__(self):
        self.file_type: str                     # 4 bytes
        self.version: int                       # 4 bytes
        self.log_metadata_file_version: bytes   # 4 bytes
        self.start_time_ns: int                 # 8 bytes
        self.reserved: bytes                    # 8 bytes
        self.crc: int                           # 4 bytes

    @staticmethod
    def create(stream: io.IOBase):
        instance = LogDataHeader()
        crc_validation = 0

        file_type = bytes(stream.read(4))
        if not file_type:
            return None
        instance.file_type = file_type.decode("ascii")
        if instance.file_type != LogDataHeader.LOG_DATA_FILE_TYPE:
            return None
        crc_validation = zlib.crc32(file_type, crc_validation)

        version = bytes(stream.read(4))
        if not version:
            return None
        instance.version = struct.unpack('<I', version)[0]
        crc_validation = zlib.crc32(version, crc_validation)

        log_metadata_file_version = bytes(stream.read(4))
        if not log_metadata_file_version:
            return None
        instance.log_metadata_file_version = log_metadata_file_version[::-1]
        crc_validation = zlib.crc32(log_metadata_file_version, crc_validation)

        start_timestamp = bytes(stream.read(8))
        if not start_timestamp:
            return None
        instance.start_time_ns = struct.unpack('<Q', start_timestamp)[0]
        crc_validation = zlib.crc32(start_timestamp, crc_validation)

        reserved = bytes(stream.read(8))
        if not reserved:
            return None
        instance.reserved = reserved
        crc_validation = zlib.crc32(reserved, crc_validation)

        instance.crc = struct.unpack('<I', bytes(stream.read(4)))[0]
        if not instance.crc:
            return None

        if instance.crc != crc_validation:
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
            'crc': self.crc
        } # type: ignore
