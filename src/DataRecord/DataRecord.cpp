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
// 11/12/2014 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/DataRecord.h"

#include "API/AosApiG.h"
#include "DataField/DataField.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "DataRecord/DataRecord.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "DataRecord/RecordCSV.h"
#include "DataRecord/RecordFixBin.h"
#include "DataRecord/RecordBuff2.h"
#include "DataRecord/RecordVariable.h"
#include "DataRecord/RecordIIL.h"
#include "DataRecord/RecordContainer.h"
#include "DataRecord/RecordXml.h"
#include "DataRecord/RecordStat.h"
#include "DataRecord/RecordMulti.h" 	// 2015/05/05 Add by Andy zhang 
#include "Debug/Debug.h"
#include "DataField/DataFieldBitmap.h"
#include "TaskMgr/Task.h"
#include<sstream>

static AosDataRecord*	sgRecords[AosDataRecordType::eMax];
static OmnMutex			sgLock;
static bool				sgInited = false;

AosDataRecord::AosDataRecord(
		const OmnString &name, 
		const int version AosMemoryCheckDecl) 
:
AosDataRecordObj(version),
mType(AosDataRecordType::eJimoRecord),
mTypeName(name),
mLock(OmnNew OmnMutex()),
mFieldsRaw(NULL),
mNumFields(0),
mDocid(0),
mFieldValues(NULL),
mFieldValFlags(NULL),
mMetaData(0),
mMetaDataRaw(0),
mOffset(0),
mIsDirty(false),
mMemory(0),
mMemLen(0),
mIsValid(true)
{
	AosMemoryCheckerObjCreated(AosClassName::eAosDataRecord);
}


AosDataRecord::AosDataRecord(
		const AosDataRecordType::E type, 
		const OmnString &name, 
		const bool flag AosMemoryCheckDecl)
:
AosDataRecordObj(0),
mType(type),
mTypeName(name),
mLock(OmnNew OmnMutex()),
mFieldsRaw(NULL),
mNumFields(0),
mDocid(0),
mFieldValues(NULL),
mFieldValFlags(NULL),
mMetaData(0),
mMetaDataRaw(0),
mOffset(0),
mIsDirty(false),
mMemory(0),
mMemLen(0),
mIsValid(true)
{
	if (flag)
	{
		bool rslt = registerRecord(type, name, this);
		if (!rslt) OmnThrowException("failed create data record");
	}
	AosMemoryCheckerObjCreated(AosClassName::eAosDataRecord);
}


