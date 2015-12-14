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
// This data proc filters a record.
//
//
// Modification History:
// 05/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcCompose.h"

#include "API/AosApi.h"


AosDataProcCompose::AosDataProcCompose(const bool flag)
:
AosDataProc(AOSDATAPROC_COMPOSE, AosDataProcId::eCompose, flag),
mValueFieldIdx(-1)
{
}
	

AosDataProcCompose::AosDataProcCompose(const AosDataProcCompose &proc)
:
AosDataProc(proc),
mKeyFields(proc.mKeyFields),
mValueFieldName(proc.mValueFieldName),
mValueFieldIdx(proc.mValueFieldIdx)
{
}


AosDataProcCompose::~AosDataProcCompose()
{
}


bool
AosDataProcCompose::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Its format is:
	// 	<dataproc
	// 		zky_value_field_name1="xxx"
	// 		zky_value_field_name2="xxx"
	// 		shift1="xxx"
	// 		shift2="xxx">
	// 		<keyfields>
	// 			AOSTAG_KEYFIELDNAME="xxx"
	// 			AOSTAG_ADDTOKEY="xxx"
	// 			AOSTAG_SEPARATOR="xxx">
	// 			<AOSTAG_FILTER ...>
	// 				...
	// 			</AOSTAG_FILTER>
	// 		</keyfields>
	// 	</dataproc>
	aos_assert_r(def, false);
	if(isVersion1(def))
	{
		// Ketty 2014/01/11
		return configV1(def, rdata);	
	}

	bool rslt = AosDataProc::config(def, rdata);
	aos_assert_r(rslt, false);
	
	// Chen Ding, 2013/05/30
	AosXmlTagPtr keyfields_tag = def->getFirstChild("keyfields");
	if (keyfields_tag)
	{
		// It is the new format. 
		return configNew(rdata, def);
	}

	mValueFieldName = def->getAttrStr(AOSTAG_VALUEFIELDNAME);
	if (mValueFieldName == "")
	{
		mNeedDocid = true;
	}
	
	// 	<data_proc ...>
	// 		<keyfields>
	// 			<keyfield .../>
	// 			<keyfield .../>
	// 			...
	// 		</keyfields>
	//
	// 	</config>
	AosXmlTagPtr entry = def->getFirstChild();
	aos_assert_r(entry, false);

	OmnString key_field_name;
	bool addtokey, ignore_empty;
	OmnString sep;
	AosConditionObjPtr filter;
	AosXmlTagPtr tag;
	AosCondType::E type;
	while(entry)
	{
		key_field_name = entry->getAttrStr(AOSTAG_KEYFIELDNAME);
		aos_assert_r(key_field_name != "", false);

		addtokey = entry->getAttrBool(AOSTAG_ADDTOKEY, true);
		ignore_empty = entry->getAttrBool(AOSTAG_IGNORE_EMPTY, false);

		sep = entry->getAttrStr(AOSTAG_SEPARATOR);
		AosConvertAsciiBinary(sep);
		
		filter = 0;
		tag = entry->getFirstChild(AOSTAG_FILTER);
		if (tag)
		{
			type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
			if (AosCondType::isValid(type))
			{
				filter = AosConditionObj::getConditionStatic(tag, rdata);
			}
		}
			
		composeField cp;
		cp.mKeyFieldName = key_field_name;
		cp.mAddToKey = addtokey;
		cp.mIgnoreEmpty = ignore_empty;
		cp.mSep = sep;
		cp.mFilter = filter;
		
		mKeyFields.push_back(cp);	
		entry = def->getNextChild();
	}
	
	aos_assert_r(mKeyFields.size() > 0, false);
	return true;
}


