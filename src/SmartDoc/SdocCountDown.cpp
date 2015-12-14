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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocCountDown.h"

#include "Debug/Error.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "Security/Session.h"
#include "Util/StrSplit.h"
#include "Util/SPtr.h"
#include "Util/Locale.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"

AosSdocCountDown::AosSdocCountDown(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_COUNTDOWN, AosSdocId::eCountDown, flag)
{
}


AosSdocCountDown::~AosSdocCountDown()
{
}

bool 
AosSdocCountDown::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// format 
	// <smartdoc 
	// 		ctnrtype="xxx"
	//		displayFormat="Day" // Day , Hour, or Min
	//		aname=""
	//		coutdowntype="A">
	// 		....
	// </smartdoc>

	// original doc format
	// <doc
	// 		endingtime="MM-DD-YEAR HH:MM:SS" // should use this format
	// 		...>
	// 	</doc>
	//
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u64 userid = rdata->getUserid();
	if (userid == AOS_INVDID)
	{
		rdata->setError() << "Missing userid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr original_doc = rdata->getAccessedDoc();

	if (!original_doc)
	{
		rdata->setError() << "Missing user account!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// can change end_time attr name
	OmnString end_time_aname = sdoc->getAttrStr(AOSTAG_COUNTDOWN_END_ANAME, AOSTAG_ENDING_TIME);

	// can change remain_time attr name
	OmnString remain_time_aname = sdoc->getAttrStr(AOSTAG_COUNTDOWN_REMAIN_ANME, AOSTAG_REMAINING_TIME);

	// countdown type "Time" or "Usage"
	OmnString type = sdoc->getAttrStr(AOSTAG_COUNTDOWN_TYPE);

	if (type == AOSCDOWNTYPE_TIME)
	{
		int Min = 60;
		int Hour = Min * 60;
		int Day = Hour * 24;
		int remingsec;
		int endingsec;

		OmnString endingTime = original_doc->getAttrStr(AOSTAG_ENDING_TIME, "NO_ENDING_TIME");

		OmnString days;
		days = original_doc->getAttrStr(AOSTAG_DAYS);

		if (endingTime == "NO_ENDING_TIME" && days == "")
		{
			rdata->setError() << "Missing ending time!";
			//OmnAlarm << rdata->getErrmsg() << enderr;
			return true;
		}

		if (days != "" && endingTime == "NO_ENDING_TIME") 
		{
			endingTime = OmnGetTime(AosLocale::eUSA);

			//endingtime="MM-DD-YEAR HH:MM:SS"  should use this format
			AosStrSplit split;
			OmnString parts[2];
			int nn = split.splitStr(endingTime, " ", parts, 2);
			int endYear, endMonth, endDay, endHour, endMin, endSec;
			if(nn != 2)
			{
				rdata->setError() << "Time Format isn't right";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			OmnString endtime1 = parts[0];
			OmnString endtime1part[3], endtime2part[3];
			// === MM-DD-YEAR
			nn = split.splitStr(endtime1, "-", endtime1part, 3);
			if(nn != 3)
			{
				rdata->setError() << "Time Format isn't right";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			endMonth = endtime1part[0].toInt();
			endDay = endtime1part[1].toInt();
			endYear = endtime1part[2].toInt();
		
			// ==== HH:MM:SS
			OmnString endtime2 = parts[1];
			nn = split.splitStr(endtime2, ":", endtime2part, 3);
			if(nn != 3)
			{
				rdata->setError() << "Time Format isn't right";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
		
			endHour = endtime2part[0].toInt();
			endMin = endtime2part[1].toInt();
			endSec = endtime2part[2].toInt();

			int d = days.toInt();
			endMonth += d / 30;

			d = d % 30;
			endDay += d;

			endingsec = AosGetEpochTime(endYear-1900, endMonth-1, endDay, endHour, endMin, endSec);
			OmnString endingTime2;
			endingTime2 << endMonth<<"-"<<endDay<<"-"<<endYear<<" "<<endHour<<":"<<endMin<<":"<<endSec;

			// Chen Ding, 2011/01/18
			// AosDocServer::getSelf()->modifyDoc(original_doc, "ending_time", endingTime2, false, false, true);
			// Chen Ding, 2011/01/27
			// AosDocServer::getSelf()->modifyAttrStr(AOSAPPNAME_SYSTEM, userid, original_doc, 
			// 	"ending_time", endingTime2, "", false, false, errcode, errmsg);
			//--Ketty
			/*AosDocServer::getSelf()->modifyAttrStr1(AOSAPPNAME_SYSTEM, userid, 
				original_doc->getAttrU64(AOSTAG_DOCID, 0), 
				original_doc->getAttrStr(AOSTAG_OBJID), 
				"ending_time", endingTime2, "", false, false, rdata->getErrcode(), rdata->getErrmsg(), ttl, __FILE__, __LINE__);*/
			AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				original_doc->getAttrU64(AOSTAG_DOCID, 0), 
				original_doc->getAttrStr(AOSTAG_OBJID), 
				"ending_time", endingTime2, "", false, false, true);
		}else 

		if (endingTime != "NO_ENDING_TIME")
		{
			AosStrSplit split;
			OmnString parts[2];
			int nn = split.splitStr(endingTime, " ", parts, 2);
			int endYear, endMonth, endDay, endHour, endMin, endSec;
			if(nn != 2)
			{
				rdata->setError() << "Time Format isn't right";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			OmnString endtime1 = parts[0];
			OmnString endtime1part[3], endtime2part[3];
			// === MM-DD-YEAR
			nn = split.splitStr(endtime1, "-", endtime1part, 3);
			if(nn != 3)
			{
				rdata->setError() << "Time Format isn't right";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			endMonth = endtime1part[0].toInt();
			endDay = endtime1part[1].toInt();
			endYear = endtime1part[2].toInt();
		
			// ==== HH:MM:SS
			OmnString endtime2 = parts[1];
			nn = split.splitStr(endtime2, ":", endtime2part, 3);
			if(nn != 3)
			{
				rdata->setError() << "Time Format isn't right";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
		
			endHour = endtime2part[0].toInt();
			endMin = endtime2part[1].toInt();
			endSec = endtime2part[2].toInt();
			endingsec = AosGetEpochTime(endYear-1900, endMonth-1, endDay, endHour, endMin, endSec);
		}

		unsigned long crtTime = OmnGetSecond();
	
	
		if (crtTime >= (unsigned long)endingsec)
		{
			//errcode = eAosXmlInt_Expired;
			rdata->setError() << "Count Down Finished!";
			return true;
		}
		remingsec = endingsec - crtTime;


		int ReDay = remingsec / Day; 	//left Day
		int ReHour = (remingsec - ReDay*Day) / Hour; 	// left Hour
		int ReMin = (remingsec - ReDay*Day - ReHour*Hour) / Min;	//left Min

		OmnScreen << "left Time is :" << ReDay << "Day" << ReHour << "Hour" << ReMin << "Mins" << endl;

		OmnString doc_reminTime;
		doc_reminTime << ReDay << "Days "<< ReHour << "Hours " << ReMin << "Mins";


		bool rslt;
		// Chen Ding, 2011/01/18
		// rslt = AosDocServer::getSelf()->modifyDoc(original_doc, end_time_aname, endingTime, false, false, true);
		// Chen Ding, 2011/01/27
		// rslt = AosDocServer::getSelf()->modifyAttrStr(AOSAPPNAME_SYSTEM, userid, original_doc, 
		// 		end_time_aname, endingTime, "", false, false, errcode, errmsg);
		//int ttl = 0;
		//--Ketty
		/*rslt = AosDocServer::getSelf()->modifyAttrStr1(AOSAPPNAME_SYSTEM, userid, 
				original_doc->getAttrU64(AOSTAG_DOCID, 0), 
				original_doc->getAttrStr(AOSTAG_OBJID), 
				end_time_aname, endingTime, "", false, false, rdata->getErrcode(), rdata->getErrmsg(), ttl, __FILE__, __LINE__);*/
		rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				original_doc->getAttrU64(AOSTAG_DOCID, 0), 
				original_doc->getAttrStr(AOSTAG_OBJID), 
				end_time_aname, endingTime, "", false, false, true);
		if (!rslt)
		{
			rdata->setError() << "Failed to Modify Doc";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		// Chen Ding, 2011/01/18
		// rslt = AosDocServer::getSelf()->modifyDoc(original_doc, remain_time_aname, doc_reminTime, false, false, true);
		// Chen Ding, 2011/01/27
		// rslt = AosDocServer::getSelf()->modifyAttrStr(AOSAPPNAME_SYSTEM, userid, original_doc, 
		// 		remain_time_aname, doc_reminTime, "", false, false, errcode, errmsg);
		//ttl = 0;
		//--Ketty
		/*rslt = AosDocServer::getSelf()->modifyAttrStr1(AOSAPPNAME_SYSTEM, userid, 
				original_doc->getAttrU64(AOSTAG_DOCID, 0), 
				original_doc->getAttrStr(AOSTAG_OBJID), 
				remain_time_aname, doc_reminTime, "", false, false, rdata->getErrcode(), rdata->getErrmsg(), ttl, __FILE__, __LINE__);*/
		rslt = AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				original_doc->getAttrU64(AOSTAG_DOCID, 0), 
				original_doc->getAttrStr(AOSTAG_OBJID), 
				remain_time_aname, doc_reminTime, "", false, false, true);
		if (!rslt)
		{
			rdata->setError() << "Failed to Modify Doc";
			return false;
		}
		return true;
	}

	// if (type == AOSCDOWNTYPE_USAGE)
	// {
	// 	// It is count down based on usage
	// 	AosIILPtr iil = AosIILMgrObj::getIILMgr()->getIILPublic(AOSZTG_COUNTDOWN_IIL);
	// 	aos_assert_r(iil, false);
	// 	if (iil->getIILType() != eAosIILType_U32)
	// 	{
	// 		OmnAlarm << "Not a u32 iil" << enderr;
	// 		return false;
	// 	}
	//
	//	AosIILU32Ptr iilu32 = (AosIILU32*)iil.getPtr();
	//	u64 counter_id = sdoc->getAttrU64(AOSTAG_COUNTER_ID, AOS_INVDID);
	//	if (!counter_id)
	//	{
	//		OmnAlarm << "Can not get couter id" <<enderr;
	//		return false;
	//	}
	//	u32 value = iilu32->decrementCounter(counter_id);
	// 
	//	if (aname != "")
	//	{
	//		origial_doc->setAttr(aname, value);
	//		//...;
	//	}
	//	return true;
	//}
	
	return true;
}
