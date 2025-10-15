#!/usr/bin/env python3

import io
import struct
import zlib

from datetime import datetime, timezone

class LogDataRecord:
    LOG_DATA_RECORD_START_MARK = bytes.fromhex('4C52534D')

    def __init__(self):
        # self.record_start_mark: bytes         # 4 bytes
        self.time_delta_ms: int                 # 4 bytes
        self.sensor_id: int                     # 4 bytes
        # self.data_length: bytes               # 1 byte
        self.data: bytes                        # data_length bytes
        self.crc: int                           # 4 bytes

    def _read_data(self, stream: io.IOBase, data_length: int):
        data_bytes = bytes(stream.read(data_length))
        if not data_bytes:
            return None

        return data_bytes

    def get_data(self):
        return self.data

    @classmethod
    def create(cls, stream: io.IOBase):
        crc_validation = 0

        record_start_mark: bytes = bytes(stream.read(4))
        if not record_start_mark or record_start_mark[::-1] != LogDataRecord.LOG_DATA_RECORD_START_MARK:
            return None
        crc_validation = zlib.crc32(record_start_mark, crc_validation)

        instance = cls()

        timestamp_delta = bytes(stream.read(4))
        if not timestamp_delta:
            return None
        instance.time_delta_ms = struct.unpack('<I', timestamp_delta)[0]
        crc_validation = zlib.crc32(timestamp_delta, crc_validation)

        sensor_id = bytes(stream.read(4))
        if not sensor_id:
            return None
        instance.sensor_id = struct.unpack('<I', sensor_id)[0]
        crc_validation = zlib.crc32(sensor_id, crc_validation)

        data_length = bytes(stream.read(1))
        if not data_length:
            return None
        crc_validation = zlib.crc32(data_length, crc_validation)

        data_bytes = instance._read_data(stream, data_length[0])
        if not data_bytes:
            return None
        instance.data = data_bytes
        crc_validation = zlib.crc32(data_bytes, crc_validation)

        crc_bytes = bytes(stream.read(4))
        crc = struct.unpack('<I', crc_bytes)[0]
        if not crc:
            return None
        instance.crc = crc

        if crc_validation != crc:
            return None

        return instance

    def to_dictionary(self):  # type: ignore
        return {
            'timestamp_delta': self.time_delta_ms,
            'sensor_id': self.sensor_id,
            'data': self.get_data(),
            'crc': self.crc
        } # type: ignore

class LogDataRecordAnalog(LogDataRecord):
    _data_length_expected: int = 4

    def _read_data(self, stream: io.IOBase, data_length: int):
        if data_length != LogDataRecordAnalog._data_length_expected:
            return None

        return LogDataRecord._read_data(self, stream, data_length)

    def get_data(self):
        return struct.unpack('<f', self.data)[0]
