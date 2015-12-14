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
#include "SmartDoc/SdocSum.h"

#include "Debug/Error.h"
#include "SEInterfaces/DocClientObj.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "Security/Session.h"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/WebRequest.h"


AosSdocSum::AosSdocSum(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_SUM, AosSdocId::eSum, flag)
{
}


AosSdocSum::~AosSdocSum()
{
}


bool
AosSdocSum::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// The smart doc is in the following format:
	// <sdoc zky_container=”xxx” 
	// 		zky_sumid=”xxx” 
	// 		zky_sumtype=”xxx”
	// 		zky_idaname=”xxx” 
	// 		zky_valueaname=”xxx”
	// 		zky_timeaname=”xxx”
	// 		zky_timeformat=”xxx”/>
	// 
	/* Chen Ding, 08/12/2011
	 * Need to check with Lynch
	OmnString siteid = rdata->getSiteid();
	if(siteid == "")
	{
		rdata->setError() << "Missing Siteid!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 1. Retrieve the smart doc
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// 1. Retrieve the created doc
	AosXmlTagPtr created_doc = rdata->getReceivedDoc();
	if (!created_doc)
	{
		rdata->setError() << "Failed to retrieve the created doc ";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Retrieve the sumtype
	SumType sumtype = getSumtype(sdoc);
	aos_assert_r(sumtype != eInvalid, false);

	// 2. Retrieve the container
	OmnString ctnr_objid = created_doc->getAttrStr(AOSTAG_PARENTC);
	if (ctnr_objid == "")
	{
		rdata->setError() << "Failed to retrieve the container objid ";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr ctnr_obj = AosDocClientObj::getDocClient()->getDoc(siteid, ctnr_objid, rdata);	
	if (!ctnr_obj)
	{
		rdata->setError() << "Failed to retrieve the container";
		return false;
	}

	// 4. Retrieve the IILID
	u64 iilid = getIILID(rdata, sdoc, ctnr_obj);

	if (iilid == 0)
	{
		// The IIL is not there yet. Create it.
		//iilid = AosIILClient::getSelf()->createIILPublic(eAosIILType_Str, rdata);
		if (iilid == 0)
		{
			rdata->setError() << "Failed to create IIL";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		// iilid = OmnGetSecond();
		// Need to save the IIL
		bool rslt = saveIIL(ctnr_obj, sdoc, iilid, rdata);
		aos_assert_r(rslt, false);
	}
	
	//iil = AosIILClient::getSelf()->retrieveIILPublic(iilid, eAosIILType_Str);
	// AosIILType iiltype = eAosIILType_Str;
	// iil = AosIILClient::getSelf()->loadIILByIDPublic(iilid, iiltype);
	// if (!iil)
	// {
	// 	rdata->setError() << "Failed to retrieve the IIL: " << iilid 
	// 		<< ":" << ctnr_objid;
	// 	OmnAlarm << rdata->getErrmsg() << enderr;
	// 	return false;
	// }

	// Retrieve the key
	OmnString key;
	bool rslt = retrieveKey(sumtype, sdoc, created_doc, key, rdata);
	aos_assert_r(rslt && key != "", false);

	// Retrieve the value
	u64 value = 0;
	aos_assert_r(retrieveValue(sdoc, created_doc, value, rdata), false);

OmnScreen << "increment iil : " << iilid << endl;

	// Increment the value
	//aname_2011_0
	//aname_2011_1
	if (!AosIILClient::getSelf()->incrementDocid(iilid, key, value, rdata))
	{
		rdata->setError() << "Failed to increment value";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	// OmnString iilname = AOSZTG_SUMMARY;
	// iilname << iilid;
	// if (!AosIILClient::getSelf()->incrementDocid(iilname, key, value))
	// {
	// 	rdata->setError() << "Failed to increment value";
	// 	OmnAlarm << rdata->getErrmsg() << enderr;
	// 	return false;
	// }

	return true;
	*/
	OmnNotImplementedYet;
	return false;
}


