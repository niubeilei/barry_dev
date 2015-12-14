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
// 05/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RecordProc/RecordProc.h"

#include "API/AosApiG.h"
#include "DataField/DataField.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "DataRecord/RecordVariable.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


static AosRecordProc*	sgRecords[AosDataRecordType::eMax];
static OmnMutex			sgLock;


AosRecordProc::AosRecordProc(
		const OmnString &name, 
		const AosDataRecordType::E type, 
		const AosTaskObjPtr &task,
		const bool flag AosMemoryCheckDecl)
:
mType(type),
mName(""),
mTask(task),
mDocid(0),
mFieldLoopIdx(0)
{
	mProcRecordIndex = 0;
	AosMemoryCheckerObjCreated(AosClassName::eAosDataRecord);
}

AosRecordProc::AosRecordProc()
:
mDocid(0),
mFieldLoopIdx(0)
{
	mProcRecordIndex = 0;
	mBuff = "";
	mBuffSize = 0;
}

AosRecordProc::AosRecordProc(
		const AosRecordProc &rhs,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
:
mType(rhs.mType),
mName(rhs.mName),
mTask(rhs.mTask),
mDocid(rhs.mDocid),
mFieldLoopIdx(rhs.mFieldLoopIdx)
{
	mProcRecordIndex = 0;
	if(rhs.mRecordDoc)
	{
		mRecordDoc = rhs.mRecordDoc->clone(AosMemoryCheckerArgsBegin);
	}
	
	if(rhs.mExtensionDoc)
	{
		mExtensionDoc = rhs.mExtensionDoc->clone(AosMemoryCheckerArgsBegin);
	}
	
	for (u32 i=0; i<rhs.mDocids.size(); i++)
	{
		mDocids.push_back(rhs.mDocids[i]);
	}

	AosDataFieldObjPtr field;
	AosDataRecordObjPtr thisptr(this, false);
	for (u32 i=0; i<rhs.mFields.size(); i++)
	{
		field = rhs.mFields[i]->clone(rdata);
		if (!field)
		{
			OmnAlarm << "Failed cloning field: " << i << enderr;
			OmnThrowException("failed cloning field");
			return;
		}
		mFields.push_back(field);
	}
	AosMemoryCheckerObjCreated(AosClassName::eAosDataRecord);
}


AosRecordProc::~AosRecordProc()
{
	AosMemoryCheckerObjDeleted(AosClassName::eAosDataRecord);
}


bool
AosRecordProc::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	mFields.clear();
	mRecordDoc = def->clone(AosMemoryCheckerArgsBegin); 
	mName = mRecordDoc->getAttrStr(AOSTAG_NAME, "doc");

	OmnString name;
	set<OmnString> names;
	set<OmnString>::iterator itr;
	AosDataFieldObjPtr elem;
	AosXmlTagPtr elemtag;
	AosDataRecordObjPtr thisptr(this, false);
	
	OmnString extension_fields_objid = mRecordDoc->getAttrStr(AOSTAG_EXTENSION_FIELDS_OBJID);
	if(extension_fields_objid != "")
	{
		AosXmlTagPtr extension_doc = AosGetDocByObjid(extension_fields_objid, rdata);
		if(!extension_doc)
		{
			AosSetErrorU(rdata, "can't_get_extension_tag") << mRecordDoc->toString() << enderr;
			AosLogError(rdata);
			return false;
		}
	
		mExtensionDoc = extension_doc->clone(AosMemoryCheckerArgsBegin);
		elemtag = mExtensionDoc->getFirstChild(true);
		while (elemtag)
		{
			elem = AosDataFieldObj::createDataField(elemtag, thisptr, rdata);
			aos_assert_r(elem, false);

			if (!elem->isFixed())
			{
				AosSetErrorU(rdata, "elem_not_fixed_len:") << mExtensionDoc->toString() << enderr;
				AosLogError(rdata);
				return false;
			}

			name = elem->getName();
			itr = names.find(name);
			if(itr != names.end())
			{
				AosSetErrorU(rdata, "elem_name_exist:") << mExtensionDoc->toString() << enderr;
				return false;
			}
			names.insert(name);
			mFields.push_back(elem);
			elemtag = mExtensionDoc->getNextChild();
		}
	}

	elemtag = mRecordDoc->getFirstChild(true);
	while (elemtag)
	{
		elem = AosDataFieldObj::createDataField(elemtag, thisptr, rdata);
		aos_assert_r(elem, false);

		if (!elem->isFixed())
		{
			AosSetErrorU(rdata, "elem_not_fixed_len:") << mRecordDoc->toString() << enderr;
			AosLogError(rdata);
			return false;
		}

		name = elem->getName();
		itr = names.find(name);
		if(itr != names.end())
		{
			AosSetErrorU(rdata, "elem_name_exist:") << mRecordDoc->toString() << enderr;
			AosLogError(rdata);
			return false;
		}
		names.insert(name);
		mFields.push_back(elem);
		elemtag = mRecordDoc->getNextChild();
	}
	
	mRecordLen = def->getAttrInt("record_len", 0);
	aos_assert_r(mRecordLen>0, false);

	mInternalData.setLength(mRecordLen);
	mData = (char *)mInternalData.data();
	mDataLen = mRecordLen;
	memset(mData, 0, mRecordLen);
	return true;
}


