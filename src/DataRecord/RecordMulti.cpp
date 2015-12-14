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
// Modification History:
// 2015/05/05 Created By Andy Zhang
////////////////////////////////////////////////////////////////////////////
#include "DataRecord/RecordMulti.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "MetaData/MetaData.h"
#include "MetaData/DocidMetaData.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "Debug/Debug.h"
#include "API/AosApiC.h"


AosRecordMulti::AosRecordMulti(const bool flag AosMemoryCheckDecl)
:
AosDataRecord(AosDataRecordType::eMulti,
	AOSRECORDTYPE_MULTI, flag AosMemoryCheckerFileLine),
mRecordLen(-1),
mEstimateRecordLen(-1),
mPicekExprRaw(0),
mRecordIdx(-1),
mWithFieldCache(true)
{
}


AosRecordMulti::AosRecordMulti(
		const AosRecordMulti &rhs,
		AosRundata *rdata AosMemoryCheckDecl)
:
AosDataRecord(rhs, rdata AosMemoryCheckerFileLine),
mRundata(rhs.mRundata),
mRecordLen(rhs.mRecordLen),
mEstimateRecordLen(rhs.mEstimateRecordLen),
mVirFields(rhs.mVirFields),
mRecordsMap(rhs.mRecordsMap),
mSchemaIdMap(rhs.mSchemaIdMap),
mRecordIdx(rhs.mRecordIdx),
mWithFieldCache(rhs.mWithFieldCache),
mRowDelimiter(rhs.mRowDelimiter)
{
	mRecords.clear();
	mRecordsRaw.clear();

	AosDataRecordObjPtr record;
	for (u32 i = 0; i < rhs.mRecords.size(); i ++)
	{
		record = rhs.mRecords[i]->clone(rdata AosMemoryCheckerArgs);
		aos_assert(record);
		mRecords.push_back(record);
		mRecordsRaw.push_back(record.getPtr());
	}

	if (rhs.mPicekExpr)
	{
		mPicekExpr = rhs.mPicekExpr->cloneExpr();
		aos_assert(mPicekExpr);
		mPicekExprRaw = mPicekExpr.getPtr();
	}
}


AosRecordMulti::~AosRecordMulti()
{
}