AosDataRecord::AosDataRecord(
		const AosDataRecord &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecordObj(0),
mOperator(rhs.mOperator),
mType(rhs.mType),
mTypeName(rhs.mTypeName),
mLock(OmnNew OmnMutex()),
mName(rhs.mName),
mTaskDocid(rhs.mTaskDocid),
mDocid(rhs.mDocid),
mMetaData(rhs.mMetaData),
mMetaDataRaw(rhs.mMetaData.getPtr()),
mOffset(rhs.mOffset),
mIsDirty(false),
mMemory(rhs.mMemory),
mMemLen(rhs.mMemLen),
mIsValid(rhs.mIsValid),
mSchemaDocid(rhs.mSchemaDocid)
{
	if (rhs.mRecordDoc)
	{
		mRecordDoc = rhs.mRecordDoc->clone(AosMemoryCheckerArgsBegin);
	}
	
	for (u32 i=0; i<rhs.mDocids.size(); i++)
	{
		mDocids.push_back(rhs.mDocids[i]);
	}

	AosDataFieldObjPtr field;
	for (u32 i=0; i<rhs.mNumFields; i++)
	{
		field = rhs.mFieldsRaw[i]->clone(rdata);
		if (!field)
		{
			AosSetEntityError(rdata, "datarecord_missing_field", 
				"Data Record", mName) 
				<< "Field Index: " << i << enderr;
			OmnThrowException("failed cloning field");
			return;
		}
		mFields.push_back(field);
	}
	mNumFields = mFields.size();

	mFieldsRaw = OmnNew AosDataFieldObj*[mNumFields];
	mFieldValues = OmnNew AosValueRslt[mNumFields];
	mFieldValFlags = OmnNew bool[mNumFields];
	memset(mFieldValFlags, 0, mNumFields); 

	for (u32 i = 0; i < mNumFields; i++)
	{
		mFieldsRaw[i] = mFields[i].getPtr();
	}

	mFieldIdxs = rhs.mFieldIdxs;
	if (mFields.size() != mFieldIdxs.size())
	{
		AosSetEntityError(rdata, "datarecord_num_fields_mismatch", 
				"Data Record", mName) << enderr;
		OmnThrowException("error");
		return;
	}

	AosMemoryCheckerObjCreated(AosClassName::eAosDataRecord);

}


AosDataRecord::~AosDataRecord()
{
	OmnDelete [] mFieldValues;
	mFieldValues = NULL;
	OmnDelete [] mFieldValFlags;
	mFieldValFlags = NULL;
	OmnDelete [] mFieldsRaw;
	mFieldsRaw = NULL;
	AosMemoryCheckerObjDeleted(AosClassName::eAosDataRecord);
}


OmnString
AosDataRecord::getFieldValue(
		AosRundata *rdata,
		const OmnString &field_name)
{
	AosValueRslt value;
	bool rslt = getFieldValue(field_name, value, false, rdata);//child class
	if (rslt==false) return "";
	return value.getStr();
}


bool 				
AosDataRecord::replaceField(
		const int idx,
		const AosDataFieldObjPtr& newfield)
{
	mFields[idx] = newfield;
	mFieldsRaw[idx] = newfield.getPtr();
	return true;
}

bool
AosDataRecord::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);

	OmnTagFuncInfo << endl;
	mFields.clear();
	mFieldsRaw = NULL;
	mFieldValues = NULL;
	mFieldValFlags = NULL;

	mNumFields = 0;
	mFieldIdxs.clear();
	mRecordDoc = def->clone(AosMemoryCheckerArgsBegin); 
	mName = mRecordDoc->getAttrStr(AOSTAG_NAME, "");
	
	mSchemaDocid = def->getAttrU64(AOSTAG_DOCID, 0);
	if (!mSchemaDocid)
	{
		OmnString objid = def->getAttrStr(AOSTAG_OBJID, "");
		if (objid != "")
		{
			AosXmlTagPtr doc = AosGetDocByObjid(objid, rdata);
			aos_assert_r(doc, false);
			mSchemaDocid = doc->getAttrU64(AOSTAG_DOCID, 0);
			aos_assert_r(mSchemaDocid != 0, false);
		}
	}
	
	if (mName == "")
	{
		AosSetEntityError(rdata, "datarecord_missing_name", 
				"Data Record", "record") << def << enderr;
		OmnAlarm << enderr;
		return false;
	}

	OmnString name;
	AosXmlTagPtr elemtag;
	AosDataFieldObjPtr elem;
	map<OmnString, int>::iterator itr;
	
	AosXmlTagPtr elems= mRecordDoc->getFirstChild("datafields");
	if (!elems) return true;

	elemtag = elems->getFirstChild(true);
	while (elemtag)
	{
		elem = AosDataFieldObj::createDataFieldStatic(elemtag, this, rdata);
		if (!elem)
		{
			AosSetEntityError(rdata, "datarecord_internal_error", 
					"Data Record", mName) << enderr;
			OmnAlarm << enderr;
			return false;
		}

		name = elem->getName();
		itr = mFieldIdxs.find(name);
		if(itr == mFieldIdxs.end())
		{
			mFieldIdxs[name] = mFields.size();
			mFields.push_back(elem);
		}
		elemtag = elems->getNextChild();
	}
	mNumFields = mFields.size();
	mFieldsRaw = OmnNew AosDataFieldObj*[mNumFields];
	for (u32 i = 0; i < mNumFields; i++)
	{
		mFieldsRaw[i] = mFields[i].getPtr();
	}

	mFieldValues = OmnNew AosValueRslt[mNumFields];
	mFieldValFlags = OmnNew bool[mNumFields];
	memset(mFieldValFlags, 0, mNumFields); 
	return true;
}


AosDataRecordObjPtr 
AosDataRecord::createDataRecord(
		const AosXmlTagPtr &cfg,
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl)
{
	if(!sgInited) staticInit();

	OmnTagFuncInfo << endl;
	aos_assert_r(cfg, 0);
	AosXmlTagPtr def = cfg;
	aos_assert_r(def, 0);

	OmnString ss = def->getAttrStr(AOSTAG_TYPE);
	AosDataRecordType::E type = AosDataRecordType::toEnum(ss);
	//aos_assert_r(AosDataRecordType::isValid(type), 0);
	if (!AosDataRecordType::isValid(type))
	{
		AosSetEntityError(rdata, "DataRecord_missing_record_type", 
			"DataRecord", "DataRecord") << def->toString() << enderr;
		return 0;
	}

	AosDataRecord * record;
	if (AosDataRecordType::isValid(type))
	{
		record = sgRecords[type];
		record->clone(rdata AosMemoryCheckerArgs);
		if (!record)
		{
			AosSetError(rdata, "internal_error") << enderr;
			return 0;
		}
	}
	else
	{
		AosJimoPtr jimo = AosCreateJimo(rdata, def);
		if (!jimo)
		{
			AosSetEntityError(rdata, "datarecord_invalid_record", 
					"Data Record", mName) << def << enderr;
			return 0;
		}

		if (jimo->getJimoType() != AosJimoType::eDataRecord)
		{
			AosSetEntityError(rdata, "internal_error", 
					"Data Record", mName) << def << enderr;
			return 0;
		}

		record = dynamic_cast<AosDataRecord*>(jimo.getPtr());
		if (!record)
		{
			AosSetEntityError(rdata, "internal_error", 
				"Data Record", mName) << cfg << enderr;
			return 0;
		}
	}

	return record->create(def, task_docid, rdata AosMemoryCheckerFileLine);
}


