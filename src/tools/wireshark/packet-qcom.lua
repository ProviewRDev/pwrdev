-- packet-qcom.lua
-- Lua dissector for ProviewR QCOM packets.
--
-- This is a standalone Lua port of the in-tree C dissector so it can be used
-- with stock Wireshark builds, e.g. the Debian package.

local HEADER_SIZE = 28
local INFO_SIZE = 56
local QCOM_MSG_SIZE = HEADER_SIZE + INFO_SIZE + 12

local NET_MESSAGE_SIZE = 12
local NET_OIDTOOBJECT_SIZE = 20
local NET_OBJECTR_SIZE = 16
local NET_GOBJECT_SIZE = 112
local NET_GETOBJECTINFO_SIZE = 24
local NET_GETOBJECTINFOR_SIZE = 32

local MH_HEADER_SIZE = 76
local MH_MSGINFO_SIZE = 140
local MH_MESSAGE_SIZE = 888
local MH_BLOCK_SIZE = 484
local MH_ACK_SIZE = 516
local MH_RETURN_SIZE = 584

local E_EVENT_CONNECT = 1
local E_EVENT_DOWN = 2
local E_EVENT_USER = 3
local E_EVENT_ACK = 4
local E_EVENT_CONNECT_PASSIVE = 5
local E_EVENT_REDCOM_ACTIVE = 6
local E_EVENT_REDCOM_PASSIVE = 7

local MSEG_FIRST = bit32 and bit32.lshift(1, 3) or 8
local MSEG_MIDDLE = bit32 and bit32.lshift(1, 4) or 16
local MSEG_LAST = bit32 and bit32.lshift(1, 5) or 32

local QCOM_C_ILOOPBACK = 0x80000000
local QCOM_C_INET_EVENT = 0x80000064
local QCOM_C_IAPPL_EVENT = 0x80000065
local QCOM_C_IMH_ALL_HANDLERS = 0x80000066
local QCOM_C_IMH_ALL_OUTUNITS = 0x80000067
local QCOM_C_IHD_SERVER = 0x80000068
local QCOM_C_IHD_CLIENT = 0x80000069
local QCOM_C_INACP = 0x8000006a
local QCOM_C_IINI = 0x8000006b

local QCOM_EBTYPE_SYSTEM = 1
local QCOM_EBTYPE_QCOM = 2
local QCOM_EBTYPE_EVENT = 3

local QCOM_ESTYPE_LINKCONNECT = 1
local QCOM_ESTYPE_LINKDISCONNECT = 2
local QCOM_ESTYPE_LINKACTIVE = 3
local QCOM_ESTYPE_LINKSTALLED = 4
local QCOM_ESTYPE_APPLCONNECT = 5
local QCOM_ESTYPE_APPLDISCONNECT = 6

local NET_CMSG_CLASS = 200
local MH_CMSG_CLASS = 201
local SEV_CMSG_CLASS = 202

local NET_EMSG = {
  error = 0,
  id = 1,
  idAck = 2,
  idAck2 = 3,
  volumes = 4,
  volumesR = 5,
  subAdd = 6,
  subRemove = 7,
  subData = 8,
  sanAdd = 9,
  sanRemove = 10,
  sanUpdate = 11,
  nameToObject = 12,
  oidToObject = 13,
  objectR = 14,
  getObjectInfo = 15,
  getObjectInfoR = 16,
  setObjectInfo = 17,
  setObjectInfoR = 18,
  flush = 19,
  createObject = 20,
  deleteObject = 21,
  moveObject = 22,
  renameObject = 23,
  nodeUp = 24,
  nodeDown = 25,
  getCclass = 26,
  getCclassR = 27,
  getGclass = 28,
  getGclassR = 29,
  serverConnect = 30,
  fileList = 31,
  fileListR = 32,
  getCircBuffer = 33,
  getCircBufferR = 34,
  updateCircBuffer = 35,
  updateCircBufferR = 36,
  volumes7 = 38,
}

local MH_EMSG = {
  ApplConnect = 1,
  ApplDisconnect = 2,
  ApplGetMsgInfo = 3,
  ApplMessage = 4,
  ApplReply = 5,
  ApplReturn = 6,
  Event = 8,
  HandlerDisconnect = 10,
  HandlerHello = 11,
  HandlerSync = 12,
  HandlerAlarmStatus = 13,
  OutunitAck = 15,
  OutunitBlock = 16,
  OutunitDisconnect = 18,
  OutunitHello = 19,
  OutunitInfo = 20,
  OutunitSync = 21,
  OutunitClear = 22,
  OutunitAlarmReq = 23,
  ProcDown = 24,
  StopScanSup = 25,
  StartScanSup = 26,
  Sync = 27,
}

local MH_EEVENT = {
  Ack = 1,
  Block = 2,
  Cancel = 3,
  CancelBlock = 4,
  Missing = 5,
  Reblock = 6,
  Return = 7,
  Unblock = 8,
  Info = 32,
  Alarm = 64,
}

local SEV_EMSG = {
  NodeUp = 0,
  HistItemsRequest = 1,
  HistItems = 2,
  HistDataStore = 3,
  HistDataGetRequest = 4,
  HistDataGet = 5,
  HistItemDelete = 6,
  HistItemStatus = 7,
  ServerStatusRequest = 8,
  ServerStatus = 9,
  HistObjectDataGetRequest = 10,
  HistObjectDataGet = 11,
}

local EVENT_NAMES = {
  [E_EVENT_CONNECT] = "Connect",
  [E_EVENT_DOWN] = "Down",
  [E_EVENT_USER] = "User",
  [E_EVENT_ACK] = "Ack",
  [E_EVENT_CONNECT_PASSIVE] = "ConnectPassive",
  [E_EVENT_REDCOM_ACTIVE] = "RedcomActive",
  [E_EVENT_REDCOM_PASSIVE] = "RedcomPassive",
}

local BTYPE_NAMES = {
  [QCOM_EBTYPE_SYSTEM] = "System",
  [QCOM_EBTYPE_QCOM] = "QCOM",
  [QCOM_EBTYPE_EVENT] = "Event",
  [NET_CMSG_CLASS] = "NET",
  [MH_CMSG_CLASS] = "MH",
  [SEV_CMSG_CLASS] = "SEV",
}

local QCOM_STYPE_NAMES = {
  [QCOM_ESTYPE_LINKCONNECT] = "LinkConnect",
  [QCOM_ESTYPE_LINKDISCONNECT] = "LinkDisconnect",
  [QCOM_ESTYPE_LINKACTIVE] = "LinkActive",
  [QCOM_ESTYPE_LINKSTALLED] = "LinkStalled",
  [QCOM_ESTYPE_APPLCONNECT] = "ApplConnect",
  [QCOM_ESTYPE_APPLDISCONNECT] = "ApplDisconnect",
}

