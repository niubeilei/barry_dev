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
// 04/01/2014 Created by Linda 
// 2014/08/12 Modified by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcStatDocShuffle.h"

//#include "DataStructs/RemoteShuffleStatId.h"
//#include "DataStructs/Vector2D.h"
#include "API/AosApi.h"
#include "Util/HashUtil.h"


AosDataProcStatDocShuffle::AosDataProcStatDocShuffle(const bool flag)
:
AosDataProc(AOSDATAPROC_STATDOCSHUFFLE, AosDataProcId::eStatDocShuffle, flag)
{
}
	

AosDataProcStatDocShuffle::AosDataProcStatDocShuffle(const AosDataProcStatDocShuffle &proc)
:
AosDataProc(proc)
{
}
	

AosDataProcStatDocShuffle::~AosDataProcStatDocShuffle()
{
}


bool
AosDataProcStatDocShuffle::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Its format is:
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);	
	
	bool rslt = initShuffleInfo(def);
	aos_assert_r(rslt, false);

	// stat_key
	OmnString key_output_rcdname;
	rslt = configStatKeyField(def, key_output_rcdname);
	aos_assert_r(rslt, false);

	// stat_time
	OmnString time_output_rcdname;
	//rslt = configStatTimeField(def, time_output_rcdname);
	rslt = configStatTimesField(def, time_output_rcdname);
	aos_assert_r(rslt, false);

	// stat_value
	OmnString value_output_rcdname;
	rslt = configStatValuesField(def, value_output_rcdname);
	aos_assert_r(rslt, false);
	
	if(time_output_rcdname != "") aos_assert_r(key_output_rcdname == time_output_rcdname, false);
	aos_assert_r(key_output_rcdname == value_output_rcdname, false);
	
	OmnString id_output_rcdname;
	//if(mShuffleByCube)
	//{
	//	rslt = configStatDocidField(def, id_output_rcdname);
	//}
	//else
	//{
		rslt = configStatCubeIdField(def, id_output_rcdname);
	//}
	aos_assert_r(rslt, false);
	aos_assert_r(key_output_rcdname == id_output_rcdname, false);

	return true;
}


bool
AosDataProcStatDocShuffle::initShuffleInfo(const AosXmlTagPtr &conf)
{
	OmnString shuffle_type = conf->getAttrStr("shuffle_type", "");

	int shuff_num = -1;
	if(shuffle_type == "cube")
	{
		shuff_num = AosGetNumCubes();
		mShuffleByCube = true;
	}
	else
	{
		aos_assert_r(shuffle_type=="physical", false);
		shuff_num = AosGetNumPhysicals();
		mShuffleByCube = false;
	}
	
	aos_assert_r(shuff_num > 0, false);
	for(int i = 0; i < shuff_num; i++)
	{
		OutputInfo info;
		mShuffleInfo.push_back(info);
	}
	return true;
}


bool
AosDataProcStatDocShuffle::configStatKeyField(
		const AosXmlTagPtr &conf,
		OmnString &key_output_rcdname)
{
	AosXmlTagPtr key_conf = conf->getFirstChild("stat_key");
	aos_assert_r(key_conf, false);

	OmnString input_name_field = key_conf->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	OmnString output_name_field = key_conf->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);

	aos_assert_r(input_name_field != "", false);
	mStatKeyInput.init(input_name_field, true);

	aos_assert_r(output_name_field != "", false);
	RecordFieldInfo statKeyOutput;
	statKeyOutput.init(output_name_field, false);
		
	key_output_rcdname = statKeyOutput.mRecordName;
	for(u32 i=0; i<mShuffleInfo.size(); i++)
	{
		mShuffleInfo[i].mStatKeyOutput = statKeyOutput; 
		mShuffleInfo[i].mStatKeyOutput.mRecordName << "_" << i; 
	}
	return true;
}


/*
bool
AosDataProcStatDocShuffle::configStatTimeField(
		const AosXmlTagPtr &conf,
		OmnString &time_output_rcdname)
{
	AosXmlTagPtr time_conf = conf->getFirstChild("stat_time");
	aos_assert_r(time_conf, false);

	OmnString input_name_field = time_conf->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	OmnString output_name_field = time_conf->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);

	aos_assert_r(input_name_field != "", false);
	mStatTimeInput.init(input_name_field, true);

	aos_assert_r(output_name_field != "", false);
	RecordFieldInfo statTimeOutput;
	statTimeOutput.init(output_name_field, false);
	
	time_output_rcdname = statTimeOutput.mRecordName;
	for(u32 i=0; i<mShuffleInfo.size(); i++)
	{
		mShuffleInfo[i].mStatTimeOutput = statTimeOutput; 
		mShuffleInfo[i].mStatTimeOutput.mRecordName << "_" << i; 
	}

	return true;	
}
*/

