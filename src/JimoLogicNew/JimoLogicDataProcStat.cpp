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
// A Statistics Task is defined as:
// 	Create Statistics stat-name
// 	(
//     	Name="<stat-name>",
//      Inputs="<input-data>",
//      KeyFields=[<expression-list>],
//      Measures=[<measure-list>],
//      Filters=<condition>,
//      TimeUnit="<time_unit>"
//  );
//
// It will create the following:
// 	1. A dataset for the key field
// 	2. A group-by data proc
// 	3. A stat-join data proc
// 	4. A vector2D data proc
// 	5. A update key table data proc
// 	6. A update index data proc
// 	7. A task for the group-by data proc
// 	8. A task for join data proc
// 	9. A task for updating vector2D data proc
// 	10. A task for updating index data proc
// 	11. A task for updating key table


// Modification History:
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcStat.h"

#include "API/AosApi.h"
#include "SEUtil/Objid.h"
#include "SEUtil/ObjType.h"
#include "StatUtil/Jimos/StatModel.h"

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcStat_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcStat(version);
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


AosJimoLogicDataProcStat::AosJimoLogicDataProcStat(const int version)
:
AosJimoLogicNew(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcStat::~AosJimoLogicDataProcStat()
{
}

AosJimoPtr
AosJimoLogicDataProcStat::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcStat(*this);
}

void
AosJimoLogicDataProcStat::initParms()
{
	mDBTableName = "";
	mOrigStmt = "";
	mOpr = "";
	mCond.clear();
	
	mStatIdx.clear();
	mKeyIdxOpr.clear();
	mStatNames.clear();
	mStatModelMeasures.clear();
	mStatModelKeyFields.clear();
}

bool
AosJimoLogicDataProcStat::parseJQL(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		AosJimoProgObj *prog,
		bool &parsed,
		bool dft)
{
	// The statement is:
	// 	Create Statistics <stat-name>
	// 	(
	// 		name-value-list,
	// 		count_value: 0 | 1,
	// 	);
	initParms();
	mIsService = dft;
	
	parsed = false;
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "statistics", rdata, false);

	mErrmsg = "dataproc statistics ";	
	OmnString stat_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,stat_name,mErrmsg))
	{
		return false;
	}
	mErrmsg << stat_name;

	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}
	
	mDataProcName = stat_name;
	mNameValueList = name_value_list;

	//parse table
	OmnString tableName = jimo_parser->getParmStr(rdata, "table", name_value_list);
	if(tableName == "")
	{
		setErrMsg(rdata,eMissingParm,"table",mErrmsg);
		return false;
	}
	AosRundataPtr rdataPtr(rdata);
	AosXmlTagPtr tableDoc = AosJqlStatement::getDoc(rdataPtr,JQLTypes::eTableDoc,tableName);
	if(!tableDoc)
	{
		setErrMsg(rdata,eNotExist,tableName,mErrmsg);
		return false;	
	}
	
	//get StatModelName
	AosXmlTagPtr statDoc;
	rslt = getStatDoc (rdata, tableDoc, statDoc);
	aos_assert_r(rslt,false);
	
	OmnString statModelStr = jimo_parser->getParmStr(rdata, "stat_model", name_value_list);
	if(statModelStr != "")
	{
		mStatNames.push_back(statModelStr);	
	}
	else
	{
		rslt = jimo_parser->getParmArrayStr(rdata, "stat_model_list", name_value_list, mStatNames);
		if(mStatNames.empty())
		{
			rslt = getStatName(rdata, mStatNames, statDoc);
			if (!rslt)
			{
				OmnString msg;
				msg << "[ERR] : " << mErrmsg << " : " << tableName << " have no statistic model!";
				rdata->setJqlMsg(msg);
				return false;
			}
		}
	}
	//parse count_value	
	mCountValue  = jimo_parser->getParmInt(rdata, "count_value", name_value_list);
	if(mCountValue == -1) 
	{ 
		mCountValue  = 1;
	}
		
	// get databasename
	OmnString databaseName = AosStatModel::getDbTableName(rdata, mStatNames[0]);
	mDBTableName << databaseName << "_" << tableName ;
	
	for(u32 i = 0; i< mStatNames.size();i++ )
	{
		// stat model index
		OmnString statModelName = mStatNames[i];
		AosStatModel statModel(statModelName);
		OmnString statIdx;
		rslt = getStatIdx(rdata, statModelName, statIdx,  statDoc);
		if (!rslt)
			return false;
		mStatIdx[statModelName] = statIdx;

		// parse key_fields
		vector<OmnString> keyFields = statModel.getKeyFields();
		mStatModelKeyFields[statModelName] = keyFields;
		
		// get ket index info from all the keys except that keys' type are str
		rslt = getKeyIndexOpr(rdata, keyFields, mKeyIdxOpr, tableDoc);
		if (!rslt) 
			return false;

		// parse measures
		vector<OmnString> measureStrs = statModel.getMeasures();
		if( measureStrs.size() <= 0)
		{
			setErrMsg(rdata,eMissingParm,"measures",mErrmsg);
			return false;
		}
		mStatModelMeasures[statModelName] = measureStrs;

		//parse shuffle fields
		vector<OmnString> shuffleFields = statModel.getShuffleFields();
		mStatModelShuffleFields[statModelName] = shuffleFields;

		// parse time
		AosStatTimePtr timefield = statModel.getTime();
		if(timefield)
		{
			StatTime time;
			time.mTimeField = timefield->getTimeFieldName();
			time.mFormat = timefield->getTimeFormat();
			time.mTimeUnit = timefield->getGrpbyTimeUnit();
			mTime[statModelName] = time;                    
		}

		// parse condition
		OmnString cond_str = statModel.getCond();
		if (cond_str != "")
			    mCond[statModelName] = cond_str;  
	}


	//Parse Input
	mInput = jimo_parser->getParmExpr(rdata, "inputs", name_value_list);
	if(!mInput || mInput->dumpByNoQuote() == "")
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);
		return false;
	}

	// parse opr
	mOpr = jimo_parser->getParmStr(rdata, "data_opr", name_value_list);
	if (mOpr == "")
		mOpr = "insert";

	if (!syntaxCheck(rdata, jimo_parser)) 
		return false;

	if(!checkNameValueList(rdata,mErrmsg,name_value_list))
		return false;
	
	parsed = true;
	return true;
}

