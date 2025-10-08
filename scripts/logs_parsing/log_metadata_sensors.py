#!/usr/bin/env python3

import io

from cbor_parser import CborParser, DictWithAttrs

class LogMetadataSensors:
    def __init__(self):
        self.sensors: DictWithAttrs

    @staticmethod
    def create(stream: io.IOBase, cddl: str):
        instance = LogMetadataSensors()
        cbor_parser = CborParser(cddl)

        content = stream.read()
        if not content or len(content) < 4:
            return None

        sensors = content[:-4]
        crc = content[-4:]

        sensors = cbor_parser.parse_cbor_hex(sensors, "SensorsConfig")
        if not sensors:
            return None

        instance.sensors = sensors

        return instance

    def to_dictionary(self): # type: ignore
        return self.sensors