bool
AosDataProcStatDocShuffle::configStatTimesField(
		const AosXmlTagPtr &conf,
		OmnString &time_output_rcdname)
{
	AosXmlTagPtr times_conf = conf->getFirstChild("stat_times");
	aos_assert_r(times_conf, false);
	
	vector<RecordFieldInfo> statTimeOutputs;
	OmnString input_name_field, output_name_field;
	
	AosXmlTagPtr each_conf = times_conf->getFirstChild("stat_time"); 
	while(each_conf)
	{
		input_name_field = each_conf->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
		output_name_field = each_conf->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);

		aos_assert_r(input_name_field != "", false);
		RecordFieldInfo statTimeInput;
		statTimeInput.init(input_name_field, true);
		mStatTimeInputs.push_back(statTimeInput);

		aos_assert_r(output_name_field != "", false);
		RecordFieldInfo statTimeOutput;
		statTimeOutput.init(output_name_field, false);
		statTimeOutputs.push_back(statTimeOutput);
		
		if(time_output_rcdname == "")
		{
			time_output_rcdname = statTimeOutput.mRecordName;
		}
		else
		{
			aos_assert_r(time_output_rcdname == statTimeOutput.mRecordName, false);
		}

		each_conf = times_conf->getNextChild("stat_value");
	}

	for(u32 i=0; i<mShuffleInfo.size(); i++)
	{
		mShuffleInfo[i].mStatTimeOutputs = statTimeOutputs; 
		for (size_t j = 0; j < statTimeOutputs.size(); j++)
		{
			mShuffleInfo[i].mStatTimeOutputs[j].mRecordName << "_" << i;
		}
	}

	return true;	
}


bool
AosDataProcStatDocShuffle::configStatValuesField(
		const AosXmlTagPtr &conf,
		OmnString &value_output_rcdname)
{
	AosXmlTagPtr values_conf = conf->getFirstChild("stat_values");
	aos_assert_r(values_conf, false);

	AosXmlTagPtr value_xml = values_conf->getFirstChild("stat_value");
	vector<RecordFieldInfo> statValueOutputs;
	OmnString input_name_field, output_name_field;
	while(value_xml)
	{
		input_name_field = value_xml->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
		output_name_field = value_xml->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);

		aos_assert_r(input_name_field != "", false);
		RecordFieldInfo statValueInput;
		statValueInput.init(input_name_field, true);
		mStatValueInputs.push_back(statValueInput);

		aos_assert_r(output_name_field != "", false);
		RecordFieldInfo statValueOutput;
		statValueOutput.init(output_name_field, false);
		statValueOutputs.push_back(statValueOutput);
		
		if(value_output_rcdname == "")
		{
			value_output_rcdname = statValueOutput.mRecordName;
		}
		else
		{
			aos_assert_r(value_output_rcdname == statValueOutput.mRecordName, false);
		}

		value_xml = values_conf->getNextChild("stat_value");
	}
	
	for(u32 i=0; i<mShuffleInfo.size(); i++)
	{
		mShuffleInfo[i].mStatValueOutputs = statValueOutputs;
		for (size_t j = 0; j < statValueOutputs.size(); j++)
		{
			mShuffleInfo[i].mStatValueOutputs[j].mRecordName << "_" << i;
		}
	}
	return true;
}

bool
AosDataProcStatDocShuffle::configStatDocidField(
		const AosXmlTagPtr &conf,
		OmnString &id_output_rcdname)
{
	AosXmlTagPtr id_conf = conf->getFirstChild("stat_docid");
	aos_assert_r(id_conf, false);

	OmnString input_name_field = id_conf->getAttrStr(AOSTAG_INPUT_FIELD_NAME);
	OmnString output_name_field = id_conf->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);

	aos_assert_r(input_name_field != "", false);
	mStatDocidInput.init(input_name_field, true);

	aos_assert_r(output_name_field != "", false);
	RecordFieldInfo statDocidOutput;
	statDocidOutput.init(output_name_field, false);
		
	id_output_rcdname = statDocidOutput.mRecordName;
	for(u32 i=0; i<mShuffleInfo.size(); i++)
	{
		mShuffleInfo[i].mIdOutput = statDocidOutput; 
		mShuffleInfo[i].mIdOutput.mRecordName << "_" << i; 
	}
	return true;
}


