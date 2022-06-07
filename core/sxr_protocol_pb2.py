# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: sxr_protocol.proto
"""Generated protocol buffer code."""
from google.protobuf.internal import builder as _builder
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x12sxr_protocol.proto\x12\x0csxr_protocol\"]\n\nMainPacket\x12\x0f\n\x07\x61\x64\x64ress\x18\x01 \x02(\x07\x12\x0e\n\x06sender\x18\x02 \x02(\x07\x12\x0f\n\x07\x63ommand\x18\x03 \x02(\x07\x12\x0c\n\x04\x64\x61ta\x18\x04 \x01(\x0c\x12\x0f\n\x07version\x18\x05 \x01(\t\"\xaa\x08\n\tAdcStatus\x12\x0c\n\x04name\x18\x01 \x01(\t\x12\x16\n\x07\x65nabled\x18\x02 \x01(\x08:\x05\x66\x61lse\x12\x18\n\tconnected\x18\x03 \x01(\x08:\x05\x66\x61lse\x12\x15\n\rsampling_rate\x18\x04 \x01(\x07\x12\x1b\n\x13inter_channel_delay\x18) \x01(\x02\x12\x1a\n\x12inter_sample_delay\x18* \x01(\x02\x12\x0f\n\x07samples\x18\x05 \x01(\x07\x12\x41\n\x05start\x18\x06 \x01(\x0e\x32\'.sxr_protocol.AdcStatus.EnumStartSource:\tSOFTSTART\x12\x17\n\x0fstart_threshold\x18= \x01(\x02\x12\x17\n\x0fstart_inversion\x18> \x01(\x08\x12\x12\n\nstart_mode\x18? \x01(\x0c\x12@\n\x04stop\x18\x07 \x01(\x0e\x32\'.sxr_protocol.AdcStatus.EnumStartSource:\tSOFTSTART\x12\x16\n\x0estop_threshold\x18G \x01(\x02\x12\x16\n\x0estop_inversion\x18H \x01(\x08\x12\x11\n\tstop_mode\x18I \x01(\x0c\x12G\n\x0c\x63lock_source\x18\x08 \x01(\x0e\x32\'.sxr_protocol.AdcStatus.EnumClockSource:\x08\x43LOCKINT\x12\x37\n\x0bmemory_type\x18\t \x01(\x0e\x32\".sxr_protocol.AdcStatus.EnumMemory\x12\x39\n\x0c\x62oard_status\x18\n \x03(\x0b\x32#.sxr_protocol.AdcStatus.BoardStatus\x1a\xe6\x01\n\x0b\x42oardStatus\x12\x0c\n\x04name\x18\x01 \x01(\t\x12\x0f\n\x07\x65nabled\x18\x02 \x01(\x08\x12\x14\n\x0c\x63hannel_mask\x18\x03 \x01(\x0c\x12I\n\x0e\x63hannel_status\x18\x04 \x03(\x0b\x32\x31.sxr_protocol.AdcStatus.BoardStatus.ChannelStatus\x1aW\n\rChannelStatus\x12\x0c\n\x04name\x18\x01 \x01(\t\x12\x16\n\x07\x65nabled\x18\x02 \x01(\x08:\x05\x66\x61lse\x12\x0f\n\x04gain\x18\x03 \x01(\x02:\x01\x31\x12\x0f\n\x04\x62ias\x18\x04 \x01(\x02:\x01\x30\"W\n\x0f\x45numStartSource\x12\r\n\tSOFTSTART\x10\x00\x12\x0c\n\x08INTSTART\x10\x00\x12\x0c\n\x08\x45XTSTART\x10\x01\x12\x07\n\x03IN0\x10\x02\x12\x0c\n\x08STARTTHR\x10\x02\x1a\x02\x10\x01\";\n\x0f\x45numClockSource\x12\x0c\n\x08\x43LOCKOFF\x10\x00\x12\x0c\n\x08\x43LOCKINT\x10\x01\x12\x0c\n\x08\x43LOCKEXT\x10\x02\"2\n\nEnumMemory\x12\x0b\n\x07MEMHOST\x10\x00\x12\n\n\x06MEMINT\x10\x01\x12\x0b\n\x07MEMFIFO\x10\x03\"7\n\tAmpStatus\x12\r\n\x05gainA\x18\x01 \x02(\x02\x12\r\n\x05gainB\x18\x02 \x02(\x02\x12\x0c\n\x04tail\x18\x03 \x02(\r\"\xb9\x02\n\x0cSystemStatus\x12\x39\n\x05state\x18\x01 \x02(\x0e\x32$.sxr_protocol.SystemStatus.EnumState:\x04IDLE\x12\x30\n\x04\x64\x65vs\x18\x02 \x03(\x0e\x32\".sxr_protocol.SystemStatus.EnumDev\"G\n\tEnumState\x12\x08\n\x04IDLE\x10\x00\x12\x0f\n\x0bMEASUREMENT\x10\x01\x12\x0f\n\x0b\x43\x41LIBRATION\x10\x02\x12\x0e\n\nBACKGROUND\x10\x03\"s\n\x07\x45numDev\x12\x07\n\x03SXR\x10\x00\x12\x07\n\x03\x41\x44\x43\x10\x01\x12\x07\n\x03PX5\x10\x02\x12\x07\n\x03\x41MP\x10\x03\x12\x07\n\x03GSA\x10\x05\x12\n\n\x06MINIX2\x10\x06\x12\x08\n\x04XRAY\x10\x06\x12\n\n\x06SOURCE\x10\x06\x12\x08\n\x04\x46\x45\x35\x35\x10\x07\x12\x0b\n\x07TOKAMAK\x10\x08\x1a\x02\x10\x01*\xb2\x01\n\x08\x43ommands\x12\x08\n\x04INFO\x10\x00\x12\n\n\x06STATUS\x10\x01\x12\x0e\n\nGET_STATUS\x10\x01\x12\x07\n\x03GET\x10\x02\x12\x10\n\x0cGET_SETTINGS\x10\x02\x12\x07\n\x03SET\x10\x03\x12\x0e\n\nSET_STATUS\x10\x03\x12\x0c\n\x08SETTINGS\x10\x03\x12\t\n\x05START\x10\x04\x12\x08\n\x04STOP\x10\x05\x12\n\n\x06REBOOT\x10\x06\x12\x0b\n\x07\x43ONNECT\x10\x07\x12\x0c\n\x08SNAPSHOT\x10\x08\x1a\x02\x10\x01')