local EVENT_STYPE_NAMES = {
  [QCOM_C_ILOOPBACK] = "LoopBack",
  [QCOM_C_INET_EVENT] = "NetEvent",
  [QCOM_C_IAPPL_EVENT] = "ApplEvent",
  [QCOM_C_IMH_ALL_HANDLERS] = "MhAllHandlers",
  [QCOM_C_IMH_ALL_OUTUNITS] = "MhAllOutunits",
  [QCOM_C_IHD_SERVER] = "HdServer",
  [QCOM_C_IHD_CLIENT] = "HdClient",
  [QCOM_C_INACP] = "Nacp",
  [QCOM_C_IINI] = "Ini",
}

local NET_MSG_NAMES = {}
for k, v in pairs(NET_EMSG) do
  NET_MSG_NAMES[v] = k
end

local MH_MSG_NAMES = {}
for k, v in pairs(MH_EMSG) do
  MH_MSG_NAMES[v] = k
end

local MH_EVENT_NAMES = {}
for k, v in pairs(MH_EEVENT) do
  MH_EVENT_NAMES[v] = k
end

local SEV_MSG_NAMES = {}
for k, v in pairs(SEV_EMSG) do
  SEV_MSG_NAMES[v] = k
end

local bitops = rawget(_G, "bit32") or rawget(_G, "bit")
if not bitops then
  local ok, mod = pcall(require, "bit")
  if ok then
    bitops = mod
  end
end
if not bitops then
  local ok, mod = pcall(require, "bit32")
  if ok then
    bitops = mod
  end
end
assert(bitops, "packet-qcom.lua requires bit or bit32")

local band = bitops.band
local rshift = bitops.rshift

local qcom_proto = Proto("qcom", "ProviewR QCOM")
local udp_table = DissectorTable.get("udp.port")