bool
AosDataRecord::staticInit()
{
	if(sgInited)    return true;

	static AosRecordFixBin		sgAosRecordFixBin(true AosMemoryCheckerArgs);
	//static AosRecordVariable	sgAosRecordVariable(true AosMemoryCheckerArgs);
	static AosRecordCSV			sgAosRecordCSV(true AosMemoryCheckerArgs);
	static AosRecordIIL			sgAosRecordIIL(true AosMemoryCheckerArgs);
	static AosRecordContainer	sgAosRecordContainer(true AosMemoryCheckerArgs);
	static AosRecordBuff2		sgAosRecordBuff2(true AosMemoryCheckerArgs);
	static AosRecordXml			sgAosRecordXml(true AosMemoryCheckerArgs);
	static AosRecordStat		sgAosRecordStat(true AosMemoryCheckerArgs);
    //static AosRecordParmList    sgAosRecordParmList(true AosMemoryCheckerArgs);
    static AosRecordMulti		sgAosRecordMulti(true AosMemoryCheckerArgs);		//2015/05/05 Add by Andy zhang
	sgInited = true;
	return true;
}


bool
AosDataRecord::registerRecord(
		const AosDataRecordType::E type, 
		const OmnString &name,
		AosDataRecord *record)
{
	sgLock.lock();
	if (!AosDataRecordType::isValid(type))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect id: ";
		errmsg << type;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgRecords[type])
	{
		sgLock.unlock();
		OmnString errmsg = "Already registered: ";
		errmsg << type;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgRecords[type] = record;
	sgLock.unlock();
	return true;
}


AosDataFieldObj* 
AosDataRecord::getDataField(
		AosRundata *rdata,
		const OmnString &name)
{
	int fieldIdx = getFieldIdx(name, rdata);	
	aos_assert_r(fieldIdx > -1, 0);
	return getFieldByIdx1(fieldIdx);
}


int
AosDataRecord::getFieldIdx(
		const OmnString &name,
		AosRundata *rdata)
{
	map<OmnString, int>::iterator itr = mFieldIdxs.find(name);
	if(itr != mFieldIdxs.end())
	{
		return itr->second;
	}
	return -1;
}

	
AosXmlTagPtr
AosDataRecord::getDataFieldConfig(
		const OmnString &name,
		AosRundata *rdata)
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
	return 0;
}


void
AosDataRecord::clear()
{
	mDocid = 0;
	mDocids.clear();
	memset(mFieldValFlags, 0, mNumFields); 
	mIsDirty = false;
	mMemory = 0;
	mMemLen = 0;
	mIsValid = true;

	//yang,2015/08/22
	for(u32 i = 0;i < mFields.size();i++)
	{
		mFields[i]->clear();
	}
}

	
bool
AosDataRecord::getRecordENV(
		const OmnString &key,
		OmnString &record_env,
		AosRundata *rdata)
{
	AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
	record_env = "";
	if(!task) return true;
	record_env = task->getTaskENV(key, rdata);
	if(record_env == "") return false;
	return true;
}


AosXmlTagPtr 
AosDataRecord::serializeToXmlDoc(
		const char *data,
		const int data_len,
		AosRundata* rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}


AosBuffPtr 
AosDataRecord::serializeToBuff(
		const AosXmlTagPtr &doc,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return 0;
}

	
bool
AosDataRecord::createRandomDoc(
		const AosBuffPtr &buff,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}
	

bool
AosDataRecord::getFieldValue(
		const int idx, 
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	OmnNotImplementedYet;
	return false;
}

	
bool
AosDataRecord::getFieldValue(
		const OmnString &field_name,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	int idx = getFieldIdx(field_name, rdata);
	return getFieldValue(idx, value, copy_flag, rdata);
}


bool
AosDataRecord::setFieldValue(
		const int idx, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	// Ketty 2014/01/17
	mIsDirty = true;
	return true;
}


bool
AosDataRecord::setFieldValue(
		const OmnString &field_name, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	int idx = getFieldIdx(field_name, rdata);
	return setFieldValue(idx, value, outofmem, rdata);
}


