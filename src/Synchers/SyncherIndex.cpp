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
// An Index Syncher does the following:
// 	For each input record:
// 		1. Update the IIL (i.e., add, delete, or modify)
// 		2. Collect the entry
// 	When finish the above loop, it constructs a SyncEvent:
// 		1. IILID
//	
// Modification History:
// 2015/09/20 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "Synchers/SyncherIndex.h"

#include "Synchers/SyncherIndexExecutor.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEUtil/SeTypes.h"
#include "Util/Buff.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataRecordBuff.h"
#include "Rundata/Rundata.h"
#include "SEUtil/SeTypes.h"
#include "JimoCall/Ptrs.h"
#include "JimoCall/JimoCall.h"
#include "JimoAPI/JimoSynchers.h"

#include <boost/make_shared.hpp>

using AosConf::DataRecordCtnr;
using AosConf::DataRecordBuff;
using boost::shared_ptr;
using boost::make_shared;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSyncherIndex(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSyncherIndex(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}
	catch (...)
	{
		AosLogError(rdata, false, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosSyncherIndex::AosSyncherIndex(const int version)
:
AosSyncher(version), 
mLock(OmnNew OmnMutex()),
mDeltaBeansBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin))
{
}


AosSyncherIndex::~AosSyncherIndex()
{
}


bool
AosSyncherIndex::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	//<index zky_iilname="_zt44_idx_t_idx_key_field2"  zky_tablename="t_idx" zky_objid="_zt4g_idxmgr_idx_t_idx_key_field2">
	//	<columns>
	//		<column zky_name="key_field2" type="str"/>
	//	</columns>
	//</index>
	
	aos_assert_r(def, false);
	mIILName = def->getAttrStr("zky_objid");
	aos_assert_r(mIILName != "", false);

	//input keys
	OmnString msg, keytype;
	AosXmlTagPtr columnsNode = def->getFirstChild("columns");
	aos_assert_r(columnsNode, false);
	AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");
	while (columnNode)
	{
		keytype = columnNode->getAttrStr("type");
		OmnString key = columnNode->getAttrStr("zky_name");
		key << ";";
		AosExprObjPtr expr = AosParseExpr(key, msg, rdata);
		aos_assert_r(expr, false);
		mInputKeys.push_back(expr);
		
		columnNode = columnsNode->getNextChild("column");
	}
	mIndexKeyDataType = AosDataFieldType::eStr;	
	if (mInputKeys.size() == 1)
	{
		aos_assert_r(AosDataFieldType::isValid(keytype), false);
		mIndexKeyDataType = AosDataFieldType::toEnum(keytype);
	}

	//input docid
	mInputDocid = AosParseExpr("docid;", msg, rdata);
	aos_assert_r(mInputDocid, false);

	//condition
	AosXmlTagPtr condNode = def->getFirstChild("cond");
	if (condNode)
	{
		OmnString condstr = condNode->getNodeText();
		condstr << ";";
		mCondition = AosParseExpr(condstr, msg, rdata);
		aos_assert_r(mCondition, false);
	}

	return createOutput(mIILName, rdata);
}


bool
AosSyncherIndex::createOutput(
		const OmnString &outputname,
		AosRundata *rdata)
{
	boost::shared_ptr<DataRecordCtnr> ctnr = boost::make_shared<DataRecordCtnr>();
	ctnr->setAttribute("zky_name", "output");

	boost::shared_ptr<DataRecordBuff> dr = boost::make_shared<DataRecordBuff>();
	dr->setAttribute("zky_name", outputname);
	dr->setField("zky_operator", "str", 50);
	dr->setField("key", "str", 50);
	dr->setField("docid", "bin_u64", sizeof(u64));
	ctnr->setRecord(dr);

	AosXmlTagPtr rcd_xml = AosXmlParser::parse(ctnr->getConfig() AosMemoryCheckerArgs);
	aos_assert_r(rcd_xml, false);
	mOutputRecord = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata AosMemoryCheckerArgs);
	aos_assert_r(mOutputRecord, false);

	return true;
}


bool 
AosSyncherIndex::proc()
{
	OmnScreen << "Processing Syncher: SyncherIndex" << endl;
	aos_assert_r(mRundata, false);
	
	AosDatasetObjPtr ds = getSyncherDataset(mRundata.getPtr(), mDatasetDef, mDeltasBuff);
	aos_assert_r(ds, false);

	return proc(mRundata.getPtr(), ds);
}


AosBuffPtr
AosSyncherIndex::serializeToBuff()
{
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	aos_assert_r(buff, 0);
	buff->setOmnStr(mIILName);
	buff->setOmnStr(mDatasetDef->toString());
	buff->setAosBuff(mDeltasBuff);
	
	return buff;
}


bool
AosSyncherIndex::serializeFromBuff(const AosBuffPtr &buff)
{
	mIILName = buff->getOmnStr("");
	aos_assert_r(mIILName != "", false);

	OmnString conf = buff->getOmnStr("");                          
	aos_assert_r(conf != "", false);                               
	mDatasetDef = AosXmlParser::parse(conf AosMemoryCheckerArgs);
	aos_assert_r(mDatasetDef, false);

	mDeltasBuff = buff->getAosBuff(true AosMemoryCheckerArgs);
	aos_assert_r(mDeltasBuff, false);

	return true;
}