bool
AosRecordMulti::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	// conf like this
	//	<datarecord type="fixbin" zky_name="myds">
	//		<datafields>
	//			<datafield type="str" zky_length="2" zky_offset="0" zky_name="type_def"/>
	//			<datafield zky_name="f1">
	//				<value fieldname="system_type" recordname="vc_54"></value>
	//				<value recordname="ib_73" fieldname="system_type1"></value>
	//			</datafield>
	//			<datafield zky_name="f2">
	//				<value recordname="ib_73" fieldname="system_type"></value>
	//			</datafield>
	//		</datafields>
	//		<datarecords>
	//			<datarecord zky_name="vc_54">
	//				<datafields>
	//					<datafield type="str" zky_name="system_type"/>
	//				</datafields>
	//			</datarecord>
	//			<datarecord t zky_name="ib_73">
	//				<datafields>
	//					<datafield type="str" zky_name="system_type"/>
	//				</datafields>
	//			</datarecord>
	//		</datarecords>
	//		<picker>
	//			<match_expr>type_def + _ + length</match_expr>
	//		</picker>
	//	</datarecord>
	
	aos_assert_r(def, false);

	bool rslt;
	OmnString name;
	AosXmlTagPtr elemtag, value_node;
	AosDataFieldObjPtr elem;
	map<OmnString, int>::iterator itr;

	mFields.clear();

	mRundata = rdata;
	mRecordDoc = def->clone(AosMemoryCheckerArgsBegin); 
	mName = mRecordDoc->getAttrStr(AOSTAG_NAME, "");
	if (mName == "")
	{
		AosSetEntityError(rdata, "datarecord_missing_name", 
				"Data Record", "record") << def << enderr;
		OmnAlarm << enderr;
		return false;
	}

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
			//AosSetEntityError(rdata, "datarecord_field_already_exist", 
			//			"Data Record", mName) << mExtensionDoc << enderr;
			//return false;
			mFieldIdxs[name] = mFields.size();
			mFields.push_back(elem);
		}
		else
		{
			OmnAlarm << "field already exist! " << name << enderr; 
		}

		//mFieldIdxs[name] = mFields.size();
		//mFields.push_back(elem);
		//mFieldsRaw.push_back(elem.getPtr());
		elemtag = elems->getNextChild();
	}
	mNumFields = mFields.size();
	mFieldsRaw = OmnNew AosDataFieldObj*[mNumFields];
	for (u32 i = 0; i < mNumFields; i++)
	{
		mFieldsRaw[i] = mFields[i].getPtr();
	}

	mRowDelimiter = AosParseRowDelimiter(def->getAttrStr(AOSTAG_ROW_DELIMITER));
	aos_assert_r(mRowDelimiter != "", false);
	mWithFieldCache = def->getAttrBool("with_field_cache", mWithFieldCache);
	
	OmnString errormsg;
	AosXmlTagPtr datarecords = def->getFirstChild(AOSTAG_DATARECORDS);
	rslt = configDataRecords(datarecords, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr datafields = def->getFirstChild(AOSTAG_DATAFIELDS);
	rslt = configDataFields(datafields, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr picker = def->getFirstChild(AOSTAG_PICKER);
	aos_assert_r(picker, false);
	AosXmlTagPtr match_expr_node = picker->getFirstChild(true);
	aos_assert_r(match_expr_node, false);
	OmnString expr_str = match_expr_node->getNodeText();
	mPicekExpr = AosParseExpr(expr_str, errormsg, rdata);
	aos_assert_r(mPicekExpr, false);
	mPicekExprRaw = mPicekExpr.getPtr();
	mRecordIdx = mRecords.size();

	if (mNumFields <= 0)
	{
		AosSetEntityError(rdata, "recordcsv_missing_field", 
			"Record", mName) << def->toString() << enderr;
		return false;
	}

	mEstimateRecordLen = 0;
	for (u32 i=0; i<mNumFields; i++)
	{
		if (!mFieldsRaw[i]->needValueFromField())
		{
			int field_len = mFieldsRaw[i]->mFieldInfo.field_data_len;
			if (field_len < eEstimateEachFieldLen)
			{
				field_len = eEstimateEachFieldLen;
			}
			mEstimateRecordLen += field_len;
		}
	}

	mFieldValues = OmnNew AosValueRslt[mFields.size()];
	mFieldValFlags = OmnNew bool[mFields.size()];
	memset(mFieldValFlags, 0, mFields.size());
	return true;
}


bool
AosRecordMulti::configDataFields(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);
	mVirFields.clear();

	int field_idx, record_idx;
	AosXmlTagPtr value;
	OmnString record_name, rfield_name, field_name;
	AosXmlTagPtr datafield = def->getFirstChild(true);
	while (datafield)
	{
		vector<int> record_idxs(mRecords.size() + 1, -1);
		field_name = datafield->getAttrStr(AOSTAG_NAME);
		value = datafield->getFirstChild("value");
		while (value)
		{
			record_name = value->getAttrStr(AOSTAG_RECORD_NAME);
			rfield_name = value->getAttrStr(AOSTAG_FIELD_NAME);
			map<OmnString, int>::iterator itr = mRecordsMap.find(record_name);
			aos_assert_r(itr != mRecordsMap.end(), false);

			record_idx = itr->second;
			field_idx = mRecordsRaw[record_idx]->getFieldIdx(rfield_name, rdata);
			aos_assert_r(record_idxs[record_idx] == -1, false);

			record_idxs[record_idx] = field_idx;
			value = datafield->getNextChild();
		}
		mVirFields.push_back(record_idxs);
		datafield = def->getNextChild();
	}
	return true;
}


