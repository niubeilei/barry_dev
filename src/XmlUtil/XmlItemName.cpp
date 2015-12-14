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
//	An XML Item is expressed by this class as:
//		Tag
//		Tag Contents (a string)   
//
// Modification History:
// 2014/12/06 Moved from XmlParser
////////////////////////////////////////////////////////////////////////////
#include "XmlUtil/XmlItemName.h"

OmnString OmnXmlItemName::eAddr = "Addr";
OmnString OmnXmlItemName::eAddrs = "Addrs";
OmnString OmnXmlItemName::eAlarmMgr = "AlarmMgr";
OmnString OmnXmlItemName::eBackups = "Backups";
OmnString OmnXmlItemName::eClientAddr = "ClientAddr";
OmnString OmnXmlItemName::eCommGroupId = "CommGroupId";
OmnString OmnXmlItemName::eCommGroups = "CommGroups";
OmnString OmnXmlItemName::eComms = "Comm";
OmnString OmnXmlItemName::eCommType = "CommType";
OmnString OmnXmlItemName::eDefaultHbTimerSec = "DefaultHbTimerSec";
OmnString OmnXmlItemName::eDN = "DN";
OmnString OmnXmlItemName::eDnsConfig = "DnsConfig";
OmnString OmnXmlItemName::eGroupType = "GroupType";
OmnString OmnXmlItemName::eHbTimerSec = "HbTimerSec";
OmnString OmnXmlItemName::eIpAddr = "IpAddr";
OmnString OmnXmlItemName::eInfobusConfig = "InfobusConfig";
OmnString OmnXmlItemName::eLengthType = "LengthType";
OmnString OmnXmlItemName::eLocalNiid = "LocalNiid";
OmnString OmnXmlItemName::eLocalNumPorts = "LocalNumPorts";
OmnString OmnXmlItemName::eSignalPort = "SignalPort";
OmnString OmnXmlItemName::eName = "Name";
OmnString OmnXmlItemName::eNIID = "NIID";
OmnString OmnXmlItemName::ePort = "Port";
OmnString OmnXmlItemName::ePrimaryConn = "PrimaryConn";
OmnString OmnXmlItemName::eProtocol = "Protocol";
OmnString OmnXmlItemName::eProxySenderAddr = "ProxSenderAddr";
OmnString OmnXmlItemName::eRecvDefs = "RecvDefs";
OmnString OmnXmlItemName::eRemoteIpAddr = "RemoteIpAddr";
OmnString OmnXmlItemName::eRemoteNumPorts = "RemoteNumPorts";
OmnString OmnXmlItemName::eRemotePort = "RemotePort";
OmnString OmnXmlItemName::eRemotePorts = "RemotePorts";
OmnString OmnXmlItemName::eRetransMgrConfig = "RetransMgrConfig";
OmnString OmnXmlItemName::eSchedules = "Schedules";
OmnString OmnXmlItemName::eSecond = "Second";
OmnString OmnXmlItemName::eTargetId = "TargetId";
OmnString OmnXmlItemName::eTargetType = "TargetType";
OmnString OmnXmlItemName::eTcpAppConfig = "TcpAppConfig";
OmnString OmnXmlItemName::eThreadMgrConfig = "ThreadMgrConfig";
OmnString OmnXmlItemName::eTimerConfig = "TimerConfig";
OmnString OmnXmlItemName::eTimerDef = "TimerDef";
OmnString OmnXmlItemName::eTooManyReqThreshold = "TooManyReqThreshold";
OmnString OmnXmlItemName::eTracerConfig = "TracerConfig";
OmnString OmnXmlItemName::eTracerFileName = "TracerFileName";
OmnString OmnXmlItemName::eTraceComm = "TraceComm";
OmnString OmnXmlItemName::eTraceThread = "TraceThread";
OmnString OmnXmlItemName::eTraceCP = "TraceGP";
OmnString OmnXmlItemName::eTraceDb = "TraceDb";
OmnString OmnXmlItemName::eTraceTimer = "TraceTimer";
OmnString OmnXmlItemName::eTraceWrite = "TraceWrite";
OmnString OmnXmlItemName::eTraceRead = "TraceRead";
OmnString OmnXmlItemName::eTraceHB = "TraceHB";
OmnString OmnXmlItemName::eTraceHK = "TraceHK";
OmnString OmnXmlItemName::eTraceNM = "TraceNM";
OmnString OmnXmlItemName::eTraceRT = "TraceRT";
OmnString OmnXmlItemName::eTraceMO = "TraceMO";
OmnString OmnXmlItemName::eTraceUser = "TraceUser";
OmnString OmnXmlItemName::eTraceSysAlarm = "TraceSysAlarm";
OmnString OmnXmlItemName::eTriggerPoint = "TriggerPoint";
OmnString OmnXmlItemName::eUsec = "Usec";
OmnString OmnXmlItemName::eEntityType = "EntityType";
OmnString OmnXmlItemName::eEntityDesc = "EntityDesc";
OmnString OmnXmlItemName::eLocationDesc = "LocationDesc";
OmnString OmnXmlItemName::eLocationId = "LocationId";
OmnString OmnXmlItemName::eEntityId = "EntityId";
OmnString OmnXmlItemName::eDescription = "Description";
OmnString OmnXmlItemName::eSignalAddr = "SignalAddr";
OmnString OmnXmlItemName::eStreamAddr = "StreamAddr";
OmnString OmnXmlItemName::eStreamPort = "StreamPort";
OmnString OmnXmlItemName::eEntity = "Entity";
OmnString OmnXmlItemName::eNotifyEntities = "NotifyEntities";
OmnString OmnXmlItemName::eSelfId = "SelfId";
OmnString OmnXmlItemName::eCommGroupMgr = "CommGroupMgr";
OmnString OmnXmlItemName::eEntityDefs = "EntityDefs";
OmnString OmnXmlItemName::eBandwidth = "Bandwidth";
OmnString OmnXmlItemName::eInterfaceName = "InterfaceName";
OmnString OmnXmlItemName::eNetConfig = "NetConfig";
OmnString OmnXmlItemName::eNetInterfaces = "NetInterfaces";
OmnString OmnXmlItemName::eLogName = "LogName";
OmnString OmnXmlItemName::eLoggerConfig = "LoggerConfig";
OmnString OmnXmlItemName::eLogType = "LogType";
OmnString OmnXmlItemName::eAppend = "Append";
OmnString OmnXmlItemName::eFileName = "FileName";
OmnString OmnXmlItemName::eNmsServer = "NmsServer";
OmnString OmnXmlItemName::eBanner = "Banner";
OmnString OmnXmlItemName::eHouseKeep = "HouseKeep";
OmnString OmnXmlItemName::eInstId = "InstId";
OmnString OmnXmlItemName::eNmsConfig = "NmsConfig";
OmnString OmnXmlItemName::eFrequency = "Frequency";
OmnString OmnXmlItemName::eMachineId = "MachineId";
OmnString OmnXmlItemName::eStreamer = "Streamer";
OmnString OmnXmlItemName::eScvsCltConfig = "ScvsCltConfig";
OmnString OmnXmlItemName::eScvsSvrConfig = "ScvsSvrConfig";
OmnString OmnXmlItemName::eTransMgrConfig = "TransMgrConfig";
OmnString OmnXmlItemName::eLocalPort = "LocalPort";
OmnString OmnXmlItemName::eLocalAddr = "LocalAddr";
OmnString OmnXmlItemName::eUserOrg  = "UserOrg";
OmnString OmnXmlItemName::eUserSys	 = "UserSys";
OmnString OmnXmlItemName::eUserSN	 = "UserSN";
OmnString OmnXmlItemName::eSrcOrg	 = "SrcOrg";
OmnString OmnXmlItemName::eSrcSys	 = "SrcSys";
OmnString OmnXmlItemName::eSrc		 = "Src";
OmnString OmnXmlItemName::eOperation = "Operation";
OmnString OmnXmlItemName::ePMIData = "PMIData";
OmnString OmnXmlItemName::ePMIServerConfig = "PMIServerConfig";
OmnString OmnXmlItemName::ePMITcpServer = "PMITcpServer";
OmnString OmnXmlItemName::eCliClientConfig = "CliClientConfig";
OmnString OmnXmlItemName::eCmdFileName = "CmdFileName";
OmnString OmnXmlItemName::eModuleCliServer = "ModuleCliServer";
OmnString OmnXmlItemName::eCliTcpServer = "CliTcpServer";
OmnString OmnXmlItemName::eModuleName = "ModuleName";
OmnString OmnXmlItemName::eOCSPData = "OCSPData";
OmnString OmnXmlItemName::eOCSPServerConfig = "OCSPServerConfig";
OmnString OmnXmlItemName::eOCSPTcpServer = "OCSPTcpServer";
OmnString OmnXmlItemName::eSN = "SN";
OmnString OmnXmlItemName::eNo = "No";
OmnString OmnXmlItemName::eCRLData = "CRLData";
OmnString OmnXmlItemName::eCRLFilePos = "CRLFilePos";
OmnString OmnXmlItemName::eSNs= "SNs";
OmnString OmnXmlItemName::eAttackMgrConfig = "AttackMgrConfig";
OmnString OmnXmlItemName::eAMConn = "AMConn";
OmnString OmnXmlItemName::eModId= "ModId";
OmnString OmnXmlItemName::eModAddr = "ModAddr";
OmnString OmnXmlItemName::eModPort = "ModPort";
			