bool
AosJimoLogicDataProcStat::getStatDoc(
		AosRundata *rdata, 
		AosXmlTagPtr &tabledoc,
		AosXmlTagPtr &statistic_def)
{

	aos_assert_r(tabledoc, false);  
	AosXmlTagPtr statistics = tabledoc->getFirstChild("statistic");
	if(!statistics)	return true;

	OmnString sobjid = statistics->getAttrStr("stat_doc_objid", "");
	aos_assert_r(sobjid != "", false);

	statistic_def = AosJqlStatement::getDocByObjid(rdata, sobjid);
	aos_assert_r(statistic_def, false);

	AosXmlTagPtr inte_stats_conf = statistic_def->getFirstChild("internal_statistics");
	if(!inte_stats_conf)	return false;

	return true;
}

bool
AosJimoLogicDataProcStat::getStatName(
		AosRundata *rdata, 
		vector<OmnString> &stat_model_name, 
		AosXmlTagPtr &statistic_def)
{
	OmnString stat_name;
	aos_assert_r(statistic_def, false);  

	AosXmlTagPtr def_confs = statistic_def->getFirstChild("statistic_defs");
	if(!def_confs)  return false;

	AosXmlTagPtr statistic = def_confs->getFirstChild("statistic");
	if(!statistic)
		return false;
	while (statistic)
	{
		OmnString conf_objid = statistic->getAttrStr("zky_stat_conf_objid", "");

		OmnString tmp = "";
		tmp << "_zt4k_";
		int idx = conf_objid.indexOf(tmp, 0);

		if (idx >= 0)
		{
			stat_name= conf_objid.substr(tmp.length()+idx);
			stat_model_name.push_back(stat_name);
		}
		statistic = def_confs->getNextChild();
	}
	return true;
}

