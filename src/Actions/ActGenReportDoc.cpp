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
// This action sets a value to rundata:
//
// Modification History:
// 09/12/2012 Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActGenReportDoc.h"

#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocSelector/DocSelector.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "QueryClient/QueryClient.h"
#include <string>
#include <vector>
using namespace std;

AosActGenReportDoc::AosActGenReportDoc(const bool flag)
:
AosSdocAction(AOSACTTYPE_GENREPORTDOC, AosActionType::eGenReportDoc, flag),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
}

AosActGenReportDoc::AosActGenReportDoc(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_GENREPORTDOC, AosActionType::eGenReportDoc, false),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}



AosActGenReportDoc::~AosActGenReportDoc()
{
}

bool
AosActGenReportDoc::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	//action formate:
	//<acton ...>
	//	<values>
	//		<value>
	//		<attrname .../>
	//		<attrvalue.../>
	//		</value>
	//		...
	//	</values>
	//	<cmd>
	//		...
	//	</cmd>
	//</action>
	//1. values: user define attributes, used to manage query;
	aos_assert_rr(def, rdata, false);
	AosXmlTagPtr values = def->getFirstChild("values");
	if (values)
	{
		AosXmlTagPtr value = values->getFirstChild();
		OmnString attr_name = "";
		OmnString attr_value= "";
		while(value)
		{
			AosValueRslt nameRslt;
			AosValueRslt valueRslt;
			bool rslt = AosValueSel::getValueStatic(nameRslt, value, "attrname", rdata);
			aos_assert_rr(rslt, rdata, false);
			if (!nameRslt.isNull())
			{
				AosSetError(rdata, "eValueIncorrect");
				OmnAlarm << rdata->getErrmsg() << ". def: " << def->toString() << enderr;
				return false;
			}

			/*
			if (nameRslt.isXmlDoc())
			{
				AosSetError(rdata, "eValueIncorrect");
				OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << def->toString() << enderr;
			}
			*/
			attr_name  = nameRslt.getStr();
			AosValueSel::getValueStatic(valueRslt, value, "attrvalue", rdata);
			if (!valueRslt.isNull())
			{
				AosSetError(rdata, "eValueIncorrect");
				OmnAlarm << rdata->getErrmsg() << ". def: " << def->toString() << enderr;
				return false;
			}

			/*
			if (valueRslt.isXmlDoc())
			{
				AosSetError(rdata, "eValueIncorrect");
				OmnAlarm << rdata->getErrmsg() << ". Sdoc: " << def->toString() << enderr;
			}
			*/

			attr_value = valueRslt.getStr();
			mAttrs[attr_name] = attr_value;
			value = values->getNextChild();
		}
	}
	mQueryReq = def->getFirstChild("cmd");
	if (!mQueryReq)
	{
		AosSetErrorU(rdata, "no query");
		OmnAlarm << "def: " << def->toString() << enderr;
		return false;
	}
	return true;
}

bool	
AosActGenReportDoc::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	bool rslt = config(sdoc, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "faild to config");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	if (mThread)
	{
		AosSetErrorU(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "gen_reoprtdoc", 0, false, true, __FILE__, __LINE__);
	mThread->start();
	rdata->setOk();
	return true;
}

bool	
AosActGenReportDoc::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	return runQuery(mRundata);
}

bool
AosActGenReportDoc::signal(const int threadLogicId)
{
	return true;
}

bool
AosActGenReportDoc::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}

bool
AosActGenReportDoc::runQuery(const AosRundataPtr &rdata)
{
	//Request format
	//<cmd ...>
	//	<cond>
	//		...
	//	</cond>
	//</cmd>
	//Response result format:
	//<Contents>
	//	<record .../>
	//	...
	//</Contents>
	aos_assert_rr(mQueryReq, rdata, false);
	AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(mQueryReq, rdata);
	if (!query || !query->isGood())
	{
		return false;
	}
	OmnString contStr;
	bool rslt = query->procPublic(mQueryReq, contStr, rdata);
	aos_assert_rr(rslt, rdata, false);
	AosXmlParser parser;
	AosXmlTagPtr contents = parser.parse(contStr, "" AosMemoryCheckerArgs);;
	aos_assert_rr(contents, rdata, false);

	map<OmnString, OmnString>::iterator itr;
	for(itr = mAttrs.begin(); itr != mAttrs.end(); itr++)
	{
		contents->setAttr(itr->first, itr->second);
	}
	AosXmlTagPtr doc = AosCreateDoc(contents, rdata);
	aos_assert_rr(doc, rdata, false);
OmnScreen << "Report: " << doc->toString() << endl;
	return true;
}