local field_defs = {
  {"header", "none", "Header"},
  {"header.nodeid", "uint32", "NodeId", base.HEX},
  {"header.birth", "int32", "Birth"},
  {"header.flags", "uint32", "Flags", base.HEX},
  {"header.lacksequence", "int32", "LackSequence"},
  {"header.lacktimestamp", "int32", "LackTimestamp"},
  {"header.racksequence", "int32", "RackSequence"},
  {"header.racktimestamp", "int32", "RackTimestamp"},

  {"info", "none", "Info"},
  {"info.senderaix", "int32", "SenderAix"},
  {"info.sendernid", "uint32", "SenderNid", base.HEX},
  {"info.pid", "uint32", "Pid"},
  {"info.receiverqix", "int32", "ReceiverQix"},
  {"info.receivernid", "uint32", "ReceiverNid", base.HEX},
  {"info.replyqix", "int32", "ReplyQix"},
  {"info.replynid", "uint32", "ReplyNid", base.HEX},
  {"info.btype", "int32", "BType", base.DEC, BTYPE_NAMES},
  {"info.stype", "int32", "SType"},
  {"info.rid", "int32", "Rid"},
  {"info.size", "uint32", "Size"},
  {"info.tmo", "int32", "Tmo"},
  {"info.status", "int32", "Status"},
  {"info.flags", "uint32", "Flags", base.HEX},

  {"net.message", "none", "NetMessage"},
  {"net.message.xdr", "int32", "Xdr"},
  {"net.message.msn", "int32", "Msn"},
  {"net.message.nid", "uint32", "Nid", base.HEX},
  {"net.oidtoobject", "none", "NetOidToObject"},
  {"net.oidtoobject.oidoix", "uint32", "OidOix"},
  {"net.oidtoobject.oidvid", "uint32", "OidVid", base.HEX},
  {"net.oidtoobject.trans", "int32", "Trans"},
  {"net.oidtoobject.lcount", "int32", "LCount"},
  {"net.oidtoobject.rcount", "int32", "RCount"},
  {"net.objectr", "none", "NetObjectR"},
  {"net.objectr.status", "int32", "Status"},
  {"net.objectr.oidoix", "uint32", "OidOix"},
  {"net.objectr.oidvid", "uint32", "OidVid", base.HEX},
  {"net.objectr.count", "int32", "Count"},
  {"net.gobject.oidoix", "uint32", "OidOix"},
  {"net.gobject.oidvid", "uint32", "OidVid", base.HEX},
  {"net.gobject.cid", "uint32", "Cid", base.HEX},
  {"net.gobject.familynameorig", "string", "FamilyNameOrig"},
  {"net.gobject.familynamenorm", "string", "FamilyNameNorm"},
  {"net.gobject.familynamekey", "uint32", "FamilyNameKey"},
  {"net.gobject.familypoidoix", "uint32", "FamilyPoidOix"},
  {"net.gobject.familypoidvid", "uint32", "FamilyPoidVid", base.HEX},
  {"net.gobject.sibflink", "int32", "SibFLink"},
  {"net.gobject.sibblink", "int32", "SibBLink"},
  {"net.gobject.soidoix", "uint32", "SOidOix"},
  {"net.gobject.soidvid", "uint32", "SOidVid", base.HEX},
  {"net.gobject.size", "uint32", "Size"},
  {"net.gobject.flags", "uint32", "Flags", base.HEX},
  {"net.getobjectinfo", "none", "NetGetObjectInfo"},
  {"net.getobjectinfo.arefoix", "uint32", "ArefOix"},
  {"net.getobjectinfo.arefvid", "uint32", "ArefVid", base.HEX},
  {"net.getobjectinfo.arefbody", "uint32", "ArefBody"},
  {"net.getobjectinfo.arefoffset", "uint32", "ArefOffset"},
  {"net.getobjectinfo.arefsize", "uint32", "ArefSize"},
  {"net.getobjectinfo.arefflags", "uint32", "ArefFlags", base.HEX},
  {"net.getobjectinfor", "none", "NetGetObjectInfoR"},
  {"net.getobjectinfor.sts", "int32", "Status"},
  {"net.getobjectinfor.arefoix", "uint32", "ArefOix"},
  {"net.getobjectinfor.arefvid", "uint32", "ArefVid", base.HEX},
  {"net.getobjectinfor.arefbody", "uint32", "ArefBody"},
  {"net.getobjectinfor.arefoffset", "uint32", "ArefOffset"},
  {"net.getobjectinfor.arefsize", "uint32", "ArefSize"},
  {"net.getobjectinfor.arefflags", "uint32", "ArefFlags", base.HEX},
  {"net.getobjectinfor.size", "uint32", "Size"},

  {"mh.header", "none", "MhHeader"},
  {"mh.header.xdr", "int32", "Xdr"},
  {"mh.header.platform", "int32", "Platform"},
  {"mh.header.version", "int32", "Version"},
  {"mh.header.source", "int32", "Source"},
  {"mh.header.birthtime", "uint64", "BirthTime"},
  {"mh.header.type", "int32", "Type", base.DEC, MH_MSG_NAMES},
  {"mh.header.qidqix", "int32", "QidQix"},
  {"mh.header.qidnid", "uint32", "QidNid", base.HEX},
  {"mh.header.nix", "uint32", "Nix", base.HEX},
  {"mh.header.outunitoix", "uint32", "OutunitOix"},
  {"mh.header.outunitvid", "uint32", "OutunitVid", base.HEX},
  {"mh.header.aidoix", "uint32", "AidOix"},
  {"mh.header.aidvid", "uint32", "AidVid", base.HEX},

  {"mh.msginfo", "none", "MhMsgInfo"},
  {"mh.msginfo.idnix", "uint32", "IdNix", base.HEX},
  {"mh.msginfo.idbirthtime", "uint64", "IdBirthTime"},
  {"mh.msginfo.ididx", "int32", "IdIdx"},
  {"mh.msginfo.outunitoix", "uint32", "OutunitOix"},
  {"mh.msginfo.outunitvid", "uint32", "OutunitVid", base.HEX},
  {"mh.msginfo.eventflags", "uint32", "EventFlags", base.HEX},
  {"mh.msginfo.eventtime", "uint64", "EventTime"},
  {"mh.msginfo.eventnamev3", "string", "EventNameV3"},
  {"mh.msginfo.eventtype", "int32", "EventType", base.DEC, MH_EVENT_NAMES},
  {"mh.msginfo.eventprio", "int32", "EventPrio"},

  {"mh.message", "none", "MhMessage"},
  {"mh.message.eventtext", "string", "EventText"},
  {"mh.message.status", "int32", "Status"},
  {"mh.message.eventsoundoix", "uint32", "EventSoundOix"},
  {"mh.message.eventsoundvid", "uint32", "EventSoundVid", base.HEX},
  {"mh.message.eventsoundbody", "uint32", "EventSoundBody"},
  {"mh.message.eventsoundoffset", "uint32", "EventSoundOffset"},
  {"mh.message.eventsoundsize", "uint32", "EventSoundSize"},
  {"mh.message.eventsoundflags", "uint32", "EventSoundFlags", base.HEX},
  {"mh.message.eventmoretext", "string", "EventMoreText"},
  {"mh.message.objectoix", "uint32", "ObjectOix"},
  {"mh.message.objectvid", "uint32", "ObjectVid", base.HEX},
  {"mh.message.objectbody", "uint32", "ObjectBody"},
  {"mh.message.objectoffset", "uint32", "ObjectOffset"},
  {"mh.message.objectsize", "uint32", "ObjectSize"},
  {"mh.message.objectflags", "uint32", "ObjectFlags", base.HEX},
  {"mh.message.supobjectoix", "uint32", "SupObjectOix"},
  {"mh.message.supobjectvid", "uint32", "SupObjectVid", base.HEX},
  {"mh.message.supobjectbody", "uint32", "SupObjectBody"},
  {"mh.message.supobjectoffset", "uint32", "SupObjectOffset"},
  {"mh.message.supobjectsize", "uint32", "SupObjectSize"},
  {"mh.message.supobjectflags", "uint32", "SupObjectFlags", base.HEX},
  {"mh.message.eventname", "string", "EventName"},
  {"mh.message.receiver", "string", "Receiver"},

  {"mh.block", "none", "MhBlock"},
  {"mh.block.targetidnix", "uint32", "TargetIdNix", base.HEX},
  {"mh.block.targetidbirthtime", "uint64", "TargetIdBirthTime"},
  {"mh.block.targetididx", "int32", "TargetIdIdx"},
  {"mh.block.detecttime", "uint64", "DetectTime"},
  {"mh.block.outunitoix", "uint32", "OutunitOix"},
  {"mh.block.outunitvid", "uint32", "OutunitVid", base.HEX},
  {"mh.block.status", "int32", "Status"},
  {"mh.block.objectoix", "uint32", "ObjectOix"},
  {"mh.block.objectvid", "uint32", "ObjectVid", base.HEX},
  {"mh.block.objectbody", "uint32", "ObjectBody"},
  {"mh.block.objectoffset", "uint32", "ObjectOffset"},
  {"mh.block.objectsize", "uint32", "ObjectSize"},
  {"mh.block.objectflags", "uint32", "ObjectFlags", base.HEX},
  {"mh.block.supobjectoix", "uint32", "SupObjectOix"},
  {"mh.block.supobjectvid", "uint32", "SupObjectVid", base.HEX},
  {"mh.block.supobjectbody", "uint32", "SupObjectBody"},
  {"mh.block.supobjectoffset", "uint32", "SupObjectOffset"},
  {"mh.block.supobjectsize", "uint32", "SupObjectSize"},
  {"mh.block.supobjectflags", "uint32", "SupObjectFlags", base.HEX},
  {"mh.block.eventname", "string", "EventName"},

  {"mh.ack", "none", "MhAck"},
  {"mh.ack.targetidnix", "uint32", "TargetIdNix", base.HEX},
  {"mh.ack.targetidbirthtime", "uint64", "TargetIdBirthTime"},
  {"mh.ack.targetididx", "int32", "TargetIdIdx"},
  {"mh.ack.detecttime", "uint64", "DetectTime"},
  {"mh.ack.outunitoix", "uint32", "OutunitOix"},
  {"mh.ack.outunitvid", "uint32", "OutunitVid", base.HEX},
  {"mh.ack.objectoix", "uint32", "ObjectOix"},
  {"mh.ack.objectvid", "uint32", "ObjectVid", base.HEX},
  {"mh.ack.objectbody", "uint32", "ObjectBody"},
  {"mh.ack.objectoffset", "uint32", "ObjectOffset"},
  {"mh.ack.objectsize", "uint32", "ObjectSize"},
  {"mh.ack.objectflags", "uint32", "ObjectFlags", base.HEX},
  {"mh.ack.supobjectoix", "uint32", "SupObjectOix"},
  {"mh.ack.supobjectvid", "uint32", "SupObjectVid", base.HEX},
  {"mh.ack.supobjectbody", "uint32", "SupObjectBody"},
  {"mh.ack.supobjectoffset", "uint32", "SupObjectOffset"},
  {"mh.ack.supobjectsize", "uint32", "SupObjectSize"},
  {"mh.ack.supobjectflags", "uint32", "SupObjectFlags", base.HEX},
  {"mh.ack.eventname", "string", "EventName"},

  {"mh.return", "none", "MhReturn"},
  {"mh.return.eventtext", "string", "EventText"},
  {"mh.return.targetidnix", "uint32", "TargetIdNix", base.HEX},
  {"mh.return.targetidbirthtime", "uint64", "TargetIdBirthTime"},
  {"mh.return.targetididx", "int32", "TargetIdIdx"},
  {"mh.return.detecttime", "uint64", "DetectTime"},
  {"mh.return.objectoix", "uint32", "ObjectOix"},
  {"mh.return.objectvid", "uint32", "ObjectVid", base.HEX},
  {"mh.return.objectbody", "uint32", "ObjectBody"},
  {"mh.return.objectoffset", "uint32", "ObjectOffset"},
  {"mh.return.objectsize", "uint32", "ObjectSize"},
  {"mh.return.objectflags", "uint32", "ObjectFlags", base.HEX},
  {"mh.return.supobjectoix", "uint32", "SupObjectOix"},
  {"mh.return.supobjectvid", "uint32", "SupObjectVid", base.HEX},
  {"mh.return.supobjectbody", "uint32", "SupObjectBody"},
  {"mh.return.supobjectoffset", "uint32", "SupObjectOffset"},
  {"mh.return.supobjectsize", "uint32", "SupObjectSize"},
  {"mh.return.supobjectflags", "uint32", "SupObjectFlags", base.HEX},
  {"mh.return.eventname", "string", "EventName"},

  {"sev.message", "none", "SevMessage"},
  {"sev.message.type", "int32", "Type", base.DEC, SEV_MSG_NAMES},

  {"payload", "bytes", "Payload"},
}