AosDataRecordObjPtr 
AosRecordProc::createDataRecord(
		const AosXmlTagPtr &cfg,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{

	aos_assert_r(cfg, 0);
	AosXmlTagPtr def;
	u64 docid = cfg->getAttrU64(AOSTAG_DOCID, 0);
	OmnString objid = cfg->getAttrStr(AOSTAG_OBJID);
	if(docid > 0)
	{
		def = AosGetDocByDocid(docid, rdata);
	}
	else if (objid != "")
	{
		def = AosGetDocByObjid(objid, rdata);

		// Chen Ding, 09/01/2012
		if (!def)
		{
			AosSetErrorU(rdata, "failed_get_doc") << ":" << objid << enderr;
			AosLogError(rdata);
			return 0;
		}

		docid = def->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(docid > 0, 0);

		cfg->setAttr(AOSTAG_DOCID, docid);
	}
	else
	{
		def = cfg;
	}
	aos_assert_r(def, 0);	

	OmnString ss = def->getAttrStr(AOSTAG_TYPE);
	AosDataRecordType::E type = AosDataRecordType::toEnum(ss);
	aos_assert_r(AosDataRecordType::isValid(type), 0);
	AosRecordProc *record = sgRecords[type];
	if (!record)
	{
		AosSetErrorU(rdata, "data_record_not_registered") << ":" << ss << enderr;
		AosLogError(rdata);
		return 0;
	}

	return record->create(def, task, rdata AosMemoryCheckerFileLine);
}


bool
AosRecordProc::staticInit()
{
	return true;
}


bool
AosRecordProc::registerRecord(const OmnString &name, AosRecordProc *record)
{
	return true;
}


AosXmlTagPtr 
AosRecordProc::serializeToXmlDoc(
		const char *data,
		const int data_len,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << mName << ">";
	int idx = 0;
	for (u32 i=0; i<mFields.size(); i++)
	{
		bool rslt = mFields[i]->serializeToXmlDoc(docstr, idx, data, data_len, rdata);
		aos_assert_r(rslt, 0);
	}
	
	docstr << "</" << mName << ">";
	AosXmlTagPtr doc = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(doc, 0);
	return doc;
	return 0;
}


AosBuffPtr 
AosRecordProc::serializeToBuff(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc, 0);
		
	//felicia, 2012/09/26
	AosBuffPtr buff = OmnNew AosBuff(eBuffInitSize AosMemoryCheckerArgs);
	for (u32 i=0; i<mFields.size(); i++)
	{
		bool rslt = mFields[i]->serializeToBuff(doc, buff, rdata);
		aos_assert_r(rslt, 0);
	}
	return buff;	
	return 0;
}

OmnString
AosRecordProc::getFieldName(int &fieldindex)
{
	return	mFields[fieldindex]->getName();
}
	