bool
AosDataProcCompose::configNew(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// Its format is:
	// 	<dataproc
	// 		<key_fields>
	// 			<key_field
	// 				AOSTAG_KEYFIELDNAME="xxx"
	// 				AOSTAG_ADDTOKEY="xxx"
	// 				AOSTAG_SEPARATOR="xxx">
	// 				<AOSTAG_FILTER ...>
	// 					...
	// 				</AOSTAG_FILTER>
	// 			</key_field>
	// 			<key_field
	// 				AOSTAG_KEYFIELDNAME="xxx"
	// 				AOSTAG_ADDTOKEY="xxx"
	// 				AOSTAG_SEPARATOR="xxx">
	// 				<AOSTAG_FILTER ...>
	// 					...
	// 				</AOSTAG_FILTER>
	// 			</key_field>
	// 			...
	// 		</key_fields>
	// 		<value_fields>
	// 			<field field_name="xxx" shift="xxx">
	// 				<AOSTAG_FILTER ...>
	// 					...
	// 				</AOSTAG_FILTER>
	// 			</field>
	// 			<field field_name="xxx" shift="xxx">
	// 				<AOSTAG_FILTER ...>
	// 					...
	// 				</AOSTAG_FILTER>
	// 			</field>
	// 		</value_fields>
	// 	</dataproc>
	
	aos_assert_r(def, false);
/*	
	AosXmlTagPtr valuefields_tag = def->getFirstChild("value_fields");
	AosCondType::E type;
	if (valuefields_tag)
	{
		AosXmlTagPtr entry = valuefields_tag->getFirstChild();
		if (entry)
		{
			mValueFieldName1 = entry->getAttrStr("field_name");
			mShift1 = entry->getAttrInt("shift", 0);
			AosXmlTagPtr tag = entry->getFirstChild(AOSTAG_FILTER);
			if (tag)
			{
				type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
				if (AosCondType::isValid(type))
				{
					mValueFilter1 = AosConditionObj::getConditionStatic(tag, rdata);
				}
			}

			entry = valuefields_tag->getNextChild();
			if (entry)
			{
				mValueFieldName2 = entry->getAttrStr("field_name");
				mShift2 = entry->getAttrInt("shift", 0);
				tag = entry->getFirstChild(AOSTAG_FILTER);
				if (tag)
				{
					type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
					if (AosCondType::isValid(type))
					{
						mValueFilter2 = AosConditionObj::getConditionStatic(tag, rdata);
					}
				}
			}
		}
	}

	AosXmlTagPtr keyfields_tag = def->getFirstChild("key_fields");
	if (!keyfields_tag)
	{
		AosSetErrorU(rdata, "missing_keyfields") << def->toString() << enderr;
		return false;
	}
	
	AosXmlTagPtr entry = keyfields_tag->getFirstChild();
	aos_assert_r(entry, false);

	OmnString key_field_name;
	bool addtokey;
	OmnString sep;
	AosConditionObjPtr filter;
	AosXmlTagPtr tag;
	while(entry)
	{
		key_field_name = entry->getAttrStr(AOSTAG_KEYFIELDNAME);
		aos_assert_r(key_field_name != "", false);

		addtokey = entry->getAttrBool(AOSTAG_ADDTOKEY, true);
		sep = entry->getAttrStr(AOSTAG_SEPARATOR);
		AosConvertAsciiBinary(sep);
		
		filter = 0;
		tag = entry->getFirstChild(AOSTAG_FILTER);
		if (tag)
		{
			type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
			if (AosCondType::isValid(type))
			{
				filter = AosConditionObj::getConditionStatic(tag, rdata);
			}
		}
			
		composeField cp;
		cp.mKeyFieldName = key_field_name;
		cp.mAddToKey = addtokey;
		cp.mSep = sep;
		cp.mFilter = filter;
		
		mKeyFields.push_back(cp);	
		entry = keyfields_tag->getNextChild();
	}
	*/
	aos_assert_r(mKeyFields.size() > 0, false);
	return true;
}