AosJimoPtr 
AosSyncherIndex::cloneJimo() const
{
	return OmnNew AosSyncherIndex(*this);
}


bool	
AosSyncherIndex::proc(
		AosRundata *rdata, 
		const AosDatasetObjPtr &dataset)
{
	AosDataRecordObj *input_record;
	while (1)
	{
		bool rslt = dataset->nextRecord(rdata, input_record);
		aos_assert_r(rslt, false);
		if (!input_record) break;

		rslt = procData(rdata, input_record);
		aos_assert_r(rslt, false);
	}
		
	return true;
}


bool
AosSyncherIndex::procData(
		AosRundata *rdata, 
		AosDataRecordObj *input_record)	
{
	bool rslt, outofmem;
	AosValueRslt key_rslt, docid_rslt;

	// 1. get operator type 
	AosDeltaBeanOpr::E opr = input_record->getOperator();
	aos_assert_r(AosDeltaBeanOpr::isValid(opr), false);

	//condition
	if (mCondition)
	{
		AosValueRslt value_rslt;
		mCondition->getValue(rdata, input_record, value_rslt);
		rslt = value_rslt.getBool();
		if (!rslt) return true;
	}

	//key_values
	OmnString key_values = "";
	for (u32 i = 0; i < mInputKeys.size(); i++)
	{
		mInputKeys[i]->setIsParsedFieldIdx(false);
		rslt = mInputKeys[i]->getValue(rdata, input_record, key_rslt);
		aos_assert_r(rslt, false);
		
		if (i > 0) key_values << char(0x01);

		if (key_rslt.isNull())
		{
			key_values << '\b';		//key1\0x01\0x08
		}
		else 
		{
			key_values << key_rslt.getStr();		//key1\0x01key2
		}
	}
	if (key_values == "") return true;

	//get docid
	//mInputDocid->setIsParsedFieldIdx(false);
	//rslt = mInputDocid->getValue(rdata, input_record, docid_rslt);
	docid_rslt.setU64(0);
	if (!rslt) return false;

	//output record
	mOutputRecord->clear();
	AosDataRecordObj * output_record = mOutputRecord.getPtr();
	aos_assert_r(output_record, false);	

	AosValueRslt opr_v(AosDeltaBeanOpr::toStr(opr));
	rslt = output_record->setFieldValue(0, opr_v, outofmem, rdata);
	aos_assert_r(rslt && (!outofmem), false);

	AosValueRslt k_v(key_values);
	rslt = output_record->setFieldValue(1, k_v, outofmem, rdata);
	aos_assert_r(rslt && (!outofmem), false);

	AosValueRslt d_v(docid_rslt);
	rslt = output_record->setFieldValue(2, d_v, outofmem, rdata);
	aos_assert_r(rslt && (!outofmem), false);

	if (!k_v.isNull() && k_v.getStr() != "")
	{
		appendDeltaBean(rdata, output_record);
	}
	
	return true;
}


bool 
AosSyncherIndex::flushDeltaBeans(AosRundata *rdata)
{
	mLock->lock();

	if (mDeltaBeansBuff->dataLen() <= 0)
	{
		mLock->unlock();
		return true;
	}

	AosXmlTagPtr dsconf = generatorIndexDatasetConf(rdata);
	aos_assert_r(dsconf, false);

	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	buff->setOmnStr(mIILName);
	buff->setOmnStr(dsconf->toString());
	buff->setAosBuff(mDeltaBeansBuff);
	buff->reset();

	AosSyncherObj *sync = OmnNew AosSyncherIndexExecutor(0);
	sync->serializeFromBuff(buff);
	Jimo::jimoSendSyncher(rdata, sync);
	
	mDeltaBeansBuff->clear();
	mLock->unlock();

	return true;
}


bool 
AosSyncherIndex::appendDeltaBean(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	aos_assert_r(record, false);

	int recordlen = record->getRecordLen();
	AosBuff::encodeRecordBuffLength(recordlen);

	mLock->lock();
	mDeltaBeansBuff->setInt(recordlen);
	mDeltaBeansBuff->setBuff(record->getData(rdata), record->getRecordLen());
	mLock->unlock();

	return true;
}


AosXmlTagPtr
AosSyncherIndex::generatorIndexDatasetConf(AosRundata *rdata)
{
	OmnString str;
	str << "<dataset jimo_objid=\"dataset_syncher_jimodoc_v0\" zky_name=\"output\" >"
		<<  "<datarecord type=\"buff\" zky_name=\"output\">"
		<<   "<datafields>"
		<<    "<datafield type=\"str\" zky_name=\"zky_operator\"/>"
		<<    "<datafield type=\"str\" zky_name=\"key\"/>"
		<<    "<datafield type=\"bin_int64\" zky_name=\"docid\"/>"
		<<   "</datafields>"
		<<  "</datarecord>"
		<< "</dataset>";
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}