int
AosRecordProc::getFieldIdx(
		const OmnString &name,
		const AosRundataPtr &rdata)
{
	for(u32 i=0; i<mFields.size(); i++)
	{
		if(mFields[i]->getName() == name)
		{
			return i;
		}
	}
	return -1;
}

	
AosXmlTagPtr
AosRecordProc::getDataFieldConfig(
		const OmnString &name,
		const AosRundataPtr &rdata)
{
	aos_assert_r(name != "", 0);
	aos_assert_r(mRecordDoc, 0);

	AosXmlTagPtr xml = mRecordDoc->clone(AosMemoryCheckerArgsBegin); 
	AosXmlTagPtr elemtag = xml->getFirstChild();
	OmnString nm;
	while (elemtag)
	{
		nm = elemtag->getAttrStr(AOSTAG_NAME);
		if(nm == name) return elemtag->clone(AosMemoryCheckerArgsBegin);
		elemtag = xml->getNextChild();
	}

	if(!mExtensionDoc) return 0;
	xml = mExtensionDoc->clone(AosMemoryCheckerArgsBegin);
	elemtag = xml->getFirstChild();
	while (elemtag)
	{
		nm = elemtag->getAttrStr(AOSTAG_NAME);
		if(nm == name) return elemtag->clone(AosMemoryCheckerArgsBegin);
		elemtag = xml->getNextChild();
	}

	return 0;
}


void
AosRecordProc::clear()
{
	mTask = 0;
	mRecordDoc = 0;
	mDocids.clear();
	mFields.clear();
	mFieldValues.clear();
}

	
bool
AosRecordProc::getRecordENV(
		const OmnString &key,
		OmnString &record_env,
		const AosRundataPtr &rdata)
{
	record_env = "";
	if(!mTask) return true;
	record_env = mTask->getTaskENV(key, rdata);
	if(record_env == "") return false;
	return true;
}


AosDataRecordObjPtr 
AosRecordProc::clone(const AosRundataPtr &rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordProc(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}

OmnString	
AosRecordProc::toString() const
{
	OmnString ss;
	ss << "\nAosRecordProc:"
		<< "\n    Docid:         " << mDocid
		<< "\n    Num of Fields: " << mFields.size()
		<< "\n    Data Length:   " << mDataLen
		<< "\n    Record Len:    " << mRecordLen;
	return ss;
}

AosDataRecordObjPtr 
AosRecordProc::create(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata AosMemoryCheckDecl) const
{
	AosRecordProc* record = OmnNew AosRecordProc(AOSRECORDTYPE_FIXBIN, AosDataRecordType::eFixedBinary, task, false AosMemoryCheckerFileLine);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosRecordProc::getField(
		const int idx,
		AosValueRslt &value,
		const bool copy_flag,
		const AosRundataPtr &rdata) 
{
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);
	if(mFieldValues.find(idx) != mFieldValues.end())
	{
		value = mFieldValues[idx];
		return true;
	}
	bool rslt = mFields[idx]->getValueFromRecord(this, value, copy_flag, rdata);
	aos_assert_r(rslt, false);
	mFieldValues[idx] = value;
	return true;
}


bool		
AosRecordProc::setField(
		const int idx, 
		const AosValueRslt &value, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(idx >= 0 && (u32)idx < mFields.size(), false);
	bool rslt = mFields[idx]->setValueToRecord(this, value, rdata);
	aos_assert_r(rslt, false);
	mFieldValues[idx] = value;
	return true;
}


bool
AosRecordProc::reset(const AosRundataPtr &rdata)
{
	aos_assert_r(mData, false);
	mInternalData.setLength(mRecordLen);
	mDataLen = mRecordLen;
	mData = (char *)mInternalData.data();
	aos_assert_r(mData, false);
	memset(mData, 0, mRecordLen);
	mDocid = 0;
	mDocids.clear();
	mFieldLoopIdx = 0;
	mFieldValues.clear();
	return true;
}

bool
AosRecordProc::resetBuff(const AosRundataPtr &rdata)
{
	mBuff="";
	mBuffSize = 0;
	return true;
}

bool 		
AosRecordProc::setData(char *data, const int len, const bool need_copy)
{
	if (need_copy)
	{
		mInternalData.assign(data, len);
		mData = (char *)mInternalData.data();
		mDataLen = len;
		return true;
	}

	mData = data;
	mDataLen = len;
	return true;
}


char *      
AosRecordProc::generateRecord(int &data_len, const AosRundataPtr &rdata)
{
	if (!mData)
	{
		data_len = 0;
		return 0;
	}

	data_len = mDataLen;
	return mData;
}

bool
AosRecordProc::createRandomDoc(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	int len = mFields.size();
	aos_assert_r(len > 0, false);

	bool rslt;
	for(int i=0; i<len; i++)
	{
		rslt = mFields[i]->createRandomValue(buff, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}