local proto_fields = {}
local F = {}

local constructors = {
  none = function(abbrev, label)
    return ProtoField.none(abbrev, label)
  end,
  uint32 = function(abbrev, label, display, values)
    return ProtoField.uint32(abbrev, label, display or base.DEC, values)
  end,
  int32 = function(abbrev, label, display, values)
    return ProtoField.int32(abbrev, label, display or base.DEC, values)
  end,
  uint64 = function(abbrev, label, display, values)
    return ProtoField.uint64(abbrev, label, display or base.DEC, values)
  end,
  string = function(abbrev, label)
    return ProtoField.string(abbrev, label)
  end,
  bytes = function(abbrev, label)
    return ProtoField.bytes(abbrev, label)
  end,
}

for _, def in ipairs(field_defs) do
  local name = def[1]
  local kind = def[2]
  local label = def[3]
  local display = def[4]
  local values = def[5]
  local field = constructors[kind]("qcom." .. name, label, display, values)
  F[name] = field
  table.insert(proto_fields, field)
end

qcom_proto.fields = proto_fields

local qcom_header_spec = {
  {field = F["header.nodeid"], len = 4},
  {field = F["header.birth"], len = 4},
  {field = F["header.flags"], len = 4},
  {field = F["header.lacksequence"], len = 4},
  {field = F["header.lacktimestamp"], len = 4},
  {field = F["header.racksequence"], len = 4},
  {field = F["header.racktimestamp"], len = 4},
}

local qcom_info_spec = {
  {field = F["info.senderaix"], len = 4},
  {field = F["info.sendernid"], len = 4},
  {field = F["info.pid"], len = 4},
  {field = F["info.receiverqix"], len = 4},
  {field = F["info.receivernid"], len = 4},
  {field = F["info.replyqix"], len = 4},
  {field = F["info.replynid"], len = 4},
  {field = F["info.btype"], len = 4},
  {field = F["info.stype"], len = 4},
  {field = F["info.rid"], len = 4},
  {field = F["info.size"], len = 4},
  {field = F["info.tmo"], len = 4},
  {field = F["info.status"], len = 4},
  {field = F["info.flags"], len = 4},
}

local net_message_spec = {
  {field = F["net.message.xdr"], len = 4},
  {field = F["net.message.msn"], len = 4},
  {field = F["net.message.nid"], len = 4},
}

local net_oidtoobject_spec = {
  {field = F["net.oidtoobject.oidoix"], len = 4},
  {field = F["net.oidtoobject.oidvid"], len = 4},
  {field = F["net.oidtoobject.trans"], len = 4},
  {field = F["net.oidtoobject.lcount"], len = 4},
  {field = F["net.oidtoobject.rcount"], len = 4},
}

local net_objectr_header_spec = {
  {field = F["net.objectr.status"], len = 4},
  {field = F["net.objectr.oidoix"], len = 4},
  {field = F["net.objectr.oidvid"], len = 4},
  {field = F["net.objectr.count"], len = 4},
}

local net_gobject_spec = {
  {field = F["net.gobject.oidoix"], len = 4},
  {field = F["net.gobject.oidvid"], len = 4},
  {field = F["net.gobject.cid"], len = 4},
  {field = F["net.gobject.familynameorig"], len = 32, kind = "string"},
  {field = F["net.gobject.familynamenorm"], len = 32, kind = "string"},
  {field = F["net.gobject.familynamekey"], len = 4},
  {field = F["net.gobject.familypoidoix"], len = 4},
  {field = F["net.gobject.familypoidvid"], len = 4},
  {field = F["net.gobject.sibflink"], len = 4},
  {field = F["net.gobject.sibblink"], len = 4},
  {field = F["net.gobject.soidoix"], len = 4},
  {field = F["net.gobject.soidvid"], len = 4},
  {field = F["net.gobject.size"], len = 4},
  {field = F["net.gobject.flags"], len = 4},
}

local net_getobjectinfo_spec = {
  {field = F["net.getobjectinfo.arefoix"], len = 4},
  {field = F["net.getobjectinfo.arefvid"], len = 4},
  {field = F["net.getobjectinfo.arefbody"], len = 4},
  {field = F["net.getobjectinfo.arefoffset"], len = 4},
  {field = F["net.getobjectinfo.arefsize"], len = 4},
  {field = F["net.getobjectinfo.arefflags"], len = 4},
}

local net_getobjectinfor_spec = {
  {field = F["net.getobjectinfor.sts"], len = 4},
  {field = F["net.getobjectinfor.arefoix"], len = 4},
  {field = F["net.getobjectinfor.arefvid"], len = 4},
  {field = F["net.getobjectinfor.arefbody"], len = 4},
  {field = F["net.getobjectinfor.arefoffset"], len = 4},
  {field = F["net.getobjectinfor.arefsize"], len = 4},
  {field = F["net.getobjectinfor.arefflags"], len = 4},
  {field = F["net.getobjectinfor.size"], len = 4},
}

local mh_header_spec = {
  {field = F["mh.header.xdr"], len = 4},
  {field = F["mh.header.platform"], len = 4},
  {field = F["mh.header.version"], len = 4},
  {field = F["mh.header.source"], len = 4},
  {field = F["mh.header.birthtime"], len = 8},
  {field = F["mh.header.type"], len = 4},
  {field = F["mh.header.qidqix"], len = 4},
  {field = F["mh.header.qidnid"], len = 4},
  {field = F["mh.header.nix"], len = 4},
  {field = F["mh.header.outunitoix"], len = 4},
  {field = F["mh.header.outunitvid"], len = 4},
  {field = F["mh.header.aidoix"], len = 4},
  {field = F["mh.header.aidvid"], len = 4},
  {skip = 20},
}

