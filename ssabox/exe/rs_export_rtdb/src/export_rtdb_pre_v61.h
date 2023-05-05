// For pre V6.1 versions
#ifndef pwr_cClass_Ssab_ExportRtdbServer

typedef pwr_tEnum pwr_tSsabDbServerConnection;

typedef enum {
  pwr_eSsabDbServerConnection_Down     = 0,
  pwr_eSsabDbServerConnection_Up       = 1,
  pwr_eSsabDbServerConnection_MsgError = 2,
  pwr_eSsabDbServerConnection_NoConfigFile = 3,
  pwr_eSsabDbServerConnection_InitFailed = 4,
} pwr_eSsabDbServerConnection;

#define pwr_cClass_Ssab_ExportRtdbServer 4194631944UL

typedef struct {
  pwr_tString80                       Description pwr_dAlignLW;
  pwr_tFloat32                        ScanTime pwr_dAlignW;
  pwr_tString80                       Topic pwr_dAlignW;
  pwr_tString80                       KafkaConfigFile pwr_dAlignW;
  pwr_tURL                            SchemaRegistryURL pwr_dAlignW;
  pwr_tSsabDbServerConnection         ServerConnection pwr_dAlignW;
  pwr_tUInt32                         SendCnt pwr_dAlignW;
} pwr_sClass_Ssab_ExportRtdbServer;
#endif