AosDataProcStatus::E
AosDataProcCompose::procData(
		const AosDataRecordObjPtr &record,			
		const u64 &docid,
		const AosDataRecordObjPtr &output,			
		const AosRundataPtr &rdata)
{
	// This function composes an entry by two data elements:
	aos_assert_r(record, AosDataProcStatus::eError);
	aos_assert_r(mIILAssembler, AosDataProcStatus::eError);
	
	int size = mKeyFields.size();
	aos_assert_r(size > 0, AosDataProcStatus::eError);

	// 2. Retrieve the keys. 
	bool rslt = true;
	OmnString key, rslt_key;
	for(int i=0; i<size; i++)
	{
		AosValueRslt key_rslt;
		rslt = record->getFieldValue(
			mKeyFields[i].mKeyFieldIdx, key_rslt, false, rdata.getPtrNoLock());
		aos_assert_r(rslt, AosDataProcStatus::eError);

		if (mKeyFields[i].mFilter
			&& !mKeyFields[i].mFilter->evalCond(key_rslt, rdata))
		{
			return AosDataProcStatus::eContinue;
		}

		if (!mKeyFields[i].mAddToKey)
		{
			continue;
		}

		rslt_key = key_rslt.getStr();
		rslt_key.normalizeWhiteSpace(true, true);  // trim
		if (rslt_key == "" && !mKeyFields[i].mIgnoreEmpty)
		{
			rslt_key << '\b';
			//OmnScreen << "########## invalid rslt_key ! " << endl;
			//	return AosDataProcStatus::eContinue;
		}

		key << mKeyFields[i].mSep << rslt_key;
	}

	// Retrieve the value (i.e., docid). By default, the value is the docid.
	// If mValueFieldIdx1 is defined, it retrieves the value from the corresponding
	// field. If mShift1 is defined, it is shifted. 
	if (mValueFieldIdx >= 0)
	{
		AosValueRslt value_rslt;
		rslt = record->getFieldValue(mValueFieldIdx, value_rslt, false, rdata.getPtrNoLock());
		aos_assert_r(rslt, AosDataProcStatus::eError);
		
		rslt = value_rslt.getU64();
		if (!rslt)
		{
			return AosDataProcStatus::eContinue;
		}
	
		// Chen Ding, 2013/05/30
		if (mValueFilter && !mValueFilter->evalCond(value_rslt, rdata))
		{
			return AosDataProcStatus::eContinue;
		}
	}

	AosValueRslt value_rslt;
	OmnAlarm << enderr;
	//value_rslt.setKeyValue(key.data(), key.length(), false, value);

	rslt = mIILAssembler->appendEntry(value_rslt, rdata.getPtrNoLock());
	aos_assert_r(rslt, AosDataProcStatus::eError);

	return AosDataProcStatus::eContinue;
}


AosDataProcObjPtr
AosDataProcCompose::cloneProc()
{
	return OmnNew AosDataProcCompose(*this);
}


AosDataProcObjPtr
AosDataProcCompose::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcCompose * proc = OmnNew AosDataProcCompose(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}
 

bool
AosDataProcCompose::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	bool rslt = AosDataProc::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);
	
	aos_assert_r(record, false);
	int idx = -1;
	for(u32 i=0; i<mKeyFields.size(); i++)
	{
		idx = record->getFieldIdx(mKeyFields[i].mKeyFieldName, rdata.getPtrNoLock());
		//aos_assert_r(idx >= 0, false);
		if (idx < 0)
		{
			AosSetEntityError(rdata, "DataProcCompose_missing_key_fieldname_from_record", 
				"DataProc", "DataProcCompose") << mKeyFields[i].mKeyFieldName << enderr;
			return false;
		}
		mKeyFields[i].mKeyFieldIdx = idx;
	}
	
	if(mValueFieldName != "")
	{
		mValueFieldIdx = record->getFieldIdx(mValueFieldName, rdata.getPtrNoLock());
		aos_assert_r(mValueFieldIdx >= 0, false);
	}

	return true;
}


bool
AosDataProcCompose::configV1(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Ketty 2014/01/11
	// Its format is:
	//	<dataproc proc_id="compose" zky_sep="xxx">
	//      <input>
	//	  		<field zky_input_field_name="xxx.xxx"/>               
	//			...
	//	  		<field zky_input_field_name="xxx.xxx"/>
	//	    </input>
	//		<output zky_output_field_name="xxx.xxx" />
	//	</dataproc>
	aos_assert_r(def, false);
	
	bool rslt = configInputs(def, rdata);
	aos_assert_r(rslt, false);
	
	AosXmlTagPtr output_xml = def->getFirstChild(AOSTAG_OUTPUT);
	aos_assert_r(output_xml, false);

	AosXmlTagPtr output_field = output_xml->getFirstChild();
	aos_assert_r(output_field, false);

	OmnString name_field = output_field->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);
	aos_assert_r(name_field != "", false);
	mOutput.init(name_field, false);
	return true;
}


