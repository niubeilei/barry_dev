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
//   
//
// Modification History:
// 08/27/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ErrorMgr/ErrMsgMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "ErrorMgr/ErrorMgr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

static OmnString sgInvalidErrorId = "Invalid Error ID!";

AosErrMsgMgr::AosErrMsgMgr(const AosLocale::E locale)
:
mLocale(locale)
{
	init();
}


AosErrMsgMgr::~AosErrMsgMgr()
{
}


bool
AosErrMsgMgr::init()
{
	OmnString dir = AosErrorMgr::getSelf()->getDirname();
	aos_assert_r(dir != "", false);

	OmnString fname = dir;
	AosLocale::init();
	fname << "/" << AosLocale::toStr(mLocale);
	OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file.isGood())
	{
		return true;
		// // This means there is no errmsg file for this locale. 
		// OmnAlarm << "No error message file for locale: " 
		// 	<< AosLocale::toStr(mLocale)
		// 	<< ". Filename: " << fname << enderr;
		// return false;
	}

	// Error messages are arranged in the following format:
	// 	<errmsgs>
	// 		<errmsg error_id="xxx"><![CDATA[xxx]]></errmsg>
	// 		<errmsg error_id="xxx"><![CDATA[xxx]]></errmsg>
	// 		...
	// 	</errmsgs>
	OmnString contents;
	bool rslt = file.readToString(contents);
	aos_assert_r(rslt, false);

	if (contents != "")
	{
		AosXmlTagPtr xml = AosXmlParser::parse(contents AosMemoryCheckerArgs);
		aos_assert_r(xml, false);

		AosXmlTagPtr entry = xml->getFirstChild();
		while (entry)
		{
			AosErrmsgId::E error_id = AosErrmsgId::toEnum(entry->getAttrStr("error_id"));
			if (!AosErrmsgId::isValid(error_id))
			{
				OmnAlarm << "Invalid Error ID: " << entry->getAttrStr("error_id")
					<< " for Locale: " << AosLocale::toStr(mLocale) << enderr;
			}
			else
			{
				mMsgs[error_id] = entry->getNodeText();
			}
		}
	}

	return true;
}


OmnString 
AosErrMsgMgr::getErrmsg(const AosErrmsgId::E id)
{
	aos_assert_r(AosErrmsgId::isValid(id), sgInvalidErrorId);
	return mMsgs[id];
}

