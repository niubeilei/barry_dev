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
// This file is generated automatically by the ProgramAid facility. 
//
// Modification History:
// 08/18/2010: Created by cding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "UserMgmt/Vote.h"


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Rundata/Rundata.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlDoc.h"



AosVote::AosVote()
{
}

AosVote::~AosVote()
{
}


bool
AosVote::deleteVotes(const AosXmlTagPtr &root)
{
	OmnNotImplementedYet;
	return false;
}

bool 
AosVote::siggleVote(
	const OmnString &siteid, 
	const OmnString &objid, 
	const OmnString &cid,
	const u64 &num, 
	const AosRundataPtr &rdata)
{  	 
	/*
	 * Chen Ding, 08/12/2011
	//A account can only cast one vote
	//key format:cid;
	//list the name of the format:iilname+siggle+siteid+":"+objid;
	
	aos_assert_r(objid!="", false);
	aos_assert_r(siteid!="", false);

	OmnString iilname = AOSZTG_VOTELIST;
	iilname <<"siggle"<< siteid << ":" << objid;
	u64 docid;
	OmnString errmsg;
	bool isunique;

	if (cid == "")
	{
		errmsg = AOSERR_MISS_CID;
		OmnAlarm << errmsg << enderr;
		return false;
	}
	//Zky2248,Linda, 01/13/2011	
	bool rslt = AosIILClientObj::getDocid(iilname, cid, docid, isunique, rdata);

	if (docid >= num) return false;

	if(docid !=0) AosIILClientObj::removeValueDoc(iilname, cid, docid, rdata);
	docid++;
	rslt = AosIILClientObj::addValueDoc(iilname, cid, docid, false, true, rdata);

	aos_assert_r(rslt, false);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosVote::dateVote(
	const OmnString &siteid, 
	const OmnString &objid, 
	const OmnString &cid,
	const u64 &num, 
	const AosRundataPtr &rdata)
{
	/*
	 * Chen Ding, 08/12/2011
	//A user can only cast num vote a day
	//key format: cid + date(yy/mm/dd);
	//list the name of the format:iilname+date+siteid+":"+objid;
	OmnString iilname = AOSZTG_VOTELIST;
	aos_assert_r(objid !="",false);
	aos_assert_r(siteid !="",false);
	iilname <<"date"<< siteid << ":" << objid;
	u64 docid;
	OmnString errmsg;
	bool isunique;

	time_t tt = time(0);
	struct tm *theTime = localtime(&tt);
	
	OmnString year, mon, day;
	year<<"20"<<theTime->tm_year-100;
	mon <<theTime->tm_mon+1;
	day<<theTime->tm_mday;
	aos_assert_r(year!="", false);
	aos_assert_r(mon!="", false);
	aos_assert_r(day!="", false);

	if (cid == "")
	{
		errmsg = AOSERR_MISS_CID;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	OmnString key;
	key<<cid<<":"<<year<<"/"<<mon<<"/"<<day;
	//Zky2248,Linda, 01/13/2011
	aos_assert_r(key!="", false);
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, key, docid, isunique, rdata);
	if (docid >=num) return false;

	if(docid !=0) AosIILClientObj::getIILClient()->removeValueDoc(iilname, key, docid, rdata);

	docid++;
	rslt = AosIILClientObj::getIILClient()->addValueDoc(iilname, key, docid, false, true, rdata);
	aos_assert_r(rslt, false);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool 
AosVote::timeVote(
	const OmnString &siteid, 
	const OmnString &objid, 
	const OmnString &cid,
	const u64 &num,
	const u64 &mhour, 
	const AosRundataPtr &rdata)
{
	/*
	 * Chen Ding, 08/12/2011
	//In mhour hours to allow num vote;
	//docid format: time(u32)+num(u32);
	aos_assert_r(siteid !="", false);
	aos_assert_r(objid !="", false);

	OmnString iilname = AOSZTG_VOTELIST;
	iilname <<"time"<< siteid << ":" << objid;
	u64 docid;
	OmnString errmsg;
	bool isunique;
	OmnString key;
	key<<cid;
	if (cid == "")
	{
		errmsg = AOSERR_MISS_CID;
		OmnAlarm << errmsg << enderr;
		return false;
	}
		
	//将小时转为秒;
	u64 m = mhour * 60;
	u64 msecond = m * 60;
	//u64 msecond = mhour *60;

	time_t t = time(NULL);
	u64 crttime = t;
	
	//Zky2248,Linda, 01/13/2011
	aos_assert_r(key!="", false);
	bool rslt = AosIILClientObj::getIILClient()->getDocid(iilname, key, docid, isunique, rdata);

	u64 crtnum = 0,oldtime;
	u64 olddocid = docid;

	if (docid == 0)
	{ 
		crtnum ++;
		docid = mergerValue ((u32)crttime, (u32)crtnum);
		rslt = AosIILClientObj::getIILClient()->addValueDoc(iilname, key, docid, false, true, rdata);
    	aos_assert_r(rslt, false);
    	return true;
	}
	//解析docid = oldtime+crtnum
	crtnum = docid & 0xffffffff; //crtnum
	oldtime = docid >>32;  //time

	if (crttime > oldtime + msecond) 
	{
		crtnum = 0;
		oldtime = crttime;
	}

	if ((crttime <= oldtime + msecond && crtnum >= num)|| (crtnum >= num) || 
			(crtnum>=num && oldtime == crttime))
	{
		return false;
	}

	crtnum ++;
	docid = mergerValue ((u32)oldtime, (u32)crtnum);
	AosIILClientObj::getIILClient()->removeValueDoc(iilname, key, olddocid, rdata);
	rslt = AosIILClientObj::getIILClient()->addValueDoc(iilname, key, docid, false, true, rdata);
    aos_assert_r(rslt, false);
    return true;
	*/
	OmnNotImplementedYet;
	return false;
}


u64
AosVote::mergerValue(u32 mtime, u32 num)
{
	u64 tvalue = tvalue & 0;
	tvalue = mtime;
	tvalue = tvalue << 32;
	tvalue = tvalue | num;
	return tvalue;
}


bool
AosVote::controlOnVote(
		const OmnString &siteid,
		const OmnString &objid,
		const OmnString &cid,
		const OmnString &mode,
		const OmnString &num,
		const OmnString &hour, 
		const AosRundataPtr &rdata)
{
	char c = mode.data()[0];
	switch(c)
	{
	case 's':
		 if (strcmp(mode, "siggle") ==0)
		 {
			return siggleVote(siteid, objid, cid, atoll(num.data()), rdata);
		 }
		 break;

	case 'd':
		 if (strcmp(mode, "date")== 0)
		 {
			return dateVote(siteid, objid, cid, atoll(num.data()), rdata);
		 }
		 break;

	case 't':
		 if (strcmp(mode, "time")== 0)
		 {
			if (hour != 0)
				return timeVote(siteid, objid, cid, atoll(num.data()), atoll(hour.data()), rdata);
			return false;
		 }
		 break;
	}
	return false;
}
#endif