bool
AosDataProcStatDocShuffle::configStatCubeIdField(
		const AosXmlTagPtr &conf,
		OmnString &id_output_rcdname)
{
	AosXmlTagPtr id_conf = conf->getFirstChild("stat_cubeid");
	aos_assert_r(id_conf, false);

	OmnString output_name_field = id_conf->getAttrStr(AOSTAG_OUTPUT_FIELD_NAME);

	aos_assert_r(output_name_field != "", false);
	RecordFieldInfo cubeIdOutput;
	cubeIdOutput.init(output_name_field, false);

	id_output_rcdname = cubeIdOutput.mRecordName;
	for(u32 i=0; i<mShuffleInfo.size(); i++)
	{
		mShuffleInfo[i].mIdOutput = cubeIdOutput; 
		mShuffleInfo[i].mIdOutput.mRecordName << "_" << i; 
	}
	return true;
}


AosDataProcStatus::E
AosDataProcStatDocShuffle::procData(
		const AosDataRecordObjPtr &record,			
		const u64 &docid,
		const AosDataRecordObjPtr &output,			
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return AosDataProcStatus::eError;
}


AosDataProcStatus::E
AosDataProcStatDocShuffle::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	bool rslt, outofmem = false;

	AosValueRslt statkey_rslt, id_rslt;
	AosDataRecordObj * input_record, *output_record;
	
	// get stat_key
	input_record = getRecord(
		mStatKeyInput, input_records, output_records);
	aos_assert_r(input_record, AosDataProcStatus::eError);

	rslt = input_record->getFieldValue(
		mStatKeyInput.mFieldIdx, statkey_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
		
	if (statkey_rslt.isNull())
	{	
		//output_record->clear();
		return AosDataProcStatus::eContinue;
	}

	int shuffle_id = -1;
	
	//if(mShuffleByCube)
	//{
	//	// shuffle by stat docid
	//	// get stat docid
	//	input_record = getRecord(
	//		mStatDocidInput, input_records, output_records);
	//	aos_assert_r(input_record, AosDataProcStatus::eError);
	//
	//	rslt = input_record->getFieldValue(
	//		mStatDocidInput.mFieldIdx, id_rslt, false, rdata_raw);
	//	aos_assert_r(rslt, AosDataProcStatus::eError);
	//
	//	u64 stat_docid;
	//	id_rslt.getU64();
	//	
	//	shuffle_id = stat_docid >> 32; 
	//}
	//else
	//{
		// shuffle by key.
		int cube_id = 0;//getCubeIdByStatKey(statkey_rslt.getStrValueRef());
		aos_assert_r(cube_id >=0, AosDataProcStatus::eError);
		id_rslt.setU64((u64)cube_id);
		
		//shuffle_id = AosGetPhysicalIdByCubeId(cube_id);
		//shuffle_id = AosGetNextSvrId(cube_id, -1);
		shuffle_id = cube_id;
	//}
	aos_assert_r(shuffle_id>= 0, AosDataProcStatus::eError);
	
	output_record = getRecord(
		mShuffleInfo[shuffle_id].mStatKeyOutput, input_records, output_records);
	aos_assert_r(output_record, AosDataProcStatus::eError);
	
	// set key
	rslt = output_record->setFieldValue(
		mShuffleInfo[shuffle_id].mStatKeyOutput.mFieldIdx, 
		statkey_rslt, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;

	// set id (stat_docid or cube_id)
	rslt = output_record->setFieldValue(
		mShuffleInfo[shuffle_id].mIdOutput.mFieldIdx, 
		id_rslt, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;

	// get and set time 
	/*
	AosValueRslt stattime_rslt;
	input_record = getRecord(
		mStatTimeInput, input_records, output_records);
	aos_assert_r(input_record, AosDataProcStatus::eError);
	
	rslt = input_record->getFieldValue(
		mStatTimeInput.mFieldIdx, stattime_rslt, false, rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	rslt = output_record->setFieldValue(
		mShuffleInfo[shuffle_id].mStatTimeOutput.mFieldIdx, 
		stattime_rslt, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;
	*/
	AosValueRslt stattime_rslt;
	for (size_t i = 0; i < mStatTimeInputs.size(); i++)
	{
		input_record = getRecord(
			mStatTimeInputs[i], input_records, output_records);
		aos_assert_r(input_record, AosDataProcStatus::eError);
		
		rslt = input_record->getFieldValue(
			mStatTimeInputs[i].mFieldIdx, stattime_rslt, false, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		if (stattime_rslt.isNull())
		{
			output_record->clear();
			return AosDataProcStatus::eContinue;
		}

		rslt = output_record->setFieldValue(
				mShuffleInfo[shuffle_id].mStatTimeOutputs[i].mFieldIdx,
				stattime_rslt, outofmem, rdata_raw);
		if (!rslt) 
		{
			return AosDataProcStatus::eError;
		}
	}
	

	// get and set stat value.
	// Ketty Print.
	/*
	OmnString print_str = "Ketty output!! ";
	OmnString key_vv = statkey_rslt.getStr();
	u64 id_vv;
	id_rslt.getU64();
	u64 time_vv;
	stattime_rslt.getU64();
	print_str << "  stat_key: " << key_vv << "; "
		<< "  cube_id /stat_id: " << id_vv << "; "
		<< "  stat_time: " << time_vv << "; "
		<< "  stat_values: ";
	*/

	AosValueRslt statvalue_rslt;
	for (size_t i = 0; i < mStatValueInputs.size(); i++)
	{
		input_record = getRecord(
			mStatValueInputs[i], input_records, output_records);
		aos_assert_r(input_record, AosDataProcStatus::eError);

		rslt = input_record->getFieldValue(
			mStatValueInputs[i].mFieldIdx, statvalue_rslt, false, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		if (statvalue_rslt.isNull()) {
			statvalue_rslt.setU64((u64)0);
		}

		/*
		if (statvalue_rslt.isNull())
		{	
			output_record->clear();
			return AosDataProcStatus::eContinue;
		}
		*/

		rslt = output_record->setFieldValue(
				mShuffleInfo[shuffle_id].mStatValueOutputs[i].mFieldIdx,
				statvalue_rslt, outofmem, rdata_raw);
		if (!rslt) 
		{
			return AosDataProcStatus::eError;
		}
	
		// Ketty Print.
		//u64 vv;
		//statvalue_rslt.getU64();
		//print_str << vv << "; ";	
	}
	//OmnScreen << print_str << endl;

	return AosDataProcStatus::eContinue;
}


AosDataProcObjPtr
AosDataProcStatDocShuffle::cloneProc()
{
	return OmnNew AosDataProcStatDocShuffle(*this);
}


AosDataProcObjPtr
AosDataProcStatDocShuffle::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcStatDocShuffle * proc = OmnNew AosDataProcStatDocShuffle(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}
 

bool
AosDataProcStatDocShuffle::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosDataProcStatDocShuffle::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	rf_infos.push_back(&mStatKeyInput);
	//rf_infos.push_back(&mStatTimeInput);
	for (size_t i = 0; i< mStatTimeInputs.size(); i++)
	{
		rf_infos.push_back(&mStatTimeInputs[i]);
	}
	
	for (size_t i = 0; i< mStatValueInputs.size(); i++)
	{
		rf_infos.push_back(&mStatValueInputs[i]);
	}

	for (size_t i = 0; i < mShuffleInfo.size(); i++)
	{
		rf_infos.push_back(&(mShuffleInfo[i].mIdOutput));
		rf_infos.push_back(&(mShuffleInfo[i].mStatKeyOutput));
		//rf_infos.push_back(&(mShuffleInfo[i].mStatTimeOutput));
		
		for (size_t j = 0; j < mShuffleInfo[i].mStatTimeOutputs.size(); j++)
		{
			rf_infos.push_back(&(mShuffleInfo[i].mStatTimeOutputs[j]));
		}

		for (size_t j = 0; j < mShuffleInfo[i].mStatValueOutputs.size(); j++)
		{
			rf_infos.push_back(&(mShuffleInfo[i].mStatValueOutputs[j]));
		}
	}
	return true;
}


bool 
AosDataProcStatDocShuffle::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	OmnScreen << "CCCCCCCCCCC AosDataProcStatDocShuffle AosDataProcStatDocShuffle finish; " << endl;
	//map<int, u64> bsnap_ids;
	//aos_assert_r(mShuffle.size() > 0, false);
	//for (size_t i = 0; i < mShuffle.size(); i++)
	//{
	//	mShuffle[i]->saveControlDoc(rdata, bsnap_ids);
	//}
	return true;
}

int
AosDataProcStatDocShuffle::getCubeIdByStatKey(const OmnString &stat_key)
{
	aos_assert_r(stat_key != "", -1);	
	u32 hash_key = AosGetHashKey(stat_key);

	int cube_num = AosGetNumCubes();
	aos_assert_r(cube_num > 0, -1);
	u32 cube_id =  hash_key % cube_num;
	return cube_id;
}


