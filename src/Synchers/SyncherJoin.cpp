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
#include "Synchers/SyncherJoin.h"
#include "Synchers/SyncherStat.h"

#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEUtil/SeTypes.h"
#include "IILMgr/IIL.h"
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
#include "JQLExpr/ExprMemberOpt.h"
#include "JQLStatement/JqlStatement.h"
#include "AosConf/DataRecord.h"
#include "AosConf/DataField.h"

#include <boost/make_shared.hpp>

using AosConf::DataRecordCtnr;
using AosConf::DataRecordBuff;
using AosConf::DataRecord;
using AosConf::DataField;
using boost::shared_ptr;
using boost::make_shared;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSyncherJoin(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSyncherJoin(version);
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


AosSyncherJoin::AosSyncherJoin(const int version)
:
AosSyncher(version), 
mLock(OmnNew OmnMutex()),
mDeltaBeansBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin))
{
}


AosSyncherJoin::~AosSyncherJoin()
{
}


bool
AosSyncherJoin::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	// <join zky_database="...">
	// 	<jointable>
	// 		<column>t1</column>
	// 		<column>t2</column>
	// 	</jointable>
	// 	<joinindex>
	// 		<column>...</column>
	// 		<column>...</column>
	// 	</joinindex>
	// 	<jointype>inner_join</jointype>
	//	<cond> ...	</cond>
	// </join>

	aos_assert_r(def, false);

	// hard code
	//mDBName = def->getAttrStr("zky_database");
	mCrtTableObjid = def->getAttrStr("zky_table_objid");

	// Join table
	AosXmlTagPtr jointableNode = def->getFirstChild("jointable");
	aos_assert_r(jointableNode, false);
	AosXmlTagPtr columnNode = jointableNode->getFirstChild("column");
	while (columnNode)
	{
		OmnString tablename = columnNode->getNodeText();
		mJoinTables.push_back(tablename);
		columnNode = jointableNode->getNextChild("column");
	}

	// Join indexs
	AosXmlTagPtr joinindexNode = def->getFirstChild("joinindex");
	aos_assert_r(joinindexNode, false);
	columnNode = joinindexNode->getFirstChild("column");
	while (columnNode)
	{
		OmnString idxname = columnNode->getNodeText();
		mJoinIndexs.push_back(idxname);
		columnNode = joinindexNode->getNextChild("column");
	}

	// Join type
	AosXmlTagPtr jointypeNode = def->getFirstChild("jointype");
	mJoinType = jointypeNode->getNodeText();	
	
	//condition
	OmnString msg = "";
	AosXmlTagPtr condNode = def->getFirstChild("cond");
	if (condNode)
	{
		OmnString condstr = condNode->getNodeText();
		condstr << ";";
		mCondition = AosParseExpr(condstr, msg, rdata);
		aos_assert_r(mCondition, false);
	}

	return true;
}


bool
AosSyncherJoin::createOutput(
		AosRundata *rdata, 
		const AosXmlTagPtr &l_rcddoc, 
		const AosXmlTagPtr &r_rcddoc)
{
	aos_assert_r(l_rcddoc && r_rcddoc, false);

	boost::shared_ptr<DataRecordCtnr> ctnr = boost::make_shared<DataRecordCtnr>();
	ctnr->setAttribute("zky_name", "output");
	boost::shared_ptr<DataRecordBuff> dr = boost::make_shared<DataRecordBuff>();
	dr->setAttribute("zky_name", "output_join");

	vector<boost::shared_ptr<DataField> > l_fields = getFields(rdata, l_rcddoc);
	vector<boost::shared_ptr<DataField> > r_fields = getFields(rdata, r_rcddoc);
	r_fields.insert(r_fields.begin(), l_fields.begin(), l_fields.end());
	for (size_t i=0; i<r_fields.size(); i++)
	{
		dr->setField(r_fields[i]);			
	}
	ctnr->setRecord(dr);

	AosXmlTagPtr rcd_xml = AosXmlParser::parse(ctnr->getConfig() AosMemoryCheckerArgs);
	aos_assert_r(rcd_xml, false);
	mOutputRecord = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata AosMemoryCheckerArgs);			
	aos_assert_r(mOutputRecord, false);

	return true;
}


bool 
AosSyncherJoin::proc()
{
	OmnScreen << "Processing Syncher: SyncherJoin" << endl;
	aos_assert_r(mRundata, false);
	
	AosDatasetObjPtr ds = getSyncherDataset(mRundata.getPtr(), mDatasetDef, mDeltasBuff);
	aos_assert_r(ds, false);

	return proc(mRundata.getPtr(), ds);
}


AosBuffPtr
AosSyncherJoin::serializeToBuff()
{
	OmnShouldNeverComeHere;
	return NULL;
}