bool
AosDataProcCompose::configInputs(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr inputs_xml = def->getFirstChild(AOSTAG_INPUT);
	aos_assert_r(inputs_xml, false);
	
	bool addtokey, ignore_empty;
	OmnString name_field;
	OmnString sep;
	AosConditionObjPtr filter;
	AosXmlTagPtr tag;
	AosCondType::E type;
	AosXmlTagPtr field = inputs_xml->getFirstChild();
	aos_assert_r(field, false);
	while(field)
	{
		name_field = field->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
		aos_assert_r(name_field != "", false);
		
		addtokey = field->getAttrBool(AOSTAG_ADDTOKEY, true);
		ignore_empty = field->getAttrBool(AOSTAG_IGNORE_EMPTY, false);

		sep = field->getAttrStr(AOSTAG_SEPARATOR);
		AosConvertAsciiBinary(sep);
		
		filter = 0;
		tag = field->getFirstChild(AOSTAG_FILTER);
		if (tag)
		{
			type = AosCondType::toEnum(tag->getAttrStr(AOSTAG_ZKY_TYPE));
			if (AosCondType::isValid(type))
			{
				filter = AosConditionObj::getConditionStatic(tag, rdata);
			}
		}
		
		composeField cp;
		cp.mInput.init(name_field, true);
		cp.mAddToKey = addtokey;
		cp.mIgnoreEmpty = ignore_empty;
		cp.mSep = sep;
		cp.mFilter = filter;
		
		mKeyFields.push_back(cp);	
		field = inputs_xml->getNextChild();
	}
	
	aos_assert_r(mKeyFields.size() > 0, false);
	return true;
}


bool
AosDataProcCompose::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	for(u32 i=0; i<mKeyFields.size(); i++)
	{
		rf_infos.push_back(&(mKeyFields[i].mInput));	
	}
	
	rf_infos.push_back(&mOutput);
	return true;
}


AosDataProcStatus::E
AosDataProcCompose::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	// Ketty 2013/12/27
	int size = mKeyFields.size();
	aos_assert_r(size > 0, AosDataProcStatus::eError);

	bool rslt = true;
	OmnString key, rslt_key;
	AosDataRecordObj * input_record;
	for(int i=0; i<size; i++)
	{
		AosValueRslt key_rslt;
		input_record = getRecord(mKeyFields[i].mInput, input_records, output_records);
		aos_assert_r(input_record, AosDataProcStatus::eError);	
		
		rslt = input_record->getFieldValue(
			mKeyFields[i].mInput.mFieldIdx, key_rslt, false, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		//if (!key_rslt.isValid())
		//{
		//	return AosDataProcStatus::eRecordFiltered;	
		//}

		if (mKeyFields[i].mFilter
			&& !mKeyFields[i].mFilter->evalCond(key_rslt, rdata_raw))
		{
			return AosDataProcStatus::eContinue;
		}

		if (!mKeyFields[i].mAddToKey)
		{
			continue;
		}

		rslt_key = key_rslt.getStr();
		rslt_key.normalizeWhiteSpace(true, true);  // trim
		if (rslt_key == "" && !mKeyFields[i].mIgnoreEmpty)
		{
			rslt_key << '\b';
			OmnScreen << "########## invalid rslt_key ! " << endl;
			//return AosDataProcStatus::eContinue;
		}

		key << mKeyFields[i].mSep << rslt_key;
	}


	AosValueRslt output_key_value;
	output_key_value.setStr(key);

	aos_assert_r(mOutput.mRecordIdx >=0, AosDataProcStatus::eError);
	aos_assert_r(mOutput.mFieldIdx >=0, AosDataProcStatus::eError);
	AosDataRecordObj * output_record = getRecord(mOutput, input_records, output_records);
	aos_assert_r(output_record, AosDataProcStatus::eError);
	bool outofmem = false;
	output_record->setFieldValue(mOutput.mFieldIdx, output_key_value, outofmem, rdata_raw);
	
	return AosDataProcStatus::eContinue;
}


