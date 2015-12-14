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
// 11/30/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MultiLang/LangDict.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "MultiLang/LangDictMgr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/QueryObj.h"
#include "SEUtil/Objid.h"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApiG.h"

const OmnString sgEntryNotFound = "Entry Not Found";
const OmnString sgTermValueEmpty = "Dictionary Term Is Empty";
static AosDocClientObjPtr	sgDocClient;
static AosQueryObjPtr		sgQueryObj;

AosLangDict::AosLangDict()
:
mLocale(AosLocale::getDftLocale()),
mLock(OmnNew OmnMutex()),
mCtnrObjid(AosObjid::composeDictCtnrObjid(mLocale))
{
}


AosLangDict::AosLangDict(const AosLocale::E locale) 
:
mLocale(locale),
mLock(OmnNew OmnMutex())
{
}


bool
AosLangDict::config(const OmnString &dirname)
{
	/* JACKIE
	aos_assert_r(dirname != "", false);
	OmnString fname = getFilename(dirname);
	OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file.isGood())
	{
		OmnAlarm << "Dictionary file not exist: " << fname << enderr;
		return false;
	}

	bool finished = false;
	OmnString crt_name;
	OmnString crt_value;
	while (!finished)
	{
		OmnString line = file.getLine(finished);
		if (line != "")
		{
			char c = line.data()[0];
			if (c != '\t' || c != ' ')
			{
				// It is a new term. 
				if (crt_name != "")
				{
					// Found a definition
					mLock->lock();
					if (crt_value == "")
					{
						OmnAlarm << "A term with empty value: " << crt_name << enderr;
						crt_value = sgTermValueEmpty;
						crt_value << ": " << crt_name;
					}
					mMap[crt_name] = crt_value;
					mLock->unlock();
				}
				crt_name = "";
				crt_value = "";

				// The entry should be in the form:
				// 	name  	value
				// 			value
				// 			...
				int idx = findName(line);
				if (idx < 1)
				{
					OmnAlarm << "Term format incorrect: " << line << enderr;
				}
				else
				{
					crt_name = line;
					crt_name.setLength(idx);
					line.remove(0, idx);
					crt_value = line;
					crt_value.removeWhiteSpaces();
				}
			}
			else
			{
				// It is a continuation line.
				line.removeWhiteSpaces();
				crt_value << line;
			}
		}
	}

	*/

	return true;
}


OmnString 
AosLangDict::getTerm(const OmnString &id, const AosRundataPtr &rdata)
{
	AOSLOG_ENTER_R(rdata, "");
	OmnString value;
	AosXmlTagPtr doc;
	AosStr2StrItr_t itr;

	mLock->lock();
	itr = mMap.find(id);
	if (itr != mMap.end()) 
	{
		OmnString term = itr->second;
		mLock->unlock();
		AOSLOG_LEAVE(rdata);
		return term;
	}
	mLock->unlock();

	OmnString objid = AosObjid::composeDictEntryObjid(mLocale, id);
	doc = AosGetDocByObjid(objid, rdata);
	if (!doc)
	{
		return id;
	}

	// The dictionary entry doc is in the following format:
	// 	<entry ...>
	// 		<![CDATA[xxxx]]>
	// 	</entry>
	value = doc->getNodeText();
	if (value == "")
	{
		return id;
	}

	mLock->lock();
	mMap[id] = value;
	mLock->unlock();
	AOSLOG_LEAVE(rdata);
	return value;
}


int
AosLangDict::findName(const OmnString &line) const
{
	// Name is terminated by a while space
	int len = line.length();
	aos_assert_r(len > 0, -1);
	const char *data = line.data();
	int idx = 0;
	while (idx < len && (data[idx] != ' ' || data[idx] != '\t')) idx++;
	if (idx >= len) return -1;
	return idx;
}

