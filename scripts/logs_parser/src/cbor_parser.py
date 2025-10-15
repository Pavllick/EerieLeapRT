#!/usr/bin/env python3

from zcbor import DataTranslator

class DictWithAttrs(dict):
    """Dictionary that allows attribute access"""
    def __getattr__(self, key):
        try:
            return self[key]
        except KeyError:
            raise AttributeError(key)

    def __setattr__(self, key, value):
        self[key] = value

def named_tuple_to_dict_with_attrs(obj):
    """Convert to dict but preserve attribute access"""
    if hasattr(obj, '_fields'):
        result = DictWithAttrs()
        for field in obj._fields:
            result[field] = named_tuple_to_dict_with_attrs(getattr(obj, field))
        return result
    elif isinstance(obj, (list, tuple)):
        return [named_tuple_to_dict_with_attrs(item) for item in obj]
    elif isinstance(obj, dict):
        return {key: named_tuple_to_dict_with_attrs(value) for key, value in obj.items()}
    else:
        return obj


class CborParser:
    def __init__(self, cddl_schema):
        self.cddl_schema = cddl_schema

    def parse_cbor_hex(self, data_bytes, type_name):
        """
        Validate and decode CBOR data from hex string

        Args:
            hex_string: CBOR data as hex string (without 0x prefix)
            cddl_schema: CDDL schema definition
            type_name: Name of the type to validate against

        Returns:
            Decoded Python object if valid, None if invalid
        """
        try:
            # Parse CDDL schema
            parsed = DataTranslator.from_cddl(cddl_string=self.cddl_schema, default_max_qty=20)

            # Get the specific type
            validator = parsed.my_types[type_name]

            # Decode and validate
            python_obj = validator.decode_str(data_bytes)

            return named_tuple_to_dict_with_attrs(python_obj[0])

        except ValueError as e:
            print(f"❌ Hex conversion error: {e}")
            return None
        except KeyError as e:
            print(f"❌ Type '{type_name}' not found in schema")
            return None
        except Exception as e:
            print(f"❌ Validation error: {e}")
            return None
