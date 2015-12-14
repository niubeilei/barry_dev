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
// Statement Syntax:
// 		DATASET mydataset
// 		(
// 		    name: value,
// 		    name: value,
// 		    ...
// 		    name: value,
//		);
//
// Modification History:
// 2015/05/30 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcUpdateIndex.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcUpdateIndex_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcUpdateIndex(version);
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


AosJimoLogicDataProcUpdateIndex::AosJimoLogicDataProcUpdateIndex(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcUpdateIndex::~AosJimoLogicDataProcUpdateIndex()
{
}


bool 
AosJimoLogicDataProcUpdateIndex::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	data proc index data_proc_index_name
	// 	(
	//		inputs:[<>,<>...],	//only have one
	//		table:t1,
	// 		new_value:<name>,
	// 		old_value:<input_name>,
	// 		max_keylen:dd,
	//		docid:docid,
	//		shuffle_type:cube,
	//		shuffle_field:
	//		record_type:xxx
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "updateindex", rdata, false);
	mErrmsg << "dataprco updateindex";
	// parse dataset name
	OmnString dataproc_index_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dataproc_index_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dataproc_index_name;
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	mDataProcName = dataproc_index_name;
	mNameValueList = name_value_list;

	// parse table
	mTable = jimo_parser->getParmExpr(rdata, "table", name_value_list);
	if (!mTable)
	{
		setErrMsg(rdata, eMissingParm, mTable->dumpByNoQuote(), mErrmsg);
		return false;
	}

	// parse inputs
	rslt = jimo_parser->getParmArrayStr(rdata, "inputs", name_value_list, mInputs);
	if (!rslt || mInputs.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);
		return false;
	}

	// parse new_value
	mNewValue = jimo_parser->getParmStr(rdata, "new_value", name_value_list);
	if (mNewValue == "")
	{
		setErrMsg(rdata,eMissingParm,"new_value",mErrmsg);
		return false;
	}

	// parse old_value
	mOldValue = jimo_parser->getParmStr(rdata, "old_value", name_value_list);
	if (mOldValue == "")
	{
		setErrMsg(rdata,eMissingParm,"old_value",mErrmsg);
		return false;
	}
	// parse max_keylen
	mMaxKeyLen= jimo_parser->getParmInt(rdata, "max_keylen", name_value_list);
	if (mMaxKeyLen <= 0)
	{
		setErrMsg(rdata,eMissingParm,"max_keylen",mErrmsg);
		return false;
	}

	//parse shuffle_type
	mShuffleType = jimo_parser->getParmStr(rdata, "shuffle_type", name_value_list);

	//parse shuffle_field
	mShuffleField = jimo_parser->getParmStr(rdata, "shuffle_field", name_value_list);

	//parse record_type
	mRecordType = jimo_parser->getParmStr(rdata, "record_type", name_value_list);
	if (mRecordType == "")
	{
		mRecordType = "buff";
	}
		
	//parse docid
	mDocid = jimo_parser->getParmStr(rdata, "docid", name_value_list);
	if(mDocid == "")
	{
		setErrMsg(rdata,eMissingParm,"docid",mErrmsg);
		return false;
	}
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool 
AosJimoLogicDataProcUpdateIndex::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	bool rslt = createDataProcUpdateIndex(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = createDataProcIILBatchopr(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

    rslt = addUpdateIndexTask(rdata, prog);
	aos_assert_rr(rslt, rdata, prog);

	rslt = addIILTask(rdata,prog);
	aos_assert_rr(rslt, rdata, prog);

	return true;
}


bool
AosJimoLogicDataProcUpdateIndex::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcUpdateIndex*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosJimoLogicDataProcUpdateIndex::createDataProcUpdateIndex(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statement_str)
{
	mDataProcUpdateIndexName << "_dp_" << mDataProcName;

	OmnString dataprocUpdateIndex_str = "create dataprocupdateindex ";
	dataprocUpdateIndex_str << mDataProcUpdateIndexName << " { "
		<< "\"type\":" <<"\"" <<mKeywords[1]<< "\"" 
		<< ", \"new_value\":" << "\"" << mNewValue << "\""
		<< ", \"old_value\":" << "\"" << mOldValue << "\""
		<< ", \"record_type\":" << "\"" << mRecordType << "\""
		<< ", \"max_keylen\":" << mMaxKeyLen
		<< ", \"docid\":" << "\"" << mDocid << "\"";
	dataprocUpdateIndex_str << "};";

	OmnScreen << dataprocUpdateIndex_str << endl;
	statement_str << "\n" << dataprocUpdateIndex_str;

    bool rslt = parseRun(rdata, dataprocUpdateIndex_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);

	return true;
}


bool
AosJimoLogicDataProcUpdateIndex::createDataProcIILBatchopr(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statement_str)
{
	OmnString dataprociiladd_str = "create dataprociilbatchopr ";
	mDataProcIILAddName << "_dp_" << mDataProcName << "_updateindex_iil_add" ;
	OmnString iilName;
	iilName << "_zt44_idx_" << mTable->dumpByNoQuote() << "_updateindex_iil_add";
	dataprociiladd_str << mDataProcIILAddName << " { "
		<< "\"iilname\":" << "\"" << iilName << "\"" << ","
		<< "\"opr\":" << "\"stradd\"" << ","
        << "\"build_bitmap\":" << "\"true\""
		<< "};";

	OmnScreen << dataprociiladd_str << endl;
	statement_str << "\n" << dataprociiladd_str;

    bool rslt = parseRun(rdata, dataprociiladd_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);

	OmnString dataprociildel_str = "create dataprociilbatchopr ";
	mDataProcIILDelName << "_dp_" << mDataProcName << "_updateindex_iil_del" ;
	iilName = "";
	iilName << "_zt44_idx_" << mTable->dumpByNoQuote() << "_updateindex_iil_del";
	dataprociildel_str << mDataProcIILDelName << " { "
		<< "\"iilname\":" << "\"" << iilName << "\"" << ","
		<< "\"opr\":" << "\"stradd\"" << ","
        << "\"build_bitmap\":" << "\"true\""
		<< "};";
	OmnScreen << dataprociildel_str << endl;
	statement_str << "\n" << dataprociildel_str;

    rslt = parseRun(rdata, dataprociildel_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);

	return true;
}


bool
AosJimoLogicDataProcUpdateIndex::addUpdateIndexTask(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog)
{
	//aos_assert_rr(jimo_prog, rdata, false);
	OmnString index_task_str;
	OmnString taskName;
	taskName << "task_updateindex_" << mDataProcName;
	index_task_str << " {"
		<< "\"name\":" << "\"" << taskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataProcUpdateIndexName << "\","
		<< "\"inputs\":" << "[" ;
	for(size_t i = 0;i < mInputs.size(); i++)
	{
		if(i>0)	
			index_task_str << ",";
		index_task_str << "\"" << mInputs[i]<< "\"";
	}
	index_task_str <<"],";
	index_task_str << "\"outputs\":" << "["
			      << "\""<< mDataProcUpdateIndexName << "_output_add"<< "\""
			      << ", \""<< mDataProcUpdateIndexName << "_output_del"<< "\"";
	index_task_str <<"]"<< "}]}";

	if(jimo_prog)
		jimo_prog->appendStatement(rdata, "tasks", index_task_str);
	OmnScreen << index_task_str << endl;

	return true;
}



bool
AosJimoLogicDataProcUpdateIndex::addIILTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{
	//aos_assert_rr(jimo_prog, rdata, false);
	OmnString iil_add_str;
	OmnString taskName;
	taskName << "task_iil_batchopr_add_" << mDataProcName;
	iil_add_str << " {"
		<< "\"name\":" << "\"" << taskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataProcIILAddName << "\","
		<< "\"inputs\":"<< "["
		<< "\"" << mDataProcUpdateIndexName << "_output_add" << "\""
		<< "]}]}";
	if(jimo_prog)
		jimo_prog->appendStatement(rdata,"tasks" ,iil_add_str);
	OmnScreen << iil_add_str << endl;

	taskName = "";
	taskName << "task_iil_batchopr_del_" << mDataProcName;
	OmnString iil_del_str;
	iil_del_str << " {"
		<< "\"name\":" << "\"" << taskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataProcIILDelName << "\","
		<< "\"inputs\":"<< "["
		<< "\"" << mDataProcUpdateIndexName << "_output_del" << "\""
		<< "]}]}";
	if(jimo_prog)
		jimo_prog->appendStatement(rdata, "tasks" ,iil_del_str);
	OmnScreen << iil_del_str << endl;

	return true;
}


AosJimoPtr 
AosJimoLogicDataProcUpdateIndex::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcUpdateIndex(*this);
}

