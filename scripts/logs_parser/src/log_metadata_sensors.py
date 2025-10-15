#!/usr/bin/env python3

import io

from src.cbor_parser import CborParser, DictWithAttrs

class LogMetadataSensors:
    def __init__(self):
        self.sensors: DictWithAttrs

    @staticmethod
    def create(data: bytes, cddl: str):
        instance = LogMetadataSensors()
        cbor_parser = CborParser(cddl)

        sensors = cbor_parser.parse_cbor_hex(data, "SensorsConfig")
        if not sensors:
            return None

        instance.sensors = sensors

        return instance

    def to_dictionary(self): # type: ignore
        return self.sensors