bool
AosJimoLogicDataProcStat::getStatIdx(
			AosRundata *rdata, 
			OmnString &stat_model_name,
			OmnString &stat_index, 
			AosXmlTagPtr &statistic_def)
{
	OmnString stat_name;
	aos_assert_r(statistic_def, false);  
	AosXmlTagPtr inte_stats_conf = statistic_def->getFirstChild("internal_statistics");
	if(!inte_stats_conf)	return false;

	if (inte_stats_conf)
	{
		AosXmlTagPtr statistic = inte_stats_conf->getFirstChild("statistic");
		while (statistic)
		{
			string conf_objid = statistic->getAttrStr("zky_stat_conf_objid", "");
			if (conf_objid.find(stat_model_name) != string::npos)
			{
				unsigned found  = conf_objid.find_last_of('_');
				stat_index = conf_objid.substr(found+1);
				if(stat_index != "")    return true;
			}
			statistic = inte_stats_conf->getNextChild();
		}
	}
	return true;
}

bool
AosJimoLogicDataProcStat::getKeyIndexOpr(
		AosRundata *rdata, 
		vector<OmnString> &keyfields, 
		map<OmnString, OmnString>   &key_index_opr,
		AosXmlTagPtr &tabledoc)
{
	aos_assert_r(tabledoc, false);  

	OmnString objid = tabledoc->getAttrStr("zky_use_schema", "");
	aos_assert_r(objid != "", false);

	AosXmlTagPtr dataschema = AosJqlStatement::getDocByObjid(rdata, objid);
	aos_assert_r(dataschema, false);

	AosXmlTagPtr datarecord = dataschema->getFirstChild("datarecord");
	aos_assert_r(datarecord, false);

	AosXmlTagPtr datafields  = datarecord->getFirstChild("datafields");
	if(!datafields) return true;

	AosXmlTagPtr datafield = datafields->getFirstChild("datafield");
	while (datafield)
	{
		OmnString name = datafield->getAttrStr("zky_name", "");
		for (u32 i=0; i<keyfields.size();i++)           
		{
			if (name == keyfields[i])
			{
				// JIMODB-357
				OmnString type = datafield->getAttrStr("data_type", "");
				if(type=="")
				{
					type = datafield->getAttrStr("type", "");
				}
				// 2015/07/24
				type = convertType(rdata, type);
				if (type != "BigStr")
				{
					key_index_opr[name] = type;
				}
				break;
			}
		}
		datafield = datafields->getNextChild();	
	}
	return true;
}
	
bool 
AosJimoLogicDataProcStat::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	// A Create Stat statement can appear:
	// 	1. In a script
	// 	2. In a job (BEGIN JOB ... END JOB)
	// 	3. A standalone (not supported yet)

	// This function creates an AosJimoLogicStat. It lets AosJimoLogicStat
	// generate code. If 'prog' is a JimoJob or a JimoScript, it adds the
	// statements to 'prog' and runs the statements.
    //mJobName = prog->getJobname();
	AosJimoLogicStatPtr stat = OmnNew AosJimoLogicStat(
	 		mStatNames, mDBTableName, mStatModelKeyFields, mStatModelMeasures,mStatModelShuffleFields,
	 		mInput, mCond, mTime, mOpr, mStatIdx,mKeyIdxOpr, mCountValue, mDataProcName, mIsService);
	stat->run(rdata, prog, statements_str, true);
	return true;
}


bool
AosJimoLogicDataProcStat::syntaxCheck(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser)
{
	// The statement should be:
	// 	Create Statistics stat-name
	// 	(
	// 		Inputs="<input-data>",
	// 		KeyFields=[<expression-list>],
	// 		Measures=[<measure-list>],
	// 		Filters=<condition>,
	// 		TimeUnit="<time_unit>"
	// 	);

	// 1. Check whether it has the inputs,
	// 2. Check whether it has key fields
	// 3. Check whether it has measures

	return true;
}

bool
AosJimoLogicDataProcStat::compileJQL(
				AosRundata *rdata,
				AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcStat*>(this);
	if(mIsService)
		prog->setStat(mDataProcName);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	return true;
}

OmnString
AosJimoLogicDataProcStat::getTableName() const
{
	return mDBTableName;
}
