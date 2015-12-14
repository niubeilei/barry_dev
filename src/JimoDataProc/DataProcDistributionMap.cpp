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
// 2015/02/11 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcDistributionMap.h"
#include "DataProc/DataProc.h"

#include "API/AosApi.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcDistributionMap_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcDistributionMap(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosDataProcDistributionMap::AosDataProcDistributionMap(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcDistributionMap)
{
}


AosDataProcDistributionMap::AosDataProcDistributionMap(const AosDataProcDistributionMap &proc)
:
AosJimoDataProc(0, AosJimoType::eDataProcDistributionMap)
{
	if (proc.mInputKey)
	{
		mInputKey = proc.mInputKey->cloneExpr();
	}
	mCurrentRecordSize = proc.mCurrentRecordSize;
	mMaxRecordSize = proc.mMaxRecordSize;
	mSplitSize = proc.mSplitSize;
	mType = proc.mType;
	mIILName = proc.mIILName;
	mFieldValues = proc.mFieldValues;
	mPositionMap = proc.mPositionMap;
}


AosDataProcDistributionMap::~AosDataProcDistributionMap()
{
}


bool
AosDataProcDistributionMap::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1">
	//	<![CDATA[
	//			{
	//			  "type": "distributionmap",
	//			  "key_field": "key_field2",
	//			  "num":10000,
	//			  "split":4,
	//			  "map_name":"_zt44_map_t1_map01"
	//			}
	//			]]>
	//</data_prco>

	aos_assert_r(def, false);
	mCurrentRecordSize = 0;
	mType =  AosDataType::eInvalid;

	mName = def->getAttrStr(AOSTAG_NAME, "");
	aos_assert_r(mName != "", false);

	OmnString dp_json = def->getNodeText();
	aos_assert_r(dp_json != "", false);

	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_json, json);
	aos_assert_r(rslt, false);

	//input key
	OmnString msg;
	OmnString inputKey;
	inputKey = json["key_field"].asString("");
	aos_assert_r(inputKey != "", false);

	inputKey << ";";                                   
	mInputKey = AosParseExpr(inputKey, msg, rdata.getPtr());
	if (!mInputKey)
	{                                                
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: "
			<< "" << enderr;
		OmnAlarm << inputKey << enderr;
		return false;
	}

	mMaxRecordSize = json["num"].asInt(0);
	aos_assert_r(mMaxRecordSize > 0 , false);

	mSplitSize = json["split"].asInt(0);
	aos_assert_r(mSplitSize > 0 , false);

	mIILName = json["map_name"].asString("");
	aos_assert_r(mIILName != "" , false);

	return true;
}


AosDataProcStatus::E
AosDataProcDistributionMap::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	if(mCurrentRecordSize >= mMaxRecordSize) return AosDataProcStatus::eExit;	
	
	bool rslt;
	AosValueRslt key_rslt, value_rslt, docid_rslt;
	AosDataRecordObj *input_record; // *output_record;

	//input_record
	input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);	

	//get key
	OmnString key_values = "";
	rslt = mInputKey->getValue(rdata_raw, input_record, key_rslt);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	if(mCurrentRecordSize == 0)
	{
		mType = key_rslt.getType();
	}
	aos_assert_r(key_rslt.getType() == mType, AosDataProcStatus::eError);
	if (key_rslt.isNull())
	{
		return AosDataProcStatus::eContinue;
	}
	mFieldValues.push_back(key_rslt);

	mCurrentRecordSize++;
	return AosDataProcStatus::eContinue;
}


AosJimoPtr 
AosDataProcDistributionMap::cloneJimo() const
{
	return OmnNew AosDataProcDistributionMap(*this);
}


AosDataProcObjPtr                                        
AosDataProcDistributionMap::cloneProc()
{
	return OmnNew AosDataProcDistributionMap(*this);
}


bool 
AosDataProcDistributionMap::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	string objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	string dp_name = dpname.data();
	string dp_str;
	dp_str = "<jimodataproc " + string(AOSTAG_CTNR_PUBLIC) + "=\"true\" ";
	dp_str +=  string(AOSTAG_PUBLIC_DOC) + "=\"true\" " + string(AOSTAG_OBJID) + "=\"" + objid + "\" ";
	dp_str += "><dataproc zky_name=\"";
	dp_str += dp_name + "\" jimo_objid=\"dataprocdistributionmap_jimodoc_v0\">";
	dp_str += "<![CDATA[" + string(jsonstr) + "]]></dataproc>";
	dp_str += "</jimodataproc>";

	//AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dp_str);
	//aos_assert_r(dp_xml, false);

	//dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//dp_xml->setAttr(AOSTAG_OBJID, AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname));
	//return AosCreateDoc(dp_xml->toString(), true, rdata);
	
	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}


bool
AosDataProcDistributionMap::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	//sort the elements in vector
		
	AosDataProcDistributionMap* proc = static_cast<AosDataProcDistributionMap*>(procs[0].getPtr());
	std::sort(proc->mFieldValues.begin(), proc->mFieldValues.end());
	aos_assert_r(proc->mCurrentRecordSize == (i64)proc->mFieldValues.size(), false);
	
	aos_assert_r(proc->mSplitSize > 0, false);
	int avgSize = proc->mCurrentRecordSize/proc->mSplitSize;
	OmnString value;
	u64 id;
	bool rslt;
	i64 recordLen = 0;
	i64 sigleRecordLen = 0;
	for(int i = 0; i < proc->mSplitSize - 1; i++)
	{
		u32 pos = avgSize*(1+i);
		aos_assert_r(pos < proc->mFieldValues.size(), false);

		value = proc->mFieldValues[pos].getStr();
		sigleRecordLen = value.length();
		if(recordLen < sigleRecordLen) recordLen = sigleRecordLen;
		proc->mPositionMap[value] = (u64)i;
	}
	recordLen = recordLen + 8 + 1; //one size for \0

	//set map to buff
	AosBuffPtr buff = OmnNew AosBuff(recordLen*(proc->mSplitSize - 1) AosMemoryCheckerArgs);	
	AosBuff* mBuff = buff.getPtr();
	std::map<OmnString, u64>::iterator itr = proc->mPositionMap.begin(); 
	const char * data;

	while(itr != proc->mPositionMap.end())
	{
		value = itr->first;
		id = itr->second;
		data = value.data() + '\0';
		mBuff->setBuff(data, recordLen - 8);
		mBuff->setU64(id);
		itr++;
	}
	
	rslt = AosStrBatchAdd(
				proc->mIILName, recordLen, buff, 0, 0, mTaskDocid,
				rdata);
	aos_assert_r(rslt, false);

	return true;
}
