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
// 2015/11/04 Created by Andy 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcSyncher.h"

#include "API/AosApi.h"
#include "DataProc/DataProc.h"
#include "JQLStatement/JqlStmtTable.cpp"
#include "Util/DataTypes.h"

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcSyncher_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcSyncher(version);
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


AosDataProcSyncher::AosDataProcSyncher(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcSyncher),
mTableSyncher(NULL)
{
}


AosDataProcSyncher::AosDataProcSyncher(const AosDataProcSyncher &proc)
:
AosJimoDataProc(proc),
mTableSyncher(NULL)
{
	if (proc.mTableSyncher)
	{
		AosJimoPtr jimo = proc.mTableSyncher->cloneJimo();
		aos_assert(jimo);
		mTableSyncherPtr = dynamic_cast<AosSyncherObj *>(jimo.getPtr()); 
		mTableSyncher = mTableSyncherPtr.getPtr();
	}
	mOpr = proc.mOpr;
}


AosDataProcSyncher::~AosDataProcSyncher()
{
}


bool
AosDataProcSyncher::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1"><![CDATA[
	//{
	//	"table": "table_name",
	//	"database": "db_name",
	//	"operator":"add|delete",
	//}
	//]]></data_proc>
	try
	{
		aos_assert_r(def, false);

		OmnString dp_jsonstr = def->getNodeText();
		mName = def->getAttrStr(AOSTAG_NAME, "");
		aos_assert_r(mName != "", false);

		JSONValue json;
		JSONReader reader;
		bool rslt = reader.parse(dp_jsonstr, json);
		aos_assert_r(rslt, false);
		mJson = json;

		OmnString db_name = json["database"].asString();
		aos_assert_r(db_name != "", false);

		OmnString table_name = json["table"].asString();
		aos_assert_r(table_name != "", false);

		OmnString name = AosJqlStmtTable::getTableName(table_name, db_name);
		aos_assert_r(name != "", false);

		OmnString objid = AosJqlStmtTable::getObjid(rdata, JQLTypes::eTableDoc, name);
		AosXmlTagPtr table_doc = AosGetDocByObjid(objid, rdata);
		aos_assert_r(table_doc, false);

		mTableSyncherPtr = AosSyncEngineGetSyncher(rdata.getPtr(), objid, AosSyncherType::eTable);
		mTableSyncher = mTableSyncherPtr.getPtr();
		aos_assert_r(mTableSyncher, false);

		OmnString opr = json["operator"].asString(); 
		mOpr = AosDeltaBeanOpr::toEnum(opr);
		aos_assert_r(AosDeltaBeanOpr::isValid(mOpr), false);
		return true;
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
}


bool
AosDataProcSyncher::createOutput(
		const OmnString &dpname,
		const JSONValue &json_conf,
		const AosRundataPtr &rdata)
{
	return true;
}
			

AosDataProcStatus::E
AosDataProcSyncher::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	mProcDataCount++;
	mOutputCount++;
	AosDataRecordObj *input_record = input_records[0];
	input_record->setOperator(mOpr);                          
	bool rslt = mTableSyncher->appendRecord(rdata_raw, input_record);   
	aos_assert_r(rslt, AosDataProcStatus::eContinue);
	return AosDataProcStatus::eContinue;
}

vector<AosDataRecordObjPtr> 
AosDataProcSyncher::getOutputRecords()
{
	vector<AosDataRecordObjPtr> v;
	v.push_back(mOutputRecord);
	return v;
}


AosJimoPtr 
AosDataProcSyncher::cloneJimo() const
{
	return OmnNew AosDataProcSyncher(*this);
}

AosDataProcObjPtr
AosDataProcSyncher::cloneProc() 
{
	return OmnNew AosDataProcSyncher(*this);
}


bool 
AosDataProcSyncher::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr, 
		const AosJimoProgObjPtr &prog)
{
	AosXmlTagPtr dp_xml;
	
	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, json);
	aos_assert_r(rslt, false);
	mJson = json;

	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" << objid << "\" ";
	dp_str << "><dataproc zky_name=\"";
	dp_str << dpname << "\" jimo_objid=\"dataprocsyncher_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
	dp_str << " </jimodataproc>";
	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}


bool
AosDataProcSyncher::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	i64 procDataCount = 0;
	i64 procOutputCount = 0;
	for (size_t i = 0; i < procs.size(); i++)
	{
		procDataCount += procs[i]->getProcDataCount();
		procOutputCount += procs[i]->getProcOutputCount();
	}
	OmnScreen << "DataProcSyncher " << "(" << mName << ")" << " finished:" 
				<< " procDataCount:" << procDataCount
				<< ", OutputCount:" << procOutputCount << endl;
	return true;
}

void
AosDataProcSyncher::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	//mInputRecords = records;
}