bool 
AosRecordMulti::configDataRecords(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	aos_assert_r(def, false);
	OmnString record_docid;
	OmnString record_name;
	AosDataRecordObjPtr data_record;
	AosXmlTagPtr data_record_node = def->getFirstChild(true);
	while (data_record_node)
	{
		data_record = AosDataRecordObj::createDataRecordStatic(data_record_node, mTaskDocid, rdata AosMemoryCheckerArgs);
		aos_assert_r(data_record, false);
		record_name = data_record->getRecordName();
		aos_assert_r(record_name != "", false);
		record_docid = data_record_node->getAttrStr(AOSTAG_DOCID);
		mRecords.push_back(data_record);
		mRecordsRaw.push_back(data_record.getPtr());
		mRecordsMap.insert(make_pair(record_name, (mRecords.size() - 1)));
		mSchemaIdMap.insert(make_pair(record_docid, (mRecords.size() - 1)));
		data_record_node = def->getNextChild();
	}
	return true;
}

AosDataRecordObjPtr 
AosRecordMulti::clone(AosRundata *rdata AosMemoryCheckDecl) const
{
	try
	{
		return OmnNew AosRecordMulti(*this, rdata AosMemoryCheckerFileLine);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosJimoPtr
AosRecordMulti::cloneJimo() const
{
	try
	{
		return OmnNew AosRecordMulti(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating object" << enderr;
		return 0;
	}
}


AosDataRecordObjPtr 
AosRecordMulti::create(
		const AosXmlTagPtr &def,
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl) const
{
	AosRecordMulti * record = OmnNew AosRecordMulti(false AosMemoryCheckerArgs);
	record->setTaskDocid(task_docid);
	bool rslt = record->config(def, rdata);
	aos_assert_r(rslt, 0);
	return record;
}


bool		
AosRecordMulti::getFieldValue(
		const int idx,
		AosValueRslt &value,
		const bool copy_flag,
		AosRundata* rdata)
{
	int new_idx = -1;
	new_idx = mVirFields[idx][mRecordIdx];
	if (new_idx == -1)
	{
		if (mWithFieldCache && mFieldValFlags[idx])
		{
			value = mFieldValues[idx];
  			return true;
 		}
		else
		{
			int index = 0;
			bool rslt = mFieldsRaw[idx]->getValueFromRecord(
					this, mMemory, mMemLen, index, value, copy_flag, rdata);
			aos_assert_r(rslt, false);

			if (mWithFieldCache)
			{
				mFieldValues[idx] = value;
				mFieldValFlags[idx] = true;
			}

		}
		return true;
	}
	aos_assert_r(new_idx != -1, false);

	return mRecordsRaw[mRecordIdx]->getFieldValue(new_idx, value, copy_flag, rdata);
}


bool		
AosRecordMulti::setFieldValue(
		const int idx, 
		AosValueRslt &value, 
		bool &outofmem,
		AosRundata* rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


int
AosRecordMulti::getRecordLen()
{
	//aos_assert_r(mRecordIdx != (int)mRecords.size(), -1);
	return mRecordLen;
	//return mRecordsRaw[mRecordIdx]->getRecordLen();
}
	

char *
AosRecordMulti::getData(AosRundata *rdata)
{
	return mMemory;
}


void
AosRecordMulti::clear()
{
	AosDataRecord::clear();
	mRecordLen = 0;
	memset(mFieldValFlags, 0, mFields.size());

	if(mRecordIdx == (int)mRecords.size())
	{
		for (u32 i = 0; i < mRecordsRaw.size(); i++)
		{
			mRecordsRaw[i]->clear();
		}
		return;
	}
	
	mRecordsRaw[mRecordIdx]->clear();
	mRecordIdx = mRecords.size();
	return;
}


bool
AosRecordMulti::setData(
		char *data,
		const int len,
		AosMetaData *metaData,
		int &status)
		//const int64_t offset)
{
	bool rslt;
	aos_assert_r(data && len > 0, false);
	mMemory = data;
	mMemLen = len;
	if(metaData)
	{
		mMetaData = metaData;
		mMetaDataRaw = metaData;
	}

	//mOffset = offset;
	
	determineRecordLen(data, len, mRecordLen, status);

	if (mRecordLen > len)
	{
		status = -1;
		mRecordLen = -1;
		return true;
	}

	if (status == -1)
	{
		return true;
	}

	OmnString schema_id = mMetaDataRaw->getAttribute("schemaid");
	if (schema_id != "")
	{
		map<OmnString, int>::iterator itr = mSchemaIdMap.find(schema_id);
		if (itr != mSchemaIdMap.end())
		{
			mRecordIdx = itr->second;
			return mRecordsRaw[mRecordIdx]->setData(data, len, metaData, status);
		}
	}
	
	OmnString record_name;
	record_name = getRecordNameByPicker(mRundata);
	map<OmnString, int>::iterator itr = mRecordsMap.find(record_name);
	if (itr == mRecordsMap.end())
	{
		mIsValid = false;
		status = -2;
OmnScreen << "***********invalid record************" << endl;
OmnScreen << "record len :" << mRecordLen << endl;
OmnScreen << "record name :" << record_name << endl;
OmnString rcd_str(&data[0],mRecordLen);
OmnScreen << rcd_str << endl;
		return true;
	}
	mRecordIdx = itr->second;
	return mRecordsRaw[mRecordIdx]->setData(data, len, metaData, status);
}


bool
AosRecordMulti::determineRecordLen(
		char *data,
		const int64_t &len,
		int &record_len,
		int &status)
{
	//Jozhi strstr to test
	char last_byte = data[len-1];
	data[len-1] = 0;
	char* head = strstr(data, mRowDelimiter.data());
	if (head)
	{
		char* end = head + mRowDelimiter.length();
		record_len = end - data;
		data[len-1] = last_byte;
	}
	else
	{
		data[len-1] = last_byte;
		char* last = OmnString::strrstr(data, len, mRowDelimiter.data());
		if (last)
		{
			char* end = last + mRowDelimiter.length();
			record_len = end - data;
		}
		else
		{
			record_len = -1;
			status = -1;
		}
	}
	return true;
}


void
AosRecordMulti::flush(const bool clean_memory)
{
	aos_assert(mRecordIdx != (int)mRecords.size());
	return mRecordsRaw[mRecordIdx]->flush(clean_memory);
}


bool 
AosRecordMulti::appendField(
		AosRundata *rdata, 
		const OmnString &name,
		const AosDataType::E type,
		const AosStrValueInfo &info)
{
OmnShouldNeverComeHere;
	return false;
}


bool 
AosRecordMulti::appendField(
		AosRundata *rdata, 
		const AosDataFieldObjPtr &field)
{
	bool rslt = AosDataRecord::appendField(rdata, field);
	aos_assert_r(rslt, false);

	vector<int> record_idxs(mRecords.size() + 1, -1);
	mVirFields.push_back(record_idxs);
	return true;
}


OmnString 
AosRecordMulti::getRecordNameByPicker(AosRundata *rdata)
{
	OmnString str (mMemory, 2);
	str << "_" << mRecordLen;
	return str;
}

bool 
AosRecordMulti:: removeFields()
{
	AosDataRecord::removeFields();
	return true;
}


AosXmlTagPtr 
AosRecordMulti::serializeToXmlDoc(
		const char *data,
		const int data_len,
		AosRundata* rdata)
{
	if (mRecordIdx == (int)mRecords.size())
	{
		mMemory = (char *)data;
		mMemLen = data_len;
		mRecordLen = data_len;
		if (!mMetaData)
		{
			mMetaData = OmnNew AosDocidMetaData(); 
			mMetaDataRaw = mMetaData.getPtr();
		}
		int64_t len = data_len;
		mMetaDataRaw->setRecordLength(len); 
		OmnString record_name;
		record_name = getRecordNameByPicker(mRundata);
		map<OmnString, int>::iterator itr = mRecordsMap.find(record_name);

		if (itr == mRecordsMap.end())
		{
			OmnAlarm << "may by error." << enderr; 
			return NULL;
		}
		mRecordIdx = itr->second;
	}
	aos_assert_r(mRecordIdx != (int)mRecords.size(), NULL);
	return mRecordsRaw[mRecordIdx]->serializeToXmlDoc(data, data_len, rdata);
}
