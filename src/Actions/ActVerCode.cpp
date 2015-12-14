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
// 04/16/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActVerCode.h"

#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "Util/DynArray.h"
#include "SEBase/SeUtil.h"
#include "Security/SessionMgr.h"
#include "Security/Ptrs.h"
#include "Actions/Ptrs.h"
#include "Actions/ActUtil.h"
#include "SmartDoc/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include <vector>
using namespace std;

AosActVerCode::AosActVerCode(const bool flag)
:
AosSdocAction(AOSACTTYPE_VERCODE, AosActionType::eVerCode, flag)
{
}


/*
AosActVerCode::AosActVerCode(const AosXmlTagPtr &def)
:
AosSdocAction(AosActionType::eAVerCode, false)
{
	if(!config(def))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError,
				OmnString("Missing the doc tag"));
		throw e;
	}
}
*/


AosActVerCode::~AosActVerCode()
{
}


/*
AosActionObjPtr
AosActVerCode::clone(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosActVerCode(def);
	}

	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
*/


bool
AosActVerCode::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	//EncryptType: And "sdocvercode" in the "EncryptType" the same
	//<action zky_actid  = "avercode" zky_otype="zky_actdoc" type="avercode">
	//<config minutes="5" EncryptType="(SdocVerCode smartdoc the same)SH1|MD5"/>
	//</action>
	//Zky3233, Linda, 2011/04/18
	AosSessionObjPtr session = rdata->getSession();
	if (!session)
	{
		AosSetError(rdata, "actvercode_user_not_logged_in") << enderr;
		return false;
	}

	// AosSmartDocPtr smartdoc = rdata->getSmartdoc();
	// if (!smartdoc)
	// {
	// 	rdata->setError() << "Missing smartdoc: ";
	// 	OmnAlarm << rdata->getErrmsg() << enderr;
	// 	return false;
	// }

	//get Actions Doc
	if (!sdoc)
	{
		rdata->setError() << "Missing Actions Doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr config = sdoc->getFirstChild("config");
	if (!config)
	{
		rdata->setError() << "Missing Config Doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int minute = config->getAttrInt("minutes", 5); 
	OmnString encryptType = config->getAttrStr("EncryptType", "MD5");

	AosXmlTagPtr request = rdata->getReceivedDoc();
	if (!request)
	{
		rdata->setError() << "Missing Request Doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	//OmnString vercode = request->getAttrStr("zky_vercode", "");
	//<item name=\'zky_vercode\'><![CDATA[xxxxxxxxxxxxxxxxxxxxxxxx]]></item>
	OmnString vercode = request->getChildTextByAttr("name", "zky_vercode");
	if (vercode == "")
	{
		rdata->setError() << "Verification Code is NULL!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString uid = session->getVerificationCode(); 
	session->setVerificationCode("");	
	if (uid == "")
	{
		rdata->setError() << "Session Verification Code is NULL!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!checkTime(uid, minute, rdata)) 
	{
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString id;
	if (encryptType == "SH1")
	{
		//SH1
		OmnShouldNeverComeHere;
		return false;
	}
	else
	{
		//MD5
		id = AosMD5Encrypt(uid);
	}
	
	aos_assert_r(id !="", false);
	if (id != vercode)
	{
		rdata->setError() << "Missing Verification Code !";
		return false;
	}

	return true;
}

bool
AosActVerCode::checkTime(const OmnString &uid, const int minute, const AosRundataPtr &rdata)
{
	AosStrSplit split;
	bool finished;
	OmnString pair[2];
	split.splitStr(uid.data(),"_", pair, 2, finished);
	//Current time
	u32 crtsecond = OmnGetSecond();
	//generate a VerCode time
	u32 oldsecond = atoll(pair[0].data());
	
	aos_assert_r(oldsecond!=0, false);
	ptime p1 = from_time_t(oldsecond);
	if (p1.is_not_a_date_time())
	{
		rdata->setError() << "Missing time format!";
		OmnAlarm << rdata->getErrmsg()<< enderr;
		return false;
	}
	ptime p2 = p1 + minutes(minute);
	if (p2.is_not_a_date_time())
	{
		rdata->setError() << "Missing time format!";
		OmnAlarm << rdata->getErrmsg()<< enderr;
		return false;
	}
	time_duration td = p2 - p1;
	int nanosec = td.total_seconds();
	u32 intervaltime= oldsecond + nanosec;
	if (intervaltime < crtsecond)
	{
		rdata->setError() << "Verification Code request TimeOut!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}


AosActionObjPtr
AosActVerCode::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActVerCode(false);
	}

	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