bool 
AosDataRecord::isValid(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}

	
AosDataFieldObj*
AosDataRecord::getFieldByIdx1(const u32 idx)
{
	aos_assert_r(idx < mNumFields, 0);
	return mFieldsRaw[idx];
}


bool 
AosDataRecord::appendField(
		AosRundata *rdata, 
		const OmnString &name,
		const AosDataType::E type,
		const AosStrValueInfo &info)
{
	OmnShouldNeverComeHere;
	return false;
}


AosDataRecordObjPtr 
AosDataRecord::createBuffRecord(AosRundata *rdata AosMemoryCheckDecl)
{
	return OmnNew AosRecordBuff2(false AosMemoryCheckerFileLine);
}


bool 
AosDataRecord::appendField(
		AosRundata *rdata, 
		const AosDataFieldObjPtr &field)
{
	aos_assert_r(field, false);
	mFields.push_back(field);
	mNumFields++;

	OmnDelete [] mFieldsRaw;
	mFieldsRaw = OmnNew AosDataFieldObj*[mNumFields];

	for (u32 i = 0; i < mNumFields; i++)
	{
		mFieldsRaw[i] = mFields[i].getPtr();
	}

	OmnDelete [] mFieldValues;
	mFieldValues = OmnNew AosValueRslt[mNumFields];

	OmnDelete [] mFieldValFlags;
	mFieldValFlags = OmnNew bool[mNumFields];
	memset(mFieldValFlags, 0, mNumFields); 

	int idx = mNumFields - 1;
	mFieldIdxs.insert(make_pair(field->getName(), idx));
	return true;
}


bool 
AosDataRecord::removeFields()
{
	mFields.clear();
	OmnDelete [] mFieldsRaw;
	mFieldsRaw = NULL;

	OmnDelete [] mFieldValues;
	mFieldValues = NULL;

	OmnDelete [] mFieldValFlags;
	mFieldValFlags = NULL;

	mNumFields = 0;
	mFieldIdxs.clear();
	return true;
}

void
AosDataRecord::bindDataProcs(vector<AosDataProcObj*> &procs)
{
	mDataProcs = procs;
}


vector<AosDataProcObj*>& 
AosDataRecord::getBindDataProcs()
{
	return mDataProcs;
}


//display record buffer chars one by one
bool
AosDataRecord::dumpData(bool flag, OmnString title)
{
	if(!flag)
		return true;

	if (mType == AosDataRecordType::eContainer)
	{
		if (((AosRecordContainer *)this)->getInsideRecord())
		{
			((AosRecordContainer *)this)->getInsideRecord()->dumpData(flag, title);
			return true;
		}
	}

	u32 len = getRecordLen();
	OmnScreen << "dump dataproc data: " << endl;
	std::ostringstream os;
	os << title << " (len is: " << dec << len << 
		", number record is: " << getNumRecord() << ") [";

	//get data record's mbuff
	unsigned short c;
	aos_assert_r(mMemory, false);
	for (u32 i = 0; i < len; i++)
	{
		c = mMemory[i];
		c = c & 0xff;
		if (isprint(c))
			os << mMemory[i];
		else
			os << " 0x" << hex << c;
	}

	os << "]" << endl;
	OmnString ss = os.str();
	OmnScreen << ss << endl;
	//cout.flush();
	return true;
}

//
//Compare current record with a new record.
//Both records should have the same schema
//
// Rslt values:
//      1: current one is in first order
//      -1: new record in the param is in first order
//      0:  two records are equal
//
//
bool 
AosDataRecord::compareRecord(
		AosRundata *rdata, 
		AosDataRecordObj* rcd,
		int &cmpRslt)
{
	aos_assert_r(rcd, false);

	AosValueRslt v1, v2;

	cmpRslt = 0;
	for (u32 i = 0; i < mNumFields; i++)
	{
		getFieldValue(i , v1, false, rdata);
		rcd->getFieldValue(i , v2, false, rdata);
		if (v1.isNull()) return 1;
		if (v2.isNull()) return -1;

		AosDataType::E type = AosDataType::autoTypeConvert(v1.getType(), v2.getType());
		if (AosValueRslt::doComparison(eAosOpr_lt, type, v1, v2)) // v1 < v2
		{
			cmpRslt = 1;
			break;
		}
		else if (AosValueRslt::doComparison(eAosOpr_gt, type, v1, v2)) // v1 > v2
		{
			cmpRslt = -1;
			break;
		}
	}

	return true;
}

bool 
AosDataRecord::setData(
			char* data, 
			int len,
			AosMetaData* meta, 
			int& status)
{
	OmnShouldNeverComeHere;
	return false;
}
