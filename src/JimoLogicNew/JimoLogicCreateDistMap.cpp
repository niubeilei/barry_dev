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
// A Selectistics Task is defined as:
// 	Create dataset dataset-name
// 	(
// 	    type = <type>,
//      key_fields=[<expression-list>],
//      table = <tablename>,
//      condition = [<conditions>]
//  );
//
// It will create the following:

// Modification History:
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicCreateDistMap.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicCreateDistMap_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicCreateDistMap(version);
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


AosJimoLogicCreateDistMap::AosJimoLogicCreateDistMap(const int version)
:
AosJimoLogicNew(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicCreateDistMap::~AosJimoLogicCreateDistMap()
{
}


AosJimoPtr
AosJimoLogicCreateDistMap::cloneJimo() const
{
	return OmnNew AosJimoLogicCreateDistMap(*this);
}


bool
AosJimoLogicCreateDistMap::parseJQL(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		AosJimoProgObj *prog,
		bool &parsed,
		bool dft)
{
	// The statement is:
	// 	create distrubution map <distmap-name>
	// 	(
	// 		inputs: <dataset_name>,
	// 		table: tablename,
	// 		key_field: "<expr>",
	// 	    sample_size: <ddd>,
	// 	    split: cube | server | <ddd>
	// 	);
	parsed = false;

	aos_assert_rr(mKeywords.size() >= 3, rdata, false);
	aos_assert_rr(mKeywords[0] == "create", rdata, false);
	aos_assert_rr(mKeywords[1] == "distribution", rdata, false);
	aos_assert_rr(mKeywords[2] == "map", rdata, false);
	mErrmsg = "create distribution map "; 

	OmnString distmapname = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,distmapname,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << distmapname; 
	
	bool rslt = jimo_parser->getNameValueList(rdata, mNameValueList);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	mDistmapName = distmapname;
	// Parse table (must have at least one measure)
	AosExprObjPtr table = jimo_parser->getParmExpr(rdata, "table", mNameValueList);
	if(!table)
	{
		setErrMsg(rdata,eMissingParm,"table",mErrmsg);
		return false;
	}
	mTableName = table->dumpByNoQuote();

	if (!configExpr(rdata, jimo_parser, "inputs", mInputs, 
				mNameValueList, true, "missing_inputs")) return false;

	if (!configExpr(rdata, jimo_parser, "key_field", mKeyFieldExpr, 
				mNameValueList, true, "missing_key_field")) return false;

	if (!configInt(rdata, jimo_parser, "sample_size", eDftSampleSize,
				mSampleSize, mNameValueList)) return false;

	OmnString split = jimo_parser->getParmStr(rdata, "split", mNameValueList);
	if (split == "") split = "cube";

	if (split == "cube")
	{
		mSplit = AosGetNumCubes();
	}
	else if (split == "server")
	{
		mSplit = AosGetNumPhysicals();
	}
	else
	{
		mSplit = atoi(split.data());
	}
	if (mSplit <= 0)
	{
		 setErrMsg(rdata,eInvalidParm,"split",mErrmsg);
		 return false;
	}
	if(!checkNameValueList(rdata, mErrmsg, mNameValueList))
		return false;	

	parsed = true;
	return true;
}


bool 
AosJimoLogicCreateDistMap::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	// This function creates an AosJimoLogicSelect.
    bool rslt = createDistributionmap(rdata, prog, statements_str);
    aos_assert_rr(rslt, rdata, false);

    rslt = createDistributionmapDataproc(rdata, prog, statements_str);
    aos_assert_rr(rslt, rdata, false);

	rslt = addDistmapTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicCreateDistMap::createDistributionmap(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	//1. Distribution map name is the object name
    OmnString createmap_str;
	OmnString key_field = mKeyFieldExpr->dumpByNoQuote();
    createmap_str << "CREATE DISTRIBUTIONMAP " << mDistmapName << " ON " << mTableName
              << " KEY (" << key_field << ")"
              << " NUMBER " << mSampleSize 
              << " SPLIT " << mSplit
              << " TYPE " << "\"" << "u64" << "\""
              << " MAX LENGTH " << 50 << ";";

    OmnScreen << createmap_str << endl;
	statements_str << "\n" << createmap_str;
    bool rslt = parseRun(rdata,createmap_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);

    return true;
}


bool
AosJimoLogicCreateDistMap::createDistributionmapDataproc(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	//1. DataProc Distribution map name is "_dp_" + object name,
	//2. IIL name is "_zt44_map_t1_" + object name
	OmnString dataprocdistmap_str = "create dataprocdistributionmap ";
    OmnString iilmapname_str;
	OmnString key_field = mKeyFieldExpr->dumpByNoQuote();
	mDataprocDistName << "_dp_" << mDistmapName;
	iilmapname_str << "_zt44_map_" << mTableName << "_" << mDistmapName;
	dataprocdistmap_str << mDataprocDistName << " { "
        << "\"type\":" << "\"distributionmap\"" << ","
		<< "\"key_field\":" << "\"" << key_field << "\"" << ","
		<< "\"num\":" << mSampleSize << ","
        << "\"split\":" << mSplit << ","
        << "\"map_name\":" <<  "\"" << iilmapname_str << "\""
		<< "};";

	OmnScreen << dataprocdistmap_str << endl;
	statements_str << "\n" << dataprocdistmap_str;

    bool rslt = parseRun(rdata,dataprocdistmap_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);

	return true;
}


bool
AosJimoLogicCreateDistMap::addDistmapTask(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog)
{
	OmnString addDistmap_str;
 	OmnString inputs;
	OmnString output;
	OmnString TaskName;
	TaskName << "task_select_distmap_" << mDistmapName;
	inputs = mInputs->dumpByNoQuote();
	addDistmap_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataprocDistName << "\","
		<< "\"inputs\":" << "[\"" << inputs << "\"" << "]}]}";

	jimo_prog->appendStatement(rdata, "tasks", addDistmap_str);
	OmnScreen << addDistmap_str << endl;

	return true;
}