local mh_msginfo_spec = {
  {field = F["mh.msginfo.idnix"], len = 4},
  {field = F["mh.msginfo.idbirthtime"], len = 8},
  {field = F["mh.msginfo.ididx"], len = 4},
  {skip = 8},
  {field = F["mh.msginfo.outunitoix"], len = 4},
  {field = F["mh.msginfo.outunitvid"], len = 4},
  {skip = 8},
  {field = F["mh.msginfo.eventflags"], len = 4},
  {field = F["mh.msginfo.eventtime"], len = 8},
  {field = F["mh.msginfo.eventnamev3"], len = 80, kind = "string"},
  {field = F["mh.msginfo.eventtype"], len = 4},
  {field = F["mh.msginfo.eventprio"], len = 4},
}

local mh_message_spec = {
  {field = F["mh.message.eventtext"], len = 80, kind = "string"},
  {field = F["mh.message.status"], len = 4},
  {skip = 36},
  {field = F["mh.message.eventsoundoix"], len = 4},
  {field = F["mh.message.eventsoundvid"], len = 4},
  {field = F["mh.message.eventsoundbody"], len = 4},
  {field = F["mh.message.eventsoundoffset"], len = 4},
  {field = F["mh.message.eventsoundsize"], len = 4},
  {field = F["mh.message.eventsoundflags"], len = 4},
  {field = F["mh.message.eventmoretext"], len = 256, kind = "string"},
  {field = F["mh.message.objectoix"], len = 4},
  {field = F["mh.message.objectvid"], len = 4},
  {field = F["mh.message.objectbody"], len = 4},
  {field = F["mh.message.objectoffset"], len = 4},
  {field = F["mh.message.objectsize"], len = 4},
  {field = F["mh.message.objectflags"], len = 4},
  {field = F["mh.message.supobjectoix"], len = 4},
  {field = F["mh.message.supobjectvid"], len = 4},
  {field = F["mh.message.supobjectbody"], len = 4},
  {field = F["mh.message.supobjectoffset"], len = 4},
  {field = F["mh.message.supobjectsize"], len = 4},
  {field = F["mh.message.supobjectflags"], len = 4},
  {field = F["mh.message.eventname"], len = 400, kind = "string"},
  {field = F["mh.message.receiver"], len = 40, kind = "string"},
}

local mh_block_spec = {
  {field = F["mh.block.targetidnix"], len = 4},
  {field = F["mh.block.targetidbirthtime"], len = 8},
  {field = F["mh.block.targetididx"], len = 4},
  {field = F["mh.block.detecttime"], len = 8},
  {field = F["mh.block.outunitoix"], len = 4},
  {field = F["mh.block.outunitvid"], len = 4},
  {field = F["mh.block.status"], len = 4},
  {field = F["mh.block.objectoix"], len = 4},
  {field = F["mh.block.objectvid"], len = 4},
  {field = F["mh.block.objectbody"], len = 4},
  {field = F["mh.block.objectoffset"], len = 4},
  {field = F["mh.block.objectsize"], len = 4},
  {field = F["mh.block.objectflags"], len = 4},
  {field = F["mh.block.supobjectoix"], len = 4},
  {field = F["mh.block.supobjectvid"], len = 4},
  {field = F["mh.block.supobjectbody"], len = 4},
  {field = F["mh.block.supobjectoffset"], len = 4},
  {field = F["mh.block.supobjectsize"], len = 4},
  {field = F["mh.block.supobjectflags"], len = 4},
  {field = F["mh.block.eventname"], len = 400, kind = "string"},
}

local mh_ack_spec = {
  {field = F["mh.ack.targetidnix"], len = 4},
  {field = F["mh.ack.targetidbirthtime"], len = 8},
  {field = F["mh.ack.targetididx"], len = 4},
  {field = F["mh.ack.detecttime"], len = 8},
  {field = F["mh.ack.outunitoix"], len = 4},
  {field = F["mh.ack.outunitvid"], len = 4},
  {skip = 36},
  {field = F["mh.ack.objectoix"], len = 4},
  {field = F["mh.ack.objectvid"], len = 4},
  {field = F["mh.ack.objectbody"], len = 4},
  {field = F["mh.ack.objectoffset"], len = 4},
  {field = F["mh.ack.objectsize"], len = 4},
  {field = F["mh.ack.objectflags"], len = 4},
  {field = F["mh.ack.supobjectoix"], len = 4},
  {field = F["mh.ack.supobjectvid"], len = 4},
  {field = F["mh.ack.supobjectbody"], len = 4},
  {field = F["mh.ack.supobjectoffset"], len = 4},
  {field = F["mh.ack.supobjectsize"], len = 4},
  {field = F["mh.ack.supobjectflags"], len = 4},
  {field = F["mh.ack.eventname"], len = 400, kind = "string"},
}

local mh_return_spec = {
  {field = F["mh.return.eventtext"], len = 80, kind = "string"},
  {field = F["mh.return.targetidnix"], len = 4},
  {field = F["mh.return.targetidbirthtime"], len = 8},
  {field = F["mh.return.targetididx"], len = 4},
  {field = F["mh.return.detecttime"], len = 8},
  {skip = 36},
  {field = F["mh.return.objectoix"], len = 4},
  {field = F["mh.return.objectvid"], len = 4},
  {field = F["mh.return.objectbody"], len = 4},
  {field = F["mh.return.objectoffset"], len = 4},
  {field = F["mh.return.objectsize"], len = 4},
  {field = F["mh.return.objectflags"], len = 4},
  {field = F["mh.return.supobjectoix"], len = 4},
  {field = F["mh.return.supobjectvid"], len = 4},
  {field = F["mh.return.supobjectbody"], len = 4},
  {field = F["mh.return.supobjectoffset"], len = 4},
  {field = F["mh.return.supobjectsize"], len = 4},
  {field = F["mh.return.supobjectflags"], len = 4},
  {field = F["mh.return.eventname"], len = 400, kind = "string"},
}

local function default_qcom_port()
  local bus_id = tonumber(os.getenv("PWR_BUS_ID") or "")
  if not bus_id then
    bus_id = 999
  end
  return bus_id + 55000
end

qcom_proto.prefs.port = Pref.uint(
  "UDP port",
  default_qcom_port(),
  "QCOM UDP port (defaults to 55000 + PWR_BUS_ID, or 55999)"
)

local current_port = qcom_proto.prefs.port

local function have_bytes(buf, offset, len)
  return offset >= 0 and len >= 0 and buf:len() >= offset + len
end

local function read_u32(buf, offset, little_endian)
  local range = buf(offset, 4)
  return little_endian and range:le_uint() or range:uint()
end

local function c_string(range)
  local s = range:string()
  local nul = string.find(s, "\0", 1, true)
  if nul then
    s = string.sub(s, 1, nul - 1)
  end
  return s
end

