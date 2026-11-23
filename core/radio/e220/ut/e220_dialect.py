"""Test dialect of the E220 end-to-end test (service srp.Example), generated with pymavlink's mavgen.

The platform test helpers no longer ship a fixture dialect, so the test keeps its own.
"""
import importlib.util
import os
import shutil
import sys
import tempfile

import pymavlink
from pymavlink.generator import mavgen

MDEF_DIR = os.path.join(os.path.dirname(pymavlink.__file__), "message_definitions", "v1.0")
NAME = "srp_radio"

DIALECT_XML = """<?xml version="1.0"?>
<mavlink>
  <include>minimal.xml</include>
  <version>3</version>
  <messages>
    <message id="42000" name="SRP_EXAMPLE_STATUS">
      <description>Event Status of the Example service</description>
      <field type="uint16_t" name="seq">Sequence number (deduplication)</field>
      <field type="uint16_t" name="instance">Service instance</field>
      <field type="uint8_t" name="state">Status</field>
      <field type="float" name="temp">Temperature</field>
      <field type="uint32_t" name="counter">Counter</field>
      <field type="int16_t" name="info_offset">Flattened info.offset field</field>
    </message>
    <message id="42002" name="SRP_EXAMPLE_NAME">
      <description>Event with a string (max_len 16)</description>
      <field type="uint16_t" name="seq">Sequence number (deduplication)</field>
      <field type="uint16_t" name="instance">Service instance</field>
      <field type="char[16]" name="label">Label</field>
      <field type="double" name="value">Value</field>
    </message>
    <message id="42010" name="SRP_EXAMPLE_SET_MODE_REQ">
      <description>Method SetMode (request)</description>
      <field type="uint16_t" name="seq">Sequence number</field>
      <field type="uint16_t" name="instance">Service instance</field>
      <field type="uint8_t" name="target_system">Target system</field>
      <field type="uint8_t" name="target_component">Target component</field>
      <field type="uint8_t" name="mode">Mode</field>
    </message>
    <message id="42011" name="SRP_EXAMPLE_SET_MODE_RESP">
      <description>Method SetMode (response)</description>
      <field type="uint16_t" name="seq">Sequence number</field>
      <field type="uint16_t" name="instance">Service instance</field>
      <field type="uint8_t" name="target_system">Target system</field>
      <field type="uint8_t" name="target_component">Target component</field>
      <field type="uint8_t" name="status">0 OK, 1 method error, 2 unknown method</field>
      <field type="uint8_t" name="mode">Mode</field>
    </message>
  </messages>
</mavlink>
"""


def load_dialect():
    """Generates the MAVLink 2 dialect module and imports it."""
    d = tempfile.mkdtemp(prefix="mavdialect_", dir=os.environ.get("TEST_TMPDIR"))
    shutil.copy(os.path.join(MDEF_DIR, "minimal.xml"), d)
    with open(os.path.join(d, NAME + ".xml"), "w", encoding="utf-8") as f:
        f.write(DIALECT_XML)
    out = os.path.join(d, NAME + ".py")
    if not mavgen.mavgen(mavgen.Opts(out, wire_protocol="2.0", language="Python3", validate=True),
                         [os.path.join(d, NAME + ".xml")]):
        raise RuntimeError("mavgen rejected the dialect XML")
    spec = importlib.util.spec_from_file_location(NAME + "_e220", out)
    mod = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = mod
    spec.loader.exec_module(mod)
    return mod
