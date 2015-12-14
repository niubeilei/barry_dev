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
#include "UserMgmt/ClickRate.h"


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Tracer/Tracer.h"
#include "Util/OmnNew.h"

#include "XmlUtil/XmlDoc.h"



AosClickRate::AosClickRate()
{
}

AosClickRate::~AosClickRate()
{
}


bool
AosClickRate::dayClickRate(
		const AosXmlTagPtr &doc,
		const u64 &docid)
{
	/* Chen Ding, 10/24/2010
	 * Need to be reviewed. 
	OmnString year = AosGetSystemYear();
	OmnString mon = AosGetSystemMonth();
	int day = atoll(AosGetSystemDayOfMonth().data());

	OmnString key, value;
	time_t tt = time(0);
	struct tm *theTime = localtime(&tt);
	key<<year<<"/"<<mon<<"/"<<day;
	value<< theTime->tm_hour*60*60+ theTime->tm_min*60+theTime->tm_sec;
	OmnString clicktype;
	clicktype <<"day"<<":"<<docid;
	bool rslt = AosDocServer::getSelf()->checkClickRate(clicktype, key, value);	
	if (rslt==0)
	{
		AosDocServer::getSelf()->cleanValue(doc, AOSTAG_COUNTETRDAY);
	}
	AosDocServer::getSelf()->incrementValue(doc, AOSTAG_COUNTETRDAY, "0", true);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}

bool
AosClickRate::weekClickRate(
		const AosXmlTagPtr &doc,
		const u64 &docid)
{
	/* Chen Ding, 10/24/2010
	 * Need to be reviewed
	OmnString year = AosGetSystemYear();
	OmnString mon = AosGetSystemMonth();
	//int day = atoll(AosGetSystemDayOfMonth().data());

	OmnString key, value;
	//OmnString we = AosGetSystemDayOfWeek();
	//if (atoll(we.data())== 0) {we =""; we << 7;}
	OmnString th;
	th << week();
	key <<year <<"/"<< mon <<"/"<<th;
	value << th;
	OmnString ii;
	ii <<"week"<<":"<<docid;
	bool rslt = AosDocServer::getSelf()->checkClickRate(ii, key, value);	
	if (rslt ==0)
	{
		AosDocServer::getSelf()->cleanValue(doc, AOSTAG_COUNTETRWE);
	}
	AosDocServer::getSelf()->incrementValue(doc, AOSTAG_COUNTETRWE, "0", true);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}

u64
AosClickRate::week()
{
	//从2010/1/4 星期一
	int month_s[2][13]=
	{
		{0,31,28,31,30,31,30,31,31,30,31,30,31},
		{0,31,29,31,30,31,30,31,31,30,31,30,31}
	};

	OmnString year = AosGetSystemYear();
	OmnString mon = AosGetSystemMonth();

	//计算从2010/9/6到现在之间相隔多少天.
	//0=365;1=366;
	u64 sum = 0;	
	int flag = IsRound(atoll(year.data())); 
	for (int i = 1; i<atoll(mon.data()); i++)
	{
		sum = sum + month_s[flag][i];	
	}
	sum = sum -3;
	return sum/7;
}

int 
AosClickRate::IsRound(u64 year)
{ 
	if((year%100)&&(year%4==0)) return 1;  
	if((year%100==0)&&(year%400==0)) return 1;  
	return 0;  
}


bool
AosClickRate::monthClickRate(
		const AosXmlTagPtr &doc,
		const u64 &docid)
{
	/* Chen Ding, 10/24/2010
	 * Need to be reviewed. 
	OmnString year = AosGetSystemYear();
	OmnString mon = AosGetSystemMonth();
	int day = atoll(AosGetSystemDayOfMonth().data());

	OmnString key, value;
	key << year <<"/"<< mon;
	value << day;
	OmnString ii;
	ii <<"month"<<":"<<docid;
	bool rslt= AosDocServer::getSelf()->checkClickRate(ii, key, value);	
	if (rslt ==0)	
	{
		AosDocServer::getSelf()->cleanValue(doc, AOSTAG_COUNTETRMO);
	}
	AosDocServer::getSelf()->incrementValue(doc, AOSTAG_COUNTETRMO, "0", true);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}

bool
AosClickRate::yearClickRate(
		const AosXmlTagPtr &doc,
		const u64 &docid)
{
	/* Chen Ding, 10/24/2010
	 * Need to be reviewed
	OmnString year = AosGetSystemYear();
	OmnString mon = AosGetSystemMonth();
	int day = atoll(AosGetSystemDayOfMonth().data());
	OmnString key, value;
	key << year;
	value <<day;
	OmnString ii;
	ii <<"year"<<":"<<docid;
	bool rslt= AosDocServer::getSelf()->checkClickRate(ii, key, value);	
	if (rslt ==0)
	{
		AosDocServer::getSelf()->cleanValue(doc, AOSTAG_COUNTETRYE);
	}
	AosDocServer::getSelf()->incrementValue(doc, AOSTAG_COUNTETRYE, "0", true);
	return true;
	*/
	OmnNotImplementedYet;
	return false;
}
#endif