local function add_text(tree, buf, offset, text)
  local range
  if offset < buf:len() then
    range = buf(offset, 0)
  else
    range = buf(0, 0)
  end
  tree:add(range, text)
end

local function add_value_field(tree, field, buf, offset, len, little_endian)
  if not have_bytes(buf, offset, len) then
    return false
  end
  local range = buf(offset, len)
  if little_endian then
    tree:add_le(field, range)
  else
    tree:add(field, range)
  end
  return true
end

local function add_string_field(tree, field, buf, offset, len)
  if not have_bytes(buf, offset, len) then
    return false
  end
  local range = buf(offset, len)
  tree:add(field, range, c_string(range))
  return true
end

local function parse_spec(tree, buf, offset, spec, little_endian)
  for _, entry in ipairs(spec) do
    if entry.skip then
      offset = offset + entry.skip
    elseif entry.kind == "string" then
      if not add_string_field(tree, entry.field, buf, offset, entry.len) then
        return nil
      end
      offset = offset + entry.len
    else
      if not add_value_field(
          tree, entry.field, buf, offset, entry.len, little_endian) then
        return nil
      end
      offset = offset + entry.len
    end
  end
  return offset
end

local function oid_to_string(oix, vid)
  return string.format(
    "%u.%u.%u.%u:%u",
    rshift(vid, 24),
    band(rshift(vid, 16), 0xff),
    band(rshift(vid, 8), 0xff),
    band(vid, 0xff),
    oix
  )
end

local function aref_to_string(oix, vid, aref_offset, aref_size)
  return string.format(
    "%u.%u.%u.%u:%u[%u:%u]",
    rshift(vid, 24),
    band(rshift(vid, 16), 0xff),
    band(rshift(vid, 8), 0xff),
    band(vid, 0xff),
    oix,
    aref_offset,
    aref_size
  )
end

local function get_objectr_name(buf, offset, count, little_endian)
  local name = ""
  local prev_poid_oix = 0

  for i = 1, count do
    if not have_bytes(buf, offset, NET_GOBJECT_SIZE) then
      break
    end

    if i == 2 and prev_poid_oix == 0 then
      name = name .. ":"
    elseif i > 2 then
      name = name .. "-"
    end

    offset = offset + 12
    name = name .. c_string(buf(offset, 32))
    offset = offset + 68
    local poid_oix = read_u32(buf, offset, little_endian)
    offset = offset + 32

    if i ~= 1 and poid_oix ~= prev_poid_oix then
      break
    end
    prev_poid_oix = poid_oix
  end

  return name
end