bool
AosSdocSum::retrieveKey(
		const SumType sumtype, 
		const AosXmlTagPtr &sdoc, 
		const AosXmlTagPtr &created_doc, 
		OmnString &key,
		const AosRundataPtr &rdata)
{
	// It retrieves the key from 'created_doc'. It then constructs the 
	// key based on the sumtype. 
	OmnString idaname = sdoc->getAttrStr(AOSTAG_ID_ATTRNAME);
	if (idaname == "")
	{
		rdata->setError() << "Failed to retrieve the ID Attribute name: " 
			<< sdoc->getAttrStr(AOSTAG_OBJID);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	key = created_doc->getAttrStr(idaname);
	if (key == "")
	{
		rdata->setError() << "Failed to retrieve the key value: " << idaname;
		return false;
	}

	AosTime timeinfo;
	switch (sumtype)
	{
	case eYearly:
		 aos_assert_r(getTime(timeinfo, sdoc, created_doc, rdata), false);
		 key << "_" << timeinfo.getYear();
		 return true;
		
	case eMonthly:
		 aos_assert_r(getTime(timeinfo, sdoc, created_doc, rdata), false);
		 key << "_" << timeinfo.getYear() << "_" <<  timeinfo.getMonth();
		 return true;
		
	case eDaily:
		 aos_assert_r(getTime(timeinfo, sdoc, created_doc, rdata), false);
		 key << "_" << timeinfo.getYear() << "_" << timeinfo.getMonth() << "_" << timeinfo.getDay();
		 return true;
		
	case eHourly:
		 aos_assert_r(getTime(timeinfo, sdoc, created_doc, rdata), false);
		 key << "_" << timeinfo.getYear() << "_" << timeinfo.getMonth() << "_" << timeinfo.getDay() << "_" << timeinfo.getHour();
		 return true;

	default:
		 rdata->setError() << "Unrecognized sumtype: " << sumtype;
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosSdocSum::getTime(
		AosTime &timeinfo,
		const AosXmlTagPtr &sdoc, 
		const AosXmlTagPtr &created_doc, 
		const AosRundataPtr &rdata)
{
	// It retrieves the time.
	// 1. If smartdoc defines AOSTAG_TIME_ATTRNAME, the time is retrieved
	//    from the created_doc. 
	// 2. Otherwise, it is the current time.
	OmnString timeaname = sdoc->getAttrStr(AOSTAG_TIME_ATTRNAME);
	if (timeaname == "")
	{
		AosTimeUtil::getTime(timeinfo);
		return true;
	}

	OmnString timestr = created_doc->getAttrStr(timeaname);
	if (timestr == "")
	{
		// There is no such attribute. It is an error.
		//rdata->setError() << "Failed to retrieve the time value: " << timeaname;
		AosTimeUtil::getTime(timeinfo);
		return true;
	}

	// Retrieve the time format
	OmnString format = sdoc->getAttrStr(AOSTAG_TIMEFORMAT);
	aos_assert_r(AosTimeUtil::getTime(timestr, format, timeinfo), false);
	return true;
}


bool
AosSdocSum::retrieveValue(
		const AosXmlTagPtr &sdoc, 
		const AosXmlTagPtr &created_doc,
		u64   &value, 
		const AosRundataPtr &rdata)
{
	// It retrieves the value. 
	OmnString valueaname = sdoc->getAttrStr(AOSTAG_VALUE_ATTRNAME);
	if (valueaname == "")
	{
		rdata->setError() << "Failed to retrieve the value attribute name: " 
			<< sdoc->getAttrStr(AOSTAG_OBJID);
		return false;
	}

	u64 dft = sdoc->getAttrU64(AOSTAG_DEFAULT_VALUE, 0);
	value = created_doc->getAttrU64(valueaname, dft);
	return true;
}

AosSdocSum::SumType
AosSdocSum::getSumtype(
		const AosXmlTagPtr &sdoc)
{
	// It retrieve the Sumtype
	OmnString typeStr = sdoc->getAttrStr(AOSTAG_SUM_TYPE);
	const char *data = typeStr.data();
	switch(data[0])
	{
	case 'd':
		if (typeStr == AOSSUMTYPE_DAILY) return eDaily;
		break;
	
	case 'h':
		if (typeStr == AOSSUMTYPE_HOURLY) return eHourly;
		break;
	
	case 'm':
		if (typeStr == AOSSUMTYPE_MONTH) return eMonthly;
		break;

	case 'y':
		if (typeStr == AOSSUMTYPE_YEAR) return eYearly;
		break;

	default:
		break;

	}
	
	return eInvalid;
}


u64
AosSdocSum::getIILID(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &ctnr)
{
	// container format
	//	<container ...>
	//		<summid zky_summid="..." iilid="..." />
	//		<summid zky_summid="..." iilid="..." />
	//	</container>
	
	aos_assert_r(ctnr, 0);
	aos_assert_r(sdoc, 0);
	
	OmnString sumid = sdoc->getAttrStr(AOSTAG_SUMID);
	if (sumid == "")
	{
		rdata->setError() << "Failed to retrieve the SumID";
		return false;
	}

	u64 iilid = 0;
	//  Retrieve the sumid
	AosXmlTagPtr ctnrSumIdTag = ctnr->getFirstChild(AOSTAG_CTNR_SUMID);
	while(ctnrSumIdTag)
	{
		OmnString sid = ctnrSumIdTag->getAttrStr(AOSTAG_SUMID, "");
		if(sid == sumid)
		{
			iilid = ctnrSumIdTag->getAttrU64(AOSTAG_IILID, 0);
			break;
		}
		ctnrSumIdTag = ctnr->getNextChild(AOSTAG_CTNR_SUMID);
	}
	
	return iilid;		
}


bool
AosSdocSum::saveIIL(
		AosXmlTagPtr &ctnr,
		const AosXmlTagPtr &sdoc,
		u64	 iilid,
		const AosRundataPtr &rdata)
{
	// container format
	//	<container ...>
	//		<summid zky_summid="..." iilid="..." />
	//		<summid zky_summid="..." iilid="..." />
	//	</container>
	
	aos_assert_r(ctnr, 0);
	aos_assert_r(sdoc, 0);
	AosXmlTagPtr newdoc = ctnr->clone(AosMemoryCheckerArgsBegin);
	
	OmnString sumid = sdoc->getAttrStr(AOSTAG_SUMID);
	if (sumid == "")
	{
		rdata->setError() << "Failed to retrieve the SumID";
		return false;
	}
	
	//OmnString sumTag;
	//sumTag << "<" << AOSTAG_CTNR_SUMID << " "
	//		<< AOSTAG_SUMID << "=\"" << sumid << "\" "
	//		<< AOSTAG_IILID << "=\"" << iilid << "\" "
	//		<< "/>";

	newdoc->addNode1(AOSTAG_CTNR_SUMID);
	AosXmlTagPtr doc = newdoc->getFirstChild(AOSTAG_CTNR_SUMID);
	while(doc)
	{
		OmnString attrvalue = doc->getAttrStr(AOSTAG_IILID);
		if (attrvalue =="")
		{
			doc->setAttr(AOSTAG_SUMID, sumid);
			doc->setAttr(AOSTAG_IILID, iilid);
			break;
		}
		doc = newdoc->getNextChild(AOSTAG_CTNR_SUMID);
	}
	
	return AosDocClientObj::getDocClient()->modifyObj(rdata, newdoc, "false", false);
}
