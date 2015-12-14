////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 	Created: 13/04/2011 by Lynch 
////////////////////////////////////////////////////////////////////////////
#include "Dictionary/DictionaryMgr.h"
#include "Dictionary/Dictionary.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/OmnNew.h"

OmnSingletonImpl(
		AosDictionaryMgrSingleton, 
		AosDictionaryMgr, 
		AosDictionaryMgrSelf, 
		"AosDictionaryMgr");


AosDictionaryMgr::AosDictionaryMgr()
{
}


AosDictionaryMgr::~AosDictionaryMgr()
{
}


bool 
AosDictionaryMgr::start()
{
	return true;
}


bool 
AosDictionaryMgr::stop()
{
	return true;
}


bool
AosDictionaryMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


const OmnString 
AosDictionaryMgr::resolveTerm(
		const u64 &entry_id, 
		const OmnString &languageCode, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the dictionary entry identified by 'entry_id', 
	// which is the docid of the entry. Dictionary entry is an XML doc
	// whose format is:
	// 	<entry AOSTAG_DOCID="entry_id">
	// 		<entry type="zh-cn">xxx</entry>
	// 		<entry type="en">xxx</entry>
	// 		...
	// 	</entry>
	if (!mDftDictionary)
	{
		mDftDictionary = OmnNew AosDictionary();
		aos_assert_rr(mDftDictionary, rdata, "");
	}
	AosXmlTagPtr xml = mDftDictionary->getTerm(entry_id, rdata);
	aos_assert_rr(xml, rdata, "");
	aos_assert_rr(languageCode != "", rdata, "");

	AosXmlTagPtr tag = xml->getChildByAttr("type", languageCode);
	if (!tag) return "";
	return tag->getNodeText();
}