_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, globals())
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'sxr_protocol_pb2', globals())
if _descriptor._USE_C_DESCRIPTORS == False:

  DESCRIPTOR._options = None
  _COMMANDS._options = None
  _COMMANDS._serialized_options = b'\020\001'
  _ADCSTATUS_ENUMSTARTSOURCE._options = None
  _ADCSTATUS_ENUMSTARTSOURCE._serialized_options = b'\020\001'
  _SYSTEMSTATUS_ENUMDEV._options = None
  _SYSTEMSTATUS_ENUMDEV._serialized_options = b'\020\001'
  _COMMANDS._serialized_start=1574
  _COMMANDS._serialized_end=1752
  _MAINPACKET._serialized_start=36
  _MAINPACKET._serialized_end=129
  _ADCSTATUS._serialized_start=132
  _ADCSTATUS._serialized_end=1198
  _ADCSTATUS_BOARDSTATUS._serialized_start=766
  _ADCSTATUS_BOARDSTATUS._serialized_end=996
  _ADCSTATUS_BOARDSTATUS_CHANNELSTATUS._serialized_start=909
  _ADCSTATUS_BOARDSTATUS_CHANNELSTATUS._serialized_end=996
  _ADCSTATUS_ENUMSTARTSOURCE._serialized_start=998
  _ADCSTATUS_ENUMSTARTSOURCE._serialized_end=1085
  _ADCSTATUS_ENUMCLOCKSOURCE._serialized_start=1087
  _ADCSTATUS_ENUMCLOCKSOURCE._serialized_end=1146
  _ADCSTATUS_ENUMMEMORY._serialized_start=1148
  _ADCSTATUS_ENUMMEMORY._serialized_end=1198
  _AMPSTATUS._serialized_start=1200
  _AMPSTATUS._serialized_end=1255
  _SYSTEMSTATUS._serialized_start=1258
  _SYSTEMSTATUS._serialized_end=1571
  _SYSTEMSTATUS_ENUMSTATE._serialized_start=1383
  _SYSTEMSTATUS_ENUMSTATE._serialized_end=1454
  _SYSTEMSTATUS_ENUMDEV._serialized_start=1456
  _SYSTEMSTATUS_ENUMDEV._serialized_end=1571
# @@protoc_insertion_point(module_scope)
