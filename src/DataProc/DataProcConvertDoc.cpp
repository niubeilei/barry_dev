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
// 2013/12/23 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcConvertDoc.h"

#include "API/AosApi.h"

AosDataProcConvertDoc::AosDataProcConvertDoc(const bool flag)
:
AosDataProc(AOSDATAPROC_CONVERTDOC, AosDataProcId::eConvertDoc, flag)
{
}


AosDataProcConvertDoc::AosDataProcConvertDoc(const AosDataProcConvertDoc &proc)
:
AosDataProc(proc)
{
	//if (proc.mFilter) mFilter = proc.mFilter->clone();
}


AosDataProcConvertDoc::~AosDataProcConvertDoc()
{
}


AosDataProcStatus::E 
AosDataProcConvertDoc::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return AosDataProcStatus::eError;
}


bool
AosDataProcConvertDoc::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

AosDataProcObjPtr
AosDataProcConvertDoc::cloneProc()
{
	return OmnNew AosDataProcConvertDoc(*this);
}


AosDataProcObjPtr
AosDataProcConvertDoc::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcConvertDoc * proc = OmnNew AosDataProcConvertDoc(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcConvertDoc::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Its format is:
	// 	<dataproc proc_id="convertdoc">
	// 		<fields>
	//			<field zky_input_field_name="xxx.xxx" zky_sep="xxx" />
	//			...
	//		</fields>
	// 	</dataproc>
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);
	
	AosXmlTagPtr fields = def->getFirstChild("fields");
	aos_assert_r(fields, false);

	AosXmlTagPtr field_xml = fields->getFirstChild();
	aos_assert_r(field_xml, false);
	OmnString input_name_field, output_name_field;
	OmnString output_rcd_name;
	while(field_xml)
	{
		input_name_field = field_xml->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
		aos_assert_r(input_name_field != "", false);
		
		output_name_field = field_xml->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);
		aos_assert_r(output_name_field != "", false);

		Field field;
		field.mInput.init(input_name_field, true);
		field.mOutput.init(output_name_field, false);
		mFields.push_back(field);
		
		if(output_rcd_name == "")
		{
			output_rcd_name = field.mOutput.mRecordName; 
		}
		else
		{
			aos_assert_r(field.mOutput.mRecordName == output_rcd_name, false);
		}

		field_xml = fields->getNextChild();
	}
	
	mOutputDocid.init(output_rcd_name, AOSTAG_DOCID, false);

	return true;
}

bool
AosDataProcConvertDoc::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	for(u32 i=0; i<mFields.size(); i++)
	{
		rf_infos.push_back(&(mFields[i].mInput));
		rf_infos.push_back(&(mFields[i].mOutput));
	}
	rf_infos.push_back(&mOutputDocid);
	return true;
}


AosDataProcStatus::E
AosDataProcConvertDoc::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	bool rslt;
	int size = mFields.size();
	AosDataRecordObj * input_record;
	AosDataRecordObj * output_record;
	for(int i=0; i<size; i++)
	{
		input_record = getRecord(mFields[i].mInput, input_records, output_records);
		aos_assert_r(input_record, AosDataProcStatus::eError);	
		
		AosValueRslt key_rslt;
		rslt = input_record->getFieldValue(mFields[i].mInput.mFieldIdx, key_rslt, false, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		
		output_record = getRecord(mFields[i].mOutput, input_records, output_records);
		aos_assert_r(output_record, AosDataProcStatus::eError);	
		
		bool outofmem = false;
		rslt = output_record->setFieldValue(mFields[i].mOutput.mFieldIdx, key_rslt, outofmem, rdata_raw);
		if (!rslt) return AosDataProcStatus::eError;
	}

	// this convertDoc need the docid.
	aos_assert_r(output_record, AosDataProcStatus::eError);
	u64 docid = getFieldU64Value(rdata_raw, output_record, mOutputDocid.mFieldIdx); 
	aos_assert_r(docid != 0, AosDataProcStatus::eError);
	output_record->setDocid(docid);

	return AosDataProcStatus::eContinue;
}


