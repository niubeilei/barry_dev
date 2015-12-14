////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MsgId.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Message/MsgId.h"

#include "Alarm/Alarm.h"
#include "Util/String.h"

/*
static char *sgNames[] = {
	"Minimum",
	"NoSubCat",
	"RawMsg",
	"SIP",
	"Mgcp",
	"NMS",
	"RTPStream",
	"Reg",
	"EPCF",
	"CRCX",
	"MDCX",
	"DLCX",
	"RQNT",
	"NTFY",
	"AUEP",
	"AUCX",
	"RSIP",
	"Maximum"
};
*/


/*
OmnString
OmnMsgId::toStr(OmnMsgId::E code)
{
	switch (code)
	{
	case eFirstValidEntry:
		 return "FirstValidEntry";

	case eInvalidMsgId:
		 return "InvalidMsgId";

	case eInvalidProt:
		 return "InvalidProt";

	case eUnknown:
		 return "Unknown";

	case eNoSubCat:
		 return "NoSubCat";

	case eAlarmEntry:
		 return "AlarmEntry";

	case eCheckedout:
		 return "Checkedout";

	case eEvent:
		 return "Event";

	case eResp:
		 return "Resp";

	case eRealmImage:
		 return "RealmImage";

	case eScvsEntry:
		 return "ScvsEntry";

	case eScvsBranch:
		 return "ScvsBranch";

	case eScvsPdeBase:
		 return "ScvsPdeBase";

	case eScvsLde:
		 return "ScvsLde";

	case eScvsPde:
		 return "ScvsTag";

	case eScvsTag:
		 return "ScvsTagEntry";

	case eScvsTagEntry:
		 return "ScvsView";

	case eScvsView:
		 return "ScvsView";

	case eScvsViewEntry:
		 return "ScvsViewEntry";

	case eScvsVm:
		 return "ScvsVm";

	case eScvsVmLoc: 
		 return "ScvsVmLoc";

	case eScvsCheckout:
		 return "ScvsCheckout";

	case eSmDnsReq:
		 return "SmDnsReq";

	case eSmDnsResp:
		 return "SmDnsResp";

	case eSmGetIds:
		 return "SmGetIds";

	case eSmRealmReq:
		 return "SmRealReq";

	case eSmRealmResp:
		 return "SmRealmResp";

	case eSmRealmStreamRecved:
		 return "SmRealmStreamRecved";

	case eSmSendStreamReq:
		 return "SmSendStreamReq";

	case eSmSendStreamResp:
		 return "SmSendStreamResp";

	case eSmStreamAck:
		 return "SmStreamAck";

	case eSmStreamInviteResp:
		 return "SmStreamInviteResp";

	case eSmStreamAckResp:
		 return "SmStreamAckResp";

	case eSmStreamInvite:
		 return "SmStreamInvite";

	case eSmStreamRecvAck:
		 return "SmStreamRecvAck";

	case eSmResumeStreamSend:
		 return "SmResumeStreamSend";

	case eSmResumeStreamSendResp:
		 return "SmResumeStreamSendResp";

	case eSmScvsReq:
		 return "SmScvsReq";

	case eSmScvsAdd:
		 return "SmScvsAdd";
		 
	case eSmScvsDelete:
		 return "SmScvsDelete";

	case eSmScvsUpdate:
		 return "SmScvsUpdate";

	case eSmScvsCheckin:
		 return "SmScvsCheckin";

	case eSmScvsCheckout:
		 return "SmScvsCheckout";

	case eSmScvsSvrCmd:
		 return "SmScvsSvrCmd";

	case eSmScvsCreateVm:
		 return "SmScvsCreateVm";

	case eSmScvsCreateBranch:
		 return "SmScvsCreateBranch";

	case eSmScvsCreatePdeBase:
		 return "SmScvsCreatePdeBase";

	case eSmScvsCreateLde:
		 return "SmScvsCreateLde";

	case eSmScvsCreateLdeResp:
		 return "SmScvsCreateLdeResp";

	case eSmScvsCreatePde:
		 return "SmScvsCreatePde";

	case eSmScvsCreateTag:
		 return "SmScvsCreateTag";

	case eSmScvsCreateTagEntry:
		 return "SmScvsCreateTagEntry";

	case eSmScvsCreateView:
		 return "SmScvsCreateView";

	case eSmScvsCreateViewEntry:
		 return "SmScvsCreateViewEntry";

	case eSmScvsResp:
		 return "SmScvsResp";

	case eSoAlarm:
		 return "SoAlarm";

	case eSoDataEntity:
		 return "SoDataEntry";

	case eSoEvent:
		 return "SoEvent";
		 
	case eSoMemMtr:
		 return "SoMemMtr";

	case eSoMsg:
		 return "SoMsg";

	case eSoWebAction:
		 return "SoWebAction";

	case eStreamLogEntry:
		 return "StreamLogEntry";

	case eRepDomain:
		 return "RepDomain";

	case eRepDomainImage:
		 return "RepDomainImage";

	case eSysMsg:
		 return "SysMsg";

	case eTestCase:
		 return "TestCase";

	case eLastValidEntry:
		 return "LastValidEntry";

	default:
		 return OmnString("Unrecognized: " ) << code;
	}

	return "Incorrect";
	OmnNotImplementedYet;
	return eInvalidMsgId;
}
	*/


/*
bool
OmnMsgId::isProtocol(E e) 
{
	return (e == eSip || e == eMgcp || e == eNms|| e == eRtp || 
			e == eReg || e == eDb);
}
*/


OmnMsgId::E
OmnMsgId::strToEnum(const OmnString &name)
{
	OmnAlarm << "Not implemented yet" << enderr;
	return eInvalidMsgId;
}

