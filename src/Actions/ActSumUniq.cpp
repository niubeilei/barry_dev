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
// This action filters data through a hash map.
//
// Modification History:
// 05/06/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSumUniq.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "SEInterfaces/DocClientObj.h"
#include "Util/HashUtil.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/CompareFun.h"
#include "Util/Sort.h"


AosActSumUniq::AosActSumUniq(const bool flag)
:
AosSdocAction(AOSACTTYPE_SUMUNIQ, AosActionType::eSumUniq, flag),
mLock(OmnNew OmnMutex()),
mComp(NULL)
{
}


AosActSumUniq::AosActSumUniq(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_SUMUNIQ, AosActionType::eSumUniq, false),
mLock(OmnNew OmnMutex())
{
	if (!config(def, rdata))
	{
		OmnExcept e(OmnFileLine, OmnErrId::eSyntaxError, rdata->getErrmsg());
		throw e;
	}
}


AosActSumUniq::~AosActSumUniq()
{
	OmnDelete mComp;
	OmnDelete mMap;
}


bool
AosActSumUniq::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// Values are mapped into integer values.
	aos_assert_r(def, false);
	OmnString data_type = def->getAttrStr(AOSTAG_DATA_TYPE);
	AosDataType::E mKeyDataType = AosDataType::toEnum(data_type); 
	if (!AosDataType::isValid(mKeyDataType))
	{
		AosSetErrorU(rdata, "invalid_data_type:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mStartPos = def->getAttrInt("start_pos", -1);
	if (mStartPos < 0)
	{
		AosSetErrorU(rdata, "invalid_start_pos:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mMatchedLength = def->getAttrInt("matched_len", -1);
	if (mMatchedLength <= 0)
	{
		AosSetErrorU(rdata, "invalid_matched_len:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString tt = def->getAttrStr("coding_type", "u8"); 
	mCodingType  = toEnum(tt); 
	if (mCodingType == AosActSumUniq::eInvalid)
	{
		mCodingType = AosActSumUniq::eU8;
	}

	mMappedValue = def->getAttrInt("mapped_value", 0);

	OmnString conf = "<zky_cmparefunc  ";
	conf<< " cmpfun_type=\"custom\" "
			<< " cmpfun_size=\"" << mMatchedLength << "\" "
			<< " cmpfun_alphabetic=\"true\" "
			<< " cmp_pos1 = \"" << 0 << "\" ";
	if (mKeyDataType == AosDataType::eString)
	{
		conf << " cmp_size1 = \"" << mMatchedLength << "\" "
			<< " cmp_datatype1 = \"str\" ";
	}
	else
	{
		conf << " cmp_size1 = \"" << AosDataType::getValueSize(mKeyDataType) << "\" "
			<< " cmp_datatype1 = \"" << AosDataType::getTypeStr(mKeyDataType) << "\"  ";
	}
	conf << " cmpfun_reserve=\"false \" />";

	AosXmlParser parser;
	AosXmlTagPtr conf_tag = parser.parse(conf, "" AosMemoryCheckerArgs);
	aos_assert_r(conf_tag, false);
	mComp = OmnNew AosFunCustom(mMatchedLength, false, conf_tag);
	mMap = OmnNew SumUniqMap(*mComp);
	return true;
}


bool
AosActSumUniq::run(const char *data, int &len, const AosRundataPtr &rdata)
{
	//1. add data to the set (sort by value).
	//   1.1 data format
	//   1.2 data type
	//
	//2. count size in set 
	//   2.1 if size > max break.
	//
	//3. create doc to storge this set.
	//   objid
	
	aos_assert_rr(data, rdata, false);
	int end_pos = mStartPos + mMatchedLength;
	aos_assert_rr(mStartPos >= 0 && end_pos <= len, rdata, false);
	if (len <= 0) return true;

	mLock->lock();
	if (mMap->size() >= mCodingType)
	{
		OmnAlarm << "map size > " << mCodingType << enderr;
		mLock->unlock();
		return false;
	}
	char c = data[end_pos];
	((char*)data)[end_pos] = 0;

	char *key = OmnNew char[mMatchedLength];
	strcpy(key, &data[mStartPos]);

	((char*)data)[end_pos] = c;
	SumUniqMapItr itr = mMap->find(key);
	if (itr == mMap->end())
	{
OmnScreen << "size : " << mMap->size() << " , insert : " << key << endl;  
		mMap->insert(make_pair(key, 1));
		mLock->unlock();
		return true;
	}
OmnScreen << "size : " << mMap->size() << ", " << itr->first << " , " << itr->second << endl;  
	itr->second++;
	mLock->unlock();
	return true;
}


bool	
AosActSumUniq::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return true;
}


AosActionObjPtr
AosActSumUniq::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSumUniq(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool	
AosActSumUniq::finish(OmnString &objid, const AosRundataPtr &rdata)
{
	OmnString records = "<map ";
	records << AOSTAG_OBJID << "=\"" << objid << "\" "
		<< AOSTAG_PUBLIC_DOC << "=\"" << "true" << "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"" << "true" << "\" "
		<< "> <records "
		<< AOSTAG_TYPE << "=\"" << toString(mCodingType) << "\" > ";

	SumUniqMapItr itr;
	mLock->lock();
	for (itr = mMap->begin(); itr != mMap->end(); itr ++)
	{
OmnScreen << itr->first<< " , " << itr->second << endl;
		records << "<record " << AOSTAG_KEY << "= \"" << itr->first
			<< "\" " << AOSTAG_VALUE << " = \"" << mMappedValue ++<< "\" />";
		OmnDelete [] itr->first;
	}
	mLock->unlock();
	records << "</records></map>";

	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(rdata,
			records, "", "", true, false, false, false, false, true);
	aos_assert_r(doc, false);
	objid = doc->getAttrStr(AOSTAG_OBJID, "");
OmnScreen << "Objid : " << objid << endl;
	return true;
}

/*
bool	
AosActSumUniq::finish(OmnString &objid, const AosRundataPtr &rdata)
{
	OmnString records = "<map ";
	records << AOSTAG_OBJID << "=\"" << objid << "\" "
		<< AOSTAG_PUBLIC_DOC << "=\"" << "true" << "\" "
		<< AOSTAG_CTNR_PUBLIC << "=\"" << "true" << "\" "
		<< "> <records "
		<< AOSTAG_TYPE << "=\"" << toString(mCodingType) << "\" > <![BDATA[";

	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	SumUniqMapItr itr;
	mLock->lock();
	for (itr = mMap->begin(); itr != mMap->end(); itr ++)
	{
OmnScreen << itr->first<< " , " << itr->second << endl;
		buff->setOmnStr(itr->first);	
		buff->setU64(mMappedValue ++);	
		OmnDelete [] itr->first;
	}
	mLock->unlock();
	records << buff->dataLen() << ":";
	records.append(buff->data(), buff->dataLen());
	records << "]]></records></map>";

	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->createDocSafe1(rdata,
			records, "", "", true, false, false, false, false, true);
	aos_assert_r(doc, false);
	objid = doc->getAttrStr(AOSTAG_OBJID, "");
	return true;
}
*/