bool
AosSyncherJoin::serializeFromBuff(const AosBuffPtr &buff)
{
	OmnShouldNeverComeHere;
	return false;
}


AosJimoPtr 
AosSyncherJoin::cloneJimo() const
{
	return OmnNew AosSyncherJoin(*this);
}


bool	
AosSyncherJoin::proc(
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
AosSyncherJoin::procData(
		AosRundata *rdata, 
		AosDataRecordObj *input_record)
{
	AosValueRslt key_rslt, docid_rslt;

	// 1. get operator type 
	AosDeltaBeanOpr::E opr = input_record->getOperator();
	aos_assert_r(AosDeltaBeanOpr::isValid(opr), false);

	if (!mCondition) return true;
	
	AosExprObjPtr lhs_expr = mCondition->getLHS();
	AosExprObjPtr rhs_expr = mCondition->getRHS();
	aos_assert_r(lhs_expr && rhs_expr, false);
	
	AosValueRslt value_rslt;
	lhs_expr->getValue(rdata, input_record, value_rslt);

	OmnString l_iilname = getIILName(rdata, lhs_expr);
	OmnString r_iilname = getIILName(rdata, rhs_expr);

	AosQueryRsltObjPtr query_rslt = getRecordsByQuery(
			rdata, value_rslt, eAosOpr_prefix, r_iilname);
	generaterJoinRecord(rdata, opr, input_record, r_iilname, query_rslt);
	
	return true;
}


bool 
AosSyncherJoin::flushDeltaBeans(AosRundata *rdata)
{
	mLock->lock();

	if (mDeltaBeansBuff->dataLen() <= 0)
	{
		mLock->unlock();
		return true;
	}

	AosXmlTagPtr dsconf = generatorDatasetConf(rdata);
	aos_assert_rl(dsconf, mLock, false);
	AosSyncherStat *sync = OmnNew AosSyncherStat(0);
	aos_assert_rl(sync, mLock, false);
	
	AosXmlTagPtr tbl_doc = AosGetDocByObjid(mCrtTableObjid, rdata);
	aos_assert_rl(tbl_doc, mLock, false);
	AosXmlTagPtr statNode = tbl_doc->getFirstChild("stat");
	aos_assert_rl(statNode, mLock, false);
	sync->config(rdata, statNode);

	sync->proc(rdata, dsconf, mDeltaBeansBuff);
	mDeltaBeansBuff->clear();

	mLock->unlock();
	return true;
}


bool 
AosSyncherJoin::appendDeltaBean(
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


AosQueryRsltObjPtr
AosSyncherJoin::getRecordsByQuery(
		AosRundata *rdata, 
		const AosValueRslt &v,
		const AosOpr opr,
		const OmnString &iilname)
{
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(opr);
	query_context->setStrValue(v.getStr());
	//query_context->setReverse(true);
	query_context->setPageSize(0);
	query_context->setBlockSize(100);

	bool rslt = AosQueryColumn(iilname, query_rslt, 0, query_context, rdata);
	aos_assert_r(rslt, NULL);

	return query_rslt;
}


bool
AosSyncherJoin::generaterJoinRecord(
		AosRundata *rdata, 
		AosDeltaBeanOpr::E opr,
		const AosDataRecordObjPtr &input_record,
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt)
{
	AosXmlTagPtr recorddoc = getRecordIILConf(rdata, iilname);
	aos_assert_r(recorddoc, NULL);

	bool finished = false;
	u64 docid;
	OmnString vv;
	while (query_rslt->nextDocidValue(docid, vv, finished, rdata)) 
	{
		if (finished) return true;

		AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(recorddoc, 0, rdata AosMemoryCheckerArgs);
		aos_assert_r(record, false);
		AosBuffPtr buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
		buff->setOmnStr(vv);
		//record->setData(vv.getBuffer(), vv.length(), 0, 0);
		int status;
		record->setData(buff->data(), buff->dataLen(), 0, status);
		combinRecords(rdata, opr, input_record, record);
	}

	return true;
}


bool 
AosSyncherJoin::combinRecords(
		AosRundata *rdata, 
		AosDeltaBeanOpr::E opr,
		const AosDataRecordObjPtr &l_rcd, 
		const AosDataRecordObjPtr &r_rcd)
{
	bool rslt, outofmem;
	if (!mOutputRecord) createOutput(rdata, l_rcd->getRecordDoc(), r_rcd->getRecordDoc());
	aos_assert_r(mOutputRecord, false);

	mOutputRecord->clear();
	mOutputRecord->setOperator(opr);

	int l_nums = getNumFields(rdata, l_rcd);
	int r_nums = getNumFields(rdata, r_rcd);
	int totalFieldsNum = l_nums + r_nums;

	AosValueRslt value_rslt;
	for (int i = 0; i < totalFieldsNum; i++)
	{
		if (i < l_nums)	
		{
			rslt = l_rcd->getFieldValue(i, value_rslt, outofmem, rdata);
			aos_assert_r(rslt, false);
		}
		else 
		{
			rslt = r_rcd->getFieldValue(i - l_nums, value_rslt, outofmem, rdata);
			aos_assert_r(rslt, false);
		}
		rslt = mOutputRecord->setFieldValue(i, value_rslt, outofmem, rdata);
		aos_assert_r(rslt, false);
	}

	// append record 
	rslt = appendDeltaBean(rdata, mOutputRecord.getPtr());
	aos_assert_r(rslt, false);

	return true;
}


vector<boost::shared_ptr<DataField> >
AosSyncherJoin::getFields(
		AosRundata *rdata, 
		const AosXmlTagPtr &record_doc)
{
	vector<boost::shared_ptr<DataField> > fields;
	aos_assert_r(record_doc, fields)
	
	AosXmlTagPtr fieldsNode = record_doc->getFirstChild("datafields");
	aos_assert_r(fieldsNode, fields);
	AosXmlTagPtr fieldNode = fieldsNode->getFirstChild("datafield");
	while (fieldNode)
	{
		boost::shared_ptr<DataField> field = boost::make_shared<DataField>(fieldNode);
		fields.push_back(field);

		fieldNode = fieldsNode->getNextChild("datafield");
	}

	return fields;
}


OmnString 
AosSyncherJoin::getIILName(
		AosRundata *rdata, 
		const AosExprObjPtr &expr_opt)
{
	AosExprMemberOpt* expr = dynamic_cast<AosExprMemberOpt *>(expr_opt.getPtr());
	aos_assert_r(expr, "");

	// fieldname : t1.field1
	OmnString tablename = expr->getMember2();
	//OmnString fieldname = expr->getMember3();
	
	for (size_t i = 0; i < mJoinIndexs.size(); i++)
	{
		AosXmlTagPtr indexdoc = AosGetDocByObjid(mJoinIndexs[i], rdata);
		aos_assert_r(indexdoc, "");
		OmnString crtIndexTableName = indexdoc->getAttrStr("zky_tablename");
		if (tablename == crtIndexTableName) return mJoinIndexs[i];
	}

	//vector<AosXmlTagPtr> indexDocs;
	//AosJqlStatement::getDocs(rdata, JQLTypes::eIndexDoc, indexDocs);
	//aos_assert_r(indexDocs.size() > 0, "");
	//for (size_t i = 0; i < indexDocs.size(); i++)
	//{
	//	if (fieldname == indexDocs[i]->getAttrStr("zky_name"))
	//	{
	//		return indexDocs[i]->getAttrStr("zky_objid");
	//		//return indexDocs[i]->getAttrStr("zky_iilname");
	//	}
	//}

	return "";
}


AosXmlTagPtr 
AosSyncherJoin::getRecordIILConf(
		AosRundata *rdata, 
		const OmnString &iilobjid)
{
	AosXmlTagPtr indexdoc = AosJqlStatement::getDocByObjid(rdata, iilobjid);
	aos_assert_r(indexdoc, NULL);

	boost::shared_ptr<DataRecord> record = boost::make_shared<DataRecord>();
	record->setAttribute("type", "iil");
	record->setAttribute("zky_name", "recordiil");

	AosXmlTagPtr columnsNode = indexdoc->getFirstChild("columns");
	aos_assert_r(columnsNode, NULL);
	AosXmlTagPtr columnNode = columnsNode->getFirstChild("column");
	while (columnNode)
	{
		boost::shared_ptr<DataField> df = boost::make_shared<DataField>();
		df->setAttribute("type", columnNode->getAttrStr("type"));
		df->setAttribute("zky_name", columnNode->getAttrStr("zky_name"));
		record->setField(df);
		columnNode = columnsNode->getNextChild("column");
	}

	return AosXmlParser::parse(record->getConfig() AosMemoryCheckerArgs);
}


AosXmlTagPtr 
AosSyncherJoin::generatorDatasetConf(AosRundata *rdata)
{
	OmnString str = "";
	str << "<dataset jimo_objid=\"dataset_syncher_jimodoc_v0\" zky_name=\"output\" >";
	aos_assert_r(mOutputRecord, 0);
	str << mOutputRecord->getRecordDoc();
	str << "</dataset>";
	return AosXmlParser::parse(str AosMemoryCheckerArgs);
}


int 
AosSyncherJoin::getNumFields(
		AosRundata *rdata, 
		const AosDataRecordObjPtr &record)
{
	aos_assert_r(record, 0);
	AosXmlTagPtr recorddoc = record->getRecordDoc();
	aos_assert_r(recorddoc, 0);

	AosXmlTagPtr fieldsNode = recorddoc->getFirstChild("datafields");
	aos_assert_r(fieldsNode, 0);

	return fieldsNode->getNumChilds();
}
