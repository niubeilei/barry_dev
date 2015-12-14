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
// 05/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcDocIdToFields.h"

#include "API/AosApi.h"
#include <iostream>
#include <fstream>
using namespace std;

AosDataProcDocIdToFields::AosDataProcDocIdToFields(const bool flag)
:
AosDataProc(AOSDATAPROC_DOCIDTOFIELDS, AosDataProcId::eDocIdToFields, flag)
{
}
	

AosDataProcDocIdToFields::AosDataProcDocIdToFields(const AosDataProcDocIdToFields &proc)
:
AosDataProc(proc)
{
}


AosDataProcDocIdToFields::~AosDataProcDocIdToFields()
{
}

//save docids to a file
static OmnString docIdFile = "/tmp/docIds.txt"; 

//the 5 fields will use different swap pair
static int swapArray[5][2] = { {0,3}, {1,2}, {0,2}, {1,3}, {0,1} }; 

//
//this method is used to swap 2 bytes in a integer and return
//a new integer
//
u32
AosDataProcDocIdToFields::genFieldFunc(const u64 &docid, int swap1, int swap2)
{
	u8 *smallNumArray; 
	u8 smallNum;
	u32 num = 0;

	aos_assert_r(swap1 >= 0, false);
	aos_assert_r(swap1 <= 3, false);
	aos_assert_r(swap2 >= 0, false);
	aos_assert_r(swap2 <= 3, false);
	aos_assert_r(swap1 != swap2, false);

	num = docid & 0xFFFFFFFF;
	smallNumArray = (u8 *)&num;

	smallNum = smallNumArray[swap1];
	smallNumArray[swap1] = smallNumArray[swap2];
	smallNumArray[swap2] = smallNum;

	return num;
}

bool 
AosDataProcDocIdToFields::docIdToFile(u64 docid, OmnString fname)
{
	ofstream fDoc;

	fDoc.open(docIdFile.getBuffer(), ios::app | ios::out );
	fDoc << docid << "\n";
	fDoc.close();
	return true;
}

bool
AosDataProcDocIdToFields::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Its format is:
	// 	<dataproc proc_id="DocIdToFields">
	// 		<fields>
	//			<field zky_input_field_name="xxx.xxx" 
	//						zky_output_field_name="xxx.xxx" />
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


AosDataProcStatus::E 
AosDataProcDocIdToFields::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return AosDataProcStatus::eError;
}

//need to implement raw pointer version
AosDataProcStatus::E
AosDataProcDocIdToFields::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	bool rslt;
	AosValueRslt key_rslt;
	u32 val;
	int size = mFields.size();
	u64 docid;
	bool outofmem = false;

	AosDataRecordObj *output_record;

	//ignore input records
	//assume that there is only one output record
	output_record = *output_records;
	aos_assert_r(output_record, AosDataProcStatus::eError);
	docid = getFieldU64Value(rdata_raw, output_record, mOutputDocid.mFieldIdx); 
	aos_assert_r(docid != 0, AosDataProcStatus::eError);
	output_record->setDocid(docid);

	//save the doc id to a file. Temporary solution without
	//performance consideration
	docIdToFile(docid, docIdFile);	

	for(int i=0; i<size; i++)
	{
		val = genFieldFunc(docid, swapArray[i][0], swapArray[i][1]);
		key_rslt.setU64(val);

		rslt = output_record->setFieldValue(mFields[i].mOutput.mFieldIdx, key_rslt, outofmem, rdata_raw);
		if (!rslt) return AosDataProcStatus::eError;
	}

	return AosDataProcStatus::eContinue;
}


AosDataProcObjPtr
AosDataProcDocIdToFields::cloneProc()
{
	return OmnNew AosDataProcDocIdToFields(*this);
}


AosDataProcObjPtr
AosDataProcDocIdToFields::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcDocIdToFields * proc = OmnNew AosDataProcDocIdToFields(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}

bool
AosDataProcDocIdToFields::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

//need to implement raw pointer version
bool
AosDataProcDocIdToFields::resolveDataProc(
		const AosRundataPtr &rdata,
		const AosDataRecordObjPtr &input_record,
		const AosDataRecordObjPtr &output_record)
{
	OmnShouldNeverComeHere;
	return false;
}
