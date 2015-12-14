////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 04/25/2011	Created by Ketty Guo 
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocReserveRes.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/RdataUtil.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/ReservedMgr.h"
#include "SEModules/LogMgr.h"
#include "SEModules/ObjMgr.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"

AosSdocReserveRes::AosSdocReserveRes(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_RESERVERES, AosSdocId::eReserveRes, flag),
mLock(OmnNew OmnMutex())
{
}

AosSdocReserveRes::~AosSdocReserveRes()
{
}


bool
AosSdocReserveRes::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	aos_assert_r(rdata, false);
	if(!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr cmd = AosRdataUtil::getCommand(rdata);
	if(!cmd)
	{
		rdata->setError() << "Missing Cmd!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString opr = cmd->getAttrStr(AOSTAG_RESERVE_OPR);
	
	if(opr == "reserve")	return reserve(sdoc, rdata);
	if(opr == "confirm")	return confirm(rdata, cmd);
	if(opr == "cancel")		return cancel(rdata, sdoc, cmd);

	rdata->setError() << "Unrecognized operation:" << opr;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
	
}

bool
AosSdocReserveRes::reserve(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	mLock->lock();
	u64 actualRes = sdoc->getAttrU64(AOSTAG_RES_ACTUAL, 0);
	u64	reservedRes = sdoc->getAttrU64(AOSTAG_RESERVED_NUM, 0);

	u64	thresholdResNum = sdoc->getAttrU64(AOSTAG_RES_THRESHOLD_NUM, 0);
	u64 availableRes = sdoc->getAttrU64(AOSTAG_RES_AVAILABLE, 0);
	if(availableRes == 0)
	{
		availableRes = actualRes - reservedRes;
	}
	if( availableRes <= thresholdResNum || availableRes == 0)
	{
		rdata->setError() << "there are no more resourse!";
		mLock->unlock();
		return true;
	}

	// modify the available Num of resource
	AosXmlTagPtr sdoc_clone = sdoc->clone(AosMemoryCheckerArgsBegin);
	sdoc_clone->setAttr(AOSTAG_RESERVED_NUM, reservedRes + 1);
	sdoc_clone->setAttr(AOSTAG_RES_AVAILABLE, availableRes - 1);
	mLock->unlock();
	AosDocClientObj::getDocClient()->modifyObj(rdata, sdoc_clone, "", false);

	// create a log for this reservation
	u32 siteid = rdata->getSiteid();
	u64 userid = rdata->getUserid();
	OmnString logObjid = sdoc->getAttrStr(AOSTAG_OBJID, "");
	logObjid << "_" <<  reservedRes << (int)OmnGetSecond();
	OmnString ctnrName = sdoc->getAttrStr(AOSTAG_PARENTC, "");
	if(ctnrName == "")		ctnrName = AOSTAG_CTNR_RES;		
	AosXmlTagPtr ctnr_xml = AosDocClientObj::getDocClient()->getDocByObjid(ctnrName, rdata);
	if(!ctnr_xml)
	{
		OmnString ctnrStr = "<ctnr ";
		ctnrStr << AOSTAG_OBJID <<"=\"" << ctnrName <<"\" "
			<< AOSTAG_OTYPE <<"=\"" <<  AOSOTYPE_CONTAINER <<"\" "
			<< AOSTAG_SUBTYPE << "=\"" << AOSSTYPE_CTNR_LOG << "\" "
			<< AOSTAG_CTNR_PUBLIC <<"=\"" << "true" <<"\" "
			<< AOSTAG_PUBLIC_DOC <<"=\"" << "true" <<"\"/>";
		
		u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
		OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
		ctnr_xml = AosDocClientObj::getDocClient()->createDocSafe1(
						rdata, ctnrStr, "", "", true, true,
						false, false, true, true);
		rdata->setUserid(userid);
		rdata->setAppname(appname);
		if (!ctnr_xml)
		{
			rdata->setError() << "Failure Create Ctnr!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	OmnString docstr = "<reservation ";
	docstr << AOSTAG_OBJID << "=\"" << logObjid << "\" "
		<< AOSTAG_RESERVED_NUMBER << "=\"" << reservedRes << "\" "
		<< AOSTAG_SITEID << "=\"" << siteid << "\" "
		<< AOSTAG_CREATOR << "=\"" << userid << "\" "
		<< AOSTAG_RES_STATUS << "=\"" << AOSTAG_RES_RESERVED<< "\" "
		<< AOSTAG_RES_TIMER << "=\"" << OmnGetTime(AosLocale::getDftLocale()) << "\" "
		<< AOSTAG_PARENTC << "=\"" << ctnrName<< "\" "
		<< AOSTAG_SDOCOBJID << "=\"" << sdoc->getAttrStr(AOSTAG_OBJID, "") << "\" " 
		<< " />";
	
	userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	AosXmlTagPtr log_created = AosDocClientObj::getDocClient()->createDocSafe1(
			rdata, docstr, "", "", true, false, false, false, false, false);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	if(!log_created)
	{
		rdata->setError() << "Failure Create the Reserved log!";	
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Start the timer
	OmnString config_time = sdoc->getAttrStr("cancel_time", "");
	if(config_time != "")
	{
		OmnString time_format = sdoc->getAttrStr("time_format", "");
		if(time_format == "")
		{
			rdata->setError() << "Missing cancel_time format !";	
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		int timerSec, timerUsec;
		bool rslt = getTime(config_time, time_format, timerSec, timerUsec);
		aos_assert_r(rslt, false);

		OmnTimerObjPtr thisptr(this, false);
		OmnString docid = log_created->getAttrStr(AOSTAG_DOCID, "");
		aos_assert_r(docid != "", false);
		int timerid = OmnTimer::getSelf()->startTimer(docid, timerSec, timerUsec, thisptr, 0);

		// Add timer
		AosReservedMgr::getSelf()->addTimer(timerid, atoll(docid.data()));
	}
	return true;
}


bool
AosSdocReserveRes::cancel(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &sdoc,
				const AosXmlTagPtr &cmd)
{
	if(!cmd)
	{
		rdata->setError() << "Missing Cmd!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString canCancel = sdoc->getAttrStr(AOSTAG_RES_CANCEL, "");
	if(canCancel == "false" || canCancel == "False")
	{
		rdata->setError() << "The reservation can't be cancel!";
		return true;
	}

	OmnString reserved_objid = cmd->getAttrStr(AOSTAG_RESERVED_OBJID, "");
	if(reserved_objid == "")
	{
		rdata->setError() << "Missing the reservation Objid!";
		return true;
	}
	
	u32 siteid = rdata->getSiteid();

	AosXmlTagPtr reserved_doc = AosDocClientObj::getDocClient()->getDocByObjid(reserved_objid, rdata);
	OmnString crt_status = reserved_doc->getAttrStr(AOSTAG_RES_STATUS, "");
	if(crt_status == "")
	{
		rdata->setError() << "this reservation miss status!";
		return true;
	}
	if(crt_status == AOSTAG_RES_CANCEL)
	{
		rdata->setError() << "this reservation has canceled yet!";
		return true;
	}
	if(crt_status == AOSTAG_RES_CONFIRM)
	{
		rdata->setError() << "this reservation has confirmd, can't cancel!";
		return true;
	}

	AosXmlTagPtr reserved_doc_clone = reserved_doc->clone(AosMemoryCheckerArgsBegin);
	reserved_doc_clone->setAttr(AOSTAG_RES_STATUS, AOSTAG_RES_CANCEL);
	
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	bool rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, reserved_doc_clone, "", false);
	rdata->setUserid(userid);
	rdata->setAppname(appname);
	if(!rslt)
	{
		rdata->setError() << "Internal error!";
		return false;
	}

	mLock->lock();
	u64	reservedRes = sdoc->getAttrU64(AOSTAG_RESERVED_NUM, 0);
	u64 availableRes = sdoc->getAttrU64(AOSTAG_RES_AVAILABLE, 0);
	aos_assert_rl(reservedRes != 0, mLock, false)
	AosXmlTagPtr sdoc_clone = sdoc->clone(AosMemoryCheckerArgsBegin);
	sdoc_clone->setAttr(AOSTAG_RESERVED_NUM, reservedRes - 1);
	sdoc_clone->setAttr(AOSTAG_RES_AVAILABLE, availableRes + 1);
	mLock->unlock();
	
	userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, sdoc_clone, "", false);
	rdata->setAppname(appname);
	rdata->setUserid(userid);
	if(!rslt)
	{
		rdata->setError() << "Internal error!";
		return false;
	}
	
	// Cancel timer
	u64 reserved_docid = reserved_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(reserved_docid != 0, false);
	AosReservedMgr::getSelf()->cancelTimer(reserved_docid);
	return true;
}


bool
AosSdocReserveRes::confirm(const AosRundataPtr &rdata,
			const AosXmlTagPtr &cmd)
{
	if(!cmd)
	{
		rdata->setError() << "Missing Cmd!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	OmnString reserved_objid = cmd->getAttrStr(AOSTAG_RESERVED_OBJID, "");
	if(reserved_objid == "")
	{
		rdata->setError() << "Missing the reservation Objid!";
		return true;
	}

	u32 siteid = rdata->getSiteid();
	AosXmlTagPtr reserved_doc = AosDocClientObj::getDocClient()->getDocByObjid(reserved_objid, rdata);
	OmnString crt_status = reserved_doc->getAttrStr(AOSTAG_RES_STATUS, "");
	if(crt_status == "")
	{
		rdata->setError() << "this reservation miss status!";
		return true;
	}
	if(crt_status == AOSTAG_RES_CANCEL)
	{
		rdata->setError() << "this reservation has canceled, can't confirm";
		return true;
	}
	if(crt_status == AOSTAG_RES_CONFIRM)
	{
		rdata->setError() << "this reservation has confirmed!";
		return true;
	}
	
	AosXmlTagPtr reserved_doc_clone = reserved_doc->clone(AosMemoryCheckerArgsBegin);
	reserved_doc_clone->setAttr(AOSTAG_RES_STATUS, AOSTAG_RES_CONFIRM);
	
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, rdata));
	OmnString appname = rdata->setAppname(AOSAPPNAME_SYSTEM);
	bool rslt = AosDocClientObj::getDocClient()->modifyObj(rdata, reserved_doc_clone, "", false);
	rdata->setUserid(userid);
	rdata->setAppname(appname);
	if(!rslt)
	{
		rdata->setError() << "Internal error!";
		return false;
	}

	// Cancel timer
	u64 reserved_docid = reserved_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(reserved_docid != 0, false);
	AosReservedMgr::getSelf()->cancelTimer(reserved_docid);
	return true;
}


void 
AosSdocReserveRes::timeout(
		const int timerId, 
		const OmnString &timerName, 
		void *parm) 
{
	OmnNotImplementedYet;
	/* Need to resolve the rdata problem. Chen Ding, 07/27/2011
	// The reserved resource timed out. 
	u64 docid = atoll(timerName.data());
	aos_assert(docid != 0);

	u32 siteid = reserved_doc->getAttrU32(AOSTAG_SITEID, 0);
	AosRundataPtr supRdata = OmnNew AosRundata(siteid, AOSAPPNAME_SYSTEM);
	AosXmlTagPtr reserved_doc = AosDocClientObj::getDocClient()->getDoc(docid, "", supRdata);
	aos_assert(reserved_doc);
	AosXmlTagPtr reserved_doc_clone = reserved_doc->clone();
	reserved_doc_clone->setAttr(AOSTAG_RES_STATUS, AOSTAG_RES_CANCEL);
	
	supRdata->resetForReuse(0);
	supRdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, supRdata));
	bool rslt = AosDocServerSelf->modifyObj(supRdata, reserved_doc_clone, false, false);
	aos_assert(rslt);
	
	OmnString sdoc_objid = reserved_doc->getAttrStr(AOSTAG_SDOCOBJID, "");
	aos_assert(sdoc_objid != "");

	AosXmlTagPtr sdoc = AosDocClientObj::getDocClient()->getDoc(siteid, sdoc_objid, rdata);
	
	mLock->lock();
	u64	reservedRes = sdoc->getAttrU64(AOSTAG_RESERVED_NUM, 0);
	u64 availableRes = sdoc->getAttrU64(AOSTAG_RES_AVAILABLE, 0);
	AosXmlTagPtr sdoc_clone = sdoc->clone();
	sdoc_clone->setAttr(AOSTAG_RESERVED_NUM, reservedRes - 1);
	sdoc_clone->setAttr(AOSTAG_RES_AVAILABLE, availableRes + 1);
	mLock->unlock();
	
	supRdata->resetForReuse(0);
	supRdata->setUserid(AosObjMgr::getSuperUserDocid(siteid, supRdata));
	rslt = AosDocServerSelf->modifyObj(supRdata, sdoc_clone, false, false);
	
	aos_assert(rslt);
	
	// remove timer
	AosReservedMgr::getSelf()->removeTimer(docid);
	*/

}

bool
AosSdocReserveRes::getTime(
			const OmnString &timeStr,
			const OmnString &format, 
			int &timerSec, 
			int &timerUser)
{
	u64 tt = atoi(timeStr.data());
	aos_assert_r(tt >=0, false);

	// get Current time
	time_t lt = time(0);
	ptime p1 = from_time_t(lt);
	ptime p2;

	const char *data = format.data();
	switch(data[0])
	{
	case 'y':
		if(format == "year")
		{
			p2 = p1 + years(tt);
		}
		break;
	case 'm':
		if(format == "month")
		{
			p2 = p1 + months(tt);
		}else if(format == "minute")
		{
			p2 = p1 + minutes(tt);
		}
		break;
	case 'd':
		if(format == "day")
		{
			p2 = p1 + days(tt);
		}
		break;
	case 'h':
		if(format == "hour")
		{
			p2 = p1 + hours(tt);
		}
		break;
	case 's':
		if(format == "second")
		{
			p2 = p1 + seconds(tt);
		}
		break;
	default:
		return false;	
	}

	time_duration td = p2 -p1;
	timerSec = td.total_seconds();
	timerUser = 0;
	return true;
}