local function classify_packet(buf)
  local ctx = {
    protocol = "QCOM",
    info = "Unknown basetype",
    is_user = false,
    is_net = false,
    is_netoidtoobject = false,
    is_netobjectr = false,
    is_netgetobjectinfo = false,
    is_netgetobjectinfor = false,
    is_mh = false,
    is_mhmsg = false,
    is_mhmessage = false,
    is_mhblock = false,
    is_mhack = false,
    is_mhreturn = false,
    is_sev = false,
    mh_little_endian = false,
    net_little_endian = true,
    mh_type = 0,
    mh_eventtype = 0,
    net_gobject_count = 0,
  }

  if not have_bytes(buf, 8, 4) then
    ctx.info = "Short packet"
    return ctx
  end

  local msg_flags = read_u32(buf, 8, false)
  ctx.msg_flags = msg_flags

  local event = rshift(msg_flags, 24)
  local event_name = EVENT_NAMES[event]
  if event_name then
    ctx.info = event_name
    ctx.is_user = (event == E_EVENT_USER)
  end

  if not ctx.is_user then
    return ctx
  end

  if band(msg_flags, MSEG_MIDDLE) ~= 0 and band(msg_flags, MSEG_FIRST) == 0 then
    ctx.info = "Middle segment"
    return ctx
  end
  if band(msg_flags, MSEG_LAST) ~= 0 and band(msg_flags, MSEG_FIRST) == 0 then
    ctx.info = "Last segment"
    return ctx
  end

  if not have_bytes(buf, HEADER_SIZE, INFO_SIZE) then
    ctx.info = "Short user packet"
    return ctx
  end

  local receiver_qix = read_u32(buf, HEADER_SIZE + 12, false)
  local reply_qix = read_u32(buf, HEADER_SIZE + 20, false)
  local btype = read_u32(buf, HEADER_SIZE + 28, false)
  local stype = read_u32(buf, HEADER_SIZE + 32, false)
  local msg_size = read_u32(buf, HEADER_SIZE + 40, false)

  if receiver_qix == 122 or reply_qix == 122 then
    btype = SEV_CMSG_CLASS
  end

  ctx.receiver_qix = receiver_qix
  ctx.reply_qix = reply_qix
  ctx.btype = btype
  ctx.stype = stype
  ctx.msg_size = msg_size

  if btype == QCOM_EBTYPE_SYSTEM then
    ctx.protocol = "QCOM-SYS"
    ctx.info = "System"
  elseif btype == QCOM_EBTYPE_QCOM then
    ctx.protocol = "QCOM"
    ctx.info = QCOM_STYPE_NAMES[stype] or "Unknown subtype"
  elseif btype == QCOM_EBTYPE_EVENT then
    ctx.protocol = "QCOM-EV"
    ctx.info = EVENT_STYPE_NAMES[stype] or "Unknown subtype"
  elseif btype == NET_CMSG_CLASS then
    ctx.protocol = "QCOM-Net"
    ctx.is_net = true

    local name = NET_MSG_NAMES[stype]
    if name then
      ctx.info = name
    else
      ctx.info = "Unknown net_eMsgType"
    end

    if stype == NET_EMSG.oidToObject
        and have_bytes(buf, HEADER_SIZE + INFO_SIZE + NET_MESSAGE_SIZE,
            NET_OIDTOOBJECT_SIZE) then
      ctx.is_netoidtoobject = true
      local offs = HEADER_SIZE + INFO_SIZE + NET_MESSAGE_SIZE
      local oid_oix = read_u32(buf, offs, true)
      local oid_vid = read_u32(buf, offs + 4, true)
      ctx.info = string.format("OidToObject     oid  %s",
          oid_to_string(oid_oix, oid_vid))
    elseif stype == NET_EMSG.objectR
        and have_bytes(buf, HEADER_SIZE + INFO_SIZE + NET_MESSAGE_SIZE,
            NET_OBJECTR_SIZE) then
      ctx.is_netobjectr = true
      local offs = HEADER_SIZE + INFO_SIZE + NET_MESSAGE_SIZE + 12
      ctx.net_gobject_count = read_u32(buf, offs, true)
      local name_text = get_objectr_name(
          buf, offs + 4, ctx.net_gobject_count, true)
      if name_text ~= "" then
        ctx.info = "ObjectR         name " .. name_text
      else
        ctx.info = "ObjectR"
      end
    elseif stype == NET_EMSG.getObjectInfo
        and have_bytes(buf, HEADER_SIZE + INFO_SIZE + NET_MESSAGE_SIZE,
            NET_GETOBJECTINFO_SIZE) then
      ctx.is_netgetobjectinfo = true
      local offs = HEADER_SIZE + INFO_SIZE + NET_MESSAGE_SIZE
      local aref_oix = read_u32(buf, offs, true)
      local aref_vid = read_u32(buf, offs + 4, true)
      local aref_offset = read_u32(buf, offs + 12, true)
      local aref_size = read_u32(buf, offs + 16, true)
      ctx.info = string.format("GetObjectInfo   aref %s",
          aref_to_string(aref_oix, aref_vid, aref_offset, aref_size))
    elseif stype == NET_EMSG.getObjectInfoR
        and have_bytes(buf, HEADER_SIZE + INFO_SIZE + NET_MESSAGE_SIZE + 4,
            NET_GETOBJECTINFO_SIZE) then
      ctx.is_netgetobjectinfor = true
      local offs = HEADER_SIZE + INFO_SIZE + NET_MESSAGE_SIZE + 4
      local aref_oix = read_u32(buf, offs, true)
      local aref_vid = read_u32(buf, offs + 4, true)
      local aref_offset = read_u32(buf, offs + 12, true)
      local aref_size = read_u32(buf, offs + 16, true)
      ctx.info = string.format("GetObjectInfoR  aref %s",
          aref_to_string(aref_oix, aref_vid, aref_offset, aref_size))
    end
  elseif btype == MH_CMSG_CLASS then
    ctx.protocol = "QCOM-Mh"
    ctx.is_mh = (msg_size >= 28)

    if not ctx.is_mh or not have_bytes(buf, QCOM_MSG_SIZE + 24, 4) then
      ctx.info = "Unknown sub type"
      return ctx
    end

    local mh_type = read_u32(buf, QCOM_MSG_SIZE + 24, false)
    if mh_type > 255 then
      mh_type = read_u32(buf, QCOM_MSG_SIZE + 24, true)
      ctx.mh_little_endian = true
    end
    ctx.mh_type = mh_type

    local little = ctx.mh_little_endian
    local function mh_u32(offset)
      return read_u32(buf, offset, little)
    end

    if mh_type == MH_EMSG.Event then
      ctx.is_mhmsg = true
      if have_bytes(buf, QCOM_MSG_SIZE + MH_HEADER_SIZE + 132, 4) then
        ctx.mh_eventtype = mh_u32(QCOM_MSG_SIZE + MH_HEADER_SIZE + 132)
      end

      if ctx.mh_eventtype == MH_EEVENT.Ack then
        ctx.is_mhack = true
        local mh_id = mh_u32(
            QCOM_MSG_SIZE + MH_HEADER_SIZE + MH_MSGINFO_SIZE + 12)
        ctx.info = string.format("Event Ack       id   %-5d", mh_id)
      elseif ctx.mh_eventtype == MH_EEVENT.Block
          or ctx.mh_eventtype == MH_EEVENT.CancelBlock
          or ctx.mh_eventtype == MH_EEVENT.Reblock
          or ctx.mh_eventtype == MH_EEVENT.Unblock then
        ctx.is_mhblock = true
        local prefix = "Event Block     "
        if ctx.mh_eventtype == MH_EEVENT.CancelBlock
            or ctx.mh_eventtype == MH_EEVENT.Reblock then
          prefix = "Event CancelBlo "
        elseif ctx.mh_eventtype == MH_EEVENT.Unblock then
          prefix = "Event Unblock   "
        end
        local offs = QCOM_MSG_SIZE + MH_HEADER_SIZE + MH_MSGINFO_SIZE + 36
        if have_bytes(buf, offs, 8) then
          local oid_oix = mh_u32(offs)
          local oid_vid = mh_u32(offs + 4)
          ctx.info = prefix .. "oid  " .. oid_to_string(oid_oix, oid_vid)
        else
          ctx.info = string.sub(prefix, 1, #prefix - 1)
        end
      elseif ctx.mh_eventtype == MH_EEVENT.Missing then
        ctx.info = "Event Missing"
      elseif ctx.mh_eventtype == MH_EEVENT.Cancel
          or ctx.mh_eventtype == MH_EEVENT.Return then
        ctx.is_mhreturn = true
        local prefix = ctx.mh_eventtype == MH_EEVENT.Cancel
            and "Event Cancel    " or "Event Return    "
        local mh_id = mh_u32(
            QCOM_MSG_SIZE + MH_HEADER_SIZE + MH_MSGINFO_SIZE + 92)
        ctx.info = string.format("%sid   %-5d", prefix, mh_id)
      elseif ctx.mh_eventtype == MH_EEVENT.Info
          or ctx.mh_eventtype == MH_EEVENT.Alarm then
        ctx.is_mhmessage = true
        local prefix = ctx.mh_eventtype == MH_EEVENT.Info
            and "Event Info      " or "Event Alarm     "
        local mh_id = mh_u32(QCOM_MSG_SIZE + MH_HEADER_SIZE + 12)
        local event_text = ""
        local text_offs = QCOM_MSG_SIZE + MH_HEADER_SIZE + MH_MSGINFO_SIZE
        if have_bytes(buf, text_offs, 80) then
          event_text = c_string(buf(text_offs, 80))
        end
        ctx.info = string.format('%sid   %-5d  "%s"', prefix, mh_id, event_text)
      else
        ctx.info = "Unknown Mh EventType"
      end
    else
      ctx.info = MH_MSG_NAMES[mh_type] or "Unknown mh_eMsg"
    end
  elseif btype == SEV_CMSG_CLASS then
    ctx.protocol = "QCOM-Sev"
    ctx.is_sev = true
    if msg_size >= 4 and have_bytes(buf, HEADER_SIZE + INFO_SIZE, 4) then
      local sev_type = read_u32(buf, HEADER_SIZE + INFO_SIZE, false)
      ctx.sev_type = sev_type
      ctx.info = SEV_MSG_NAMES[sev_type] or "Unknown sev_eMsgType"
    end
  end

  if band(msg_flags, MSEG_FIRST) ~= 0 and band(msg_flags, MSEG_LAST) == 0 then
    ctx.info = ctx.info .. " First segment"
  end

  return ctx
end

local function register_qcom_port(new_port)
  if current_port then
    pcall(function()
      udp_table:remove(current_port, qcom_proto)
    end)
  end
  current_port = new_port
  udp_table:add(current_port, qcom_proto)
end

function qcom_proto.init()
end

function qcom_proto.prefs_changed()
  local new_port = qcom_proto.prefs.port
  if new_port ~= current_port then
    register_qcom_port(new_port)
  end
end

function qcom_proto.dissector(buf, pinfo, tree)
  local ctx = classify_packet(buf)

  pinfo.cols.protocol = ctx.protocol
  pinfo.cols.info = ctx.info

  local root = tree:add(qcom_proto, buf())
  root:append_text(" (" .. ctx.info .. ")")

  local header_tree = root:add(F["header"], buf(0, math.min(HEADER_SIZE, buf:len())))
  local offset = parse_spec(header_tree, buf, 0, qcom_header_spec, false) or HEADER_SIZE

  if not ctx.is_user then
    if offset < buf:len() then
      root:add(F["payload"], buf(offset))
    end
    return buf:len()
  end

  if have_bytes(buf, HEADER_SIZE, INFO_SIZE) then
    local info_tree = root:add(F["info"], buf(HEADER_SIZE, INFO_SIZE))
    offset = parse_spec(info_tree, buf, HEADER_SIZE, qcom_info_spec, false)
        or (HEADER_SIZE + INFO_SIZE)
  end

  if ctx.is_net and have_bytes(buf, offset, NET_MESSAGE_SIZE) then
    local net_message_tree = root:add(F["net.message"], buf(offset, NET_MESSAGE_SIZE))
    offset = parse_spec(net_message_tree, buf, offset, net_message_spec, true)
        or (offset + NET_MESSAGE_SIZE)

    if ctx.is_netoidtoobject and have_bytes(buf, offset, NET_OIDTOOBJECT_SIZE) then
      local section = root:add(F["net.oidtoobject"], buf(offset, NET_OIDTOOBJECT_SIZE))
      local start = offset
      offset = parse_spec(section, buf, offset, net_oidtoobject_spec, true)
          or (start + NET_OIDTOOBJECT_SIZE)
      local oid_oix = read_u32(buf, start, true)
      local oid_vid = read_u32(buf, start + 4, true)
      add_text(section, buf, offset, "OID: " .. oid_to_string(oid_oix, oid_vid))
    elseif ctx.is_netobjectr and have_bytes(buf, offset, NET_OBJECTR_SIZE) then
      local count = math.min(ctx.net_gobject_count or 0, 10)
      local expected_len = NET_OBJECTR_SIZE + count * NET_GOBJECT_SIZE
      local section = root:add(
          F["net.objectr"], buf(offset, math.min(expected_len, buf:len() - offset)))
      offset = parse_spec(section, buf, offset, net_objectr_header_spec, true)
          or (offset + NET_OBJECTR_SIZE)
      for _ = 1, count do
        if not have_bytes(buf, offset, NET_GOBJECT_SIZE) then
          break
        end
        local item = section:add(buf(offset, NET_GOBJECT_SIZE), "GObject")
        offset = parse_spec(item, buf, offset, net_gobject_spec, true)
            or (offset + NET_GOBJECT_SIZE)
      end
    elseif ctx.is_netgetobjectinfo and have_bytes(buf, offset, NET_GETOBJECTINFO_SIZE) then
      local section = root:add(F["net.getobjectinfo"], buf(offset, NET_GETOBJECTINFO_SIZE))
      local start = offset
      offset = parse_spec(section, buf, offset, net_getobjectinfo_spec, true)
          or (start + NET_GETOBJECTINFO_SIZE)
      local aref_oix = read_u32(buf, start, true)
      local aref_vid = read_u32(buf, start + 4, true)
      local aref_offset = read_u32(buf, start + 12, true)
      local aref_size = read_u32(buf, start + 16, true)
      add_text(section, buf, offset,
          "Aref: " .. aref_to_string(aref_oix, aref_vid, aref_offset, aref_size))
    elseif ctx.is_netgetobjectinfor and have_bytes(buf, offset, NET_GETOBJECTINFOR_SIZE) then
      local section = root:add(F["net.getobjectinfor"], buf(offset, NET_GETOBJECTINFOR_SIZE))
      local start = offset
      offset = parse_spec(section, buf, offset, net_getobjectinfor_spec, true)
          or (start + NET_GETOBJECTINFOR_SIZE)
      local aref_oix = read_u32(buf, start + 4, true)
      local aref_vid = read_u32(buf, start + 8, true)
      local aref_offset = read_u32(buf, start + 16, true)
      local aref_size = read_u32(buf, start + 20, true)
      add_text(section, buf, offset,
          "Aref: " .. aref_to_string(aref_oix, aref_vid, aref_offset, aref_size))
    end
  elseif ctx.is_mh and have_bytes(buf, offset + 12, MH_HEADER_SIZE) then
    offset = offset + 12
    local little = ctx.mh_little_endian

    local mh_header_tree = root:add(F["mh.header"], buf(offset, MH_HEADER_SIZE))
    offset = parse_spec(mh_header_tree, buf, offset, mh_header_spec, little)
        or (offset + MH_HEADER_SIZE)

    if ctx.is_mhmsg and have_bytes(buf, offset, MH_MSGINFO_SIZE) then
      local mh_msginfo_tree = root:add(F["mh.msginfo"], buf(offset, MH_MSGINFO_SIZE))
      offset = parse_spec(mh_msginfo_tree, buf, offset, mh_msginfo_spec, little)
          or (offset + MH_MSGINFO_SIZE)

      if ctx.is_mhmessage and have_bytes(buf, offset, MH_MESSAGE_SIZE) then
        local section = root:add(F["mh.message"], buf(offset, MH_MESSAGE_SIZE))
        offset = parse_spec(section, buf, offset, mh_message_spec, little)
            or (offset + MH_MESSAGE_SIZE)
      elseif ctx.is_mhblock and have_bytes(buf, offset, MH_BLOCK_SIZE) then
        local section = root:add(F["mh.block"], buf(offset, MH_BLOCK_SIZE))
        offset = parse_spec(section, buf, offset, mh_block_spec, little)
            or (offset + MH_BLOCK_SIZE)
      elseif ctx.is_mhack and have_bytes(buf, offset, MH_ACK_SIZE) then
        local section = root:add(F["mh.ack"], buf(offset, MH_ACK_SIZE))
        offset = parse_spec(section, buf, offset, mh_ack_spec, little)
            or (offset + MH_ACK_SIZE)
      elseif ctx.is_mhreturn and have_bytes(buf, offset, MH_RETURN_SIZE) then
        local section = root:add(F["mh.return"], buf(offset, MH_RETURN_SIZE))
        offset = parse_spec(section, buf, offset, mh_return_spec, little)
            or (offset + MH_RETURN_SIZE)
      end
    end
  elseif ctx.is_sev and have_bytes(buf, offset, 4) then
    local section = root:add(F["sev.message"], buf(offset, 4))
    add_value_field(section, F["sev.message.type"], buf, offset, 4, false)
    offset = offset + 4
  end

  if offset < buf:len() then
    root:add(F["payload"], buf(offset))
  end

  return buf:len()
end

pcall(function()
  udp_table:add_for_decode_as(qcom_proto)
end)

register_qcom_port(current_port)
