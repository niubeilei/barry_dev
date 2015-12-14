////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//	It defines all XML item names. If we need an XML item, we should
//  define the item name in this class instead of using the name
//  directly. 
//   
//
// Modification History:
// 2014/12/06 Moved from XmlParser by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlUtil_XmlItemName_h
#define Aos_XmlUtil_XmlItemName_h


#include "Util/String.h"

class OmnXmlItemName
{
public:
	static OmnString	eAddr;
	static OmnString	eAddrs;
	static OmnString	eAlarmMgr;
	static OmnString 	eAMConn;
	static OmnString	eAppend;
	static OmnString	eAttackMgrConfig;
	static OmnString	eBackups;
	static OmnString	eBandwidth;
	static OmnString	eBanner;
	static OmnString	eCliClientConfig;
	static OmnString	eClientAddr;
	static OmnString	eCliTcpServer;
	static OmnString	eCmdFileName;
	static OmnString	eCommGroupId;
	static OmnString	eCommGroupMgr;
	static OmnString	eCommGroups;
	static OmnString	eComms;
	static OmnString	eCommType;
	static OmnString	eCRLData;
	static OmnString	eCRLFilePos;
	static OmnString	eDefaultFreq;
	static OmnString	eDefaultHbTimerSec;
	static OmnString	eDescription;
	static OmnString	eDN;
	static OmnString	eDnsConfig;
	static OmnString	eEntity;
	static OmnString	eEntityDefs;
	static OmnString	eEntityDesc;
	static OmnString	eEntityId;
	static OmnString	eEntityType;
	static OmnString	eFileMaxSize;
	static OmnString	eFileName;
	static OmnString	eFrequency;
	static OmnString	eGroupSize;
	static OmnString	eGroupType;
	static OmnString	eHbTimerSec;
	static OmnString	eHeartbeatConfig;
	static OmnString	eHouseKeep;
	static OmnString	eInstId;
	static OmnString	eIpAddr;
	static OmnString	eInfobusConfig;
	static OmnString	eInterfaceName;
	static OmnString	eLengthType;
	static OmnString	eLocalAddr;
	static OmnString	eLocalPort;
	static OmnString	eSignalAddr;
	static OmnString	eSignalPort;
	static OmnString	eStreamAddr;
	static OmnString	eStreamPort;
	static OmnString	eLocationDesc;
	static OmnString	eLocationId;
	static OmnString	eLocalNiid;
	static OmnString	eLocalNumPorts;
	static OmnString	eLoggerConfig;
	static OmnString	eLogName;
	static OmnString	eLogType;
	static OmnString	eMachineId;
	static OmnString	eModId;
	static OmnString	eModAddr;
	static OmnString	eModPort;
	static OmnString	eModuleCliServer;
	static OmnString	eModuleName;
	static OmnString	eName;
	static OmnString	eNetConfig;
	static OmnString	eNo;
	static OmnString	eNmsConfig;
	static OmnString	eNmsServer;
	static OmnString	eNotifyEntities;
	static OmnString	eNetInterfaces;
	static OmnString	eNIID;
	static OmnString	eOCSPData;
	static OmnString	eOCSPTcpServer;
	static OmnString	eOCSPServerConfig;
	static OmnString	eOperation;	 
	static OmnString	ePort;
	static OmnString	ePMIData;
	static OmnString	ePMITcpServer;
	static OmnString	ePMIServerConfig;
	static OmnString	ePrimaryConn;
	static OmnString	eProtocol;
	static OmnString	eProxySenderAddr;
	static OmnString	eRecvDefs;
	static OmnString	eRemoteIpAddr;
	static OmnString	eRemoteNumPorts;
	static OmnString	eRemotePort;
	static OmnString	eRemotePorts;
	static OmnString	eRetransMgrConfig;
	static OmnString	eSchedules;
	static OmnString	eScvsSvrConfig;
	static OmnString	eScvsCltConfig;
	static OmnString	eSecond;
	static OmnString	eSelfId;
	static OmnString	eSN;
	static OmnString	eSNs;
	static OmnString	eSrcOrg;
	static OmnString	eSrcSys;
	static OmnString	eSrc;
	static OmnString	eStreamer;
	static OmnString	eTargetId;
	static OmnString	eTargetType;
	static OmnString	eTcpAppConfig;
	static OmnString	eThreadMgrConfig;
	static OmnString	eTimerConfig;
	static OmnString	eTimerDef;
	static OmnString	eTooManyReqThreshold;
	static OmnString	eTracerConfig;
	static OmnString	eTracerFileName;
	static OmnString	eTraceComm;
	static OmnString	eTraceThread;
	static OmnString	eTraceMgcp;
	static OmnString	eTraceMR;
	static OmnString	eTraceCP;
	static OmnString	eTraceDb;
	static OmnString	eTraceTimer;
	static OmnString	eTraceWrite;
	static OmnString	eTraceRead;
	static OmnString	eTraceHB;
	static OmnString	eTraceHK;
	static OmnString	eTraceNM;
	static OmnString	eTraceRT;
	static OmnString	eTraceMO;
	static OmnString	eTraceUser;
	static OmnString	eTraceSysAlarm;
	static OmnString	eTransMgrConfig;
	static OmnString	eTriggerPoint;
	static OmnString	eUsec;
	static OmnString	eUserOrg;
	static OmnString	eUserSN;
	static OmnString	eUserSys;
	static OmnString	eWebSvr;

};

#endif


