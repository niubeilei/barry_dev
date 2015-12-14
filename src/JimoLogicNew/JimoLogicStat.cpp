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
//
// Modification History:
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicStat.h"
#include "JimoProg/JimoProgJob.h"
#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"
#include "JQLStatement/JqlStatement.h"
#include "SEInterfaces/ExprObj.h"
#include "StatUtil/Jimos/StatModel.h"
/*
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicStat_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicStat(version);
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
*/

AosJimoLogicStat::AosJimoLogicStat(
					vector<OmnString> &statname,
					OmnString &tablename,
					map<OmnString, vector<OmnString> > &keys,
					map<OmnString, vector<OmnString> >&measures,
					map<OmnString, vector<OmnString> >&shuffleFields,
					AosExprObjPtr &input,
					map<OmnString, OmnString> &cond,
					map<OmnString, StatTime> &time,
					OmnString	&opr,
					map<OmnString,OmnString>	&statidx,
					map<OmnString, OmnString> &key_idx_opr,
					const int64_t	&countValue,
					OmnString &dataprocname,
					bool dft)
{
	mStatModelName = statname;
	mTableName = tablename;
	mKeyFields = keys;
	mMeasures = measures;
	mShuffleFields = shuffleFields;
	mInput = input;
	mCond = cond;
	mCountValue = countValue;
	mIsService = false;

	map<OmnString,StatTime> ::iterator itr;
	for (u32 i=0; i<statname.size(); i++)
	{
		itr = time.find(statname[i]);
		if (itr != time.end())
		{
			StatTime stat_time;
			stat_time.mTimeUnit = itr->second.mTimeUnit;
			stat_time.mTimeField = itr->second.mTimeField;
			stat_time.mFormat = itr->second.mFormat;
			mTime[statname[i]] = stat_time;
		}
	}
	
	mIsNeedShuffle = true;
	mStatIdx = statidx;
	mOpr = opr;
	mKeyIdxOpr = key_idx_opr;
	mMaxLen = "200";
	mIdx = 0;
	if(dft)
		mIsService = dft;
	mDataProcName = dataprocname;
}


AosJimoLogicStat::~AosJimoLogicStat()
{
}


bool
AosJimoLogicStat::run(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str,
		bool inparser)
{
	// This is a JQL statement:
	// 		Add Statistics to Job
	// 		(
	// 			job_name="xxx",
	// 			stat_name="xxx",
	// 			Inputs="<input-data>",			mandatory
	// 			KeyFields=[<expression-list>],	mandatory
	// 			Measures=[<measure-list>],		mandatory
	// 			Filters=<condition>,			optional
	// 			TimeUnit="<time_unit>"			optional
	// 		    name=expression,
	// 		    name=expression,
	// 		    name=expression,
	// 		    ...
	// 		);
	//
	// This statement adds a statistics to a job. If 'Inputs',
	// 'KeyFields', and 'Measures' are present, this is an embedded
	// statistics. Otherwise, 'stat_name' refers to a statistics
	// created somewhere else and saved in the database.
	//
	// It will create the following:
	// 	1. A dataset for the key field
	// 	2. A group-by data proc
	// 	3. A stat-join data proc
	// 	4. A vector2D data proc
	// 	5. A update key table data proc
	// 	6. A update index data proc (one for each key field)
	// 	7. A task for the group-by data proc
	// 	8. A task for join data proc
	// 	9. A task for updating vector2D data proc
	// 	10. A task for updating index data proc
	// 	11. A task for updating key table

	if(mIsService)
	{
		vector<OmnString> fields;
		int num = AosSplitStr(mTableName, "_", fields, 4);
		aos_assert_rr(num > 0, rdata, false);
		mTabName = fields[1];
		mDBName = fields[0];
		AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdata,JQLTypes::eTableDoc, mTabName);
		if (!table_doc)
		{
			//setErrMsg(rdata,3,stmt->mTableName, mErrmsg);
			return false;
		}
		OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
		aos_assert_rr(schema_objid != "", rdata, false);
		AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdata);;
		aos_assert_rr(schema_doc, rdata, false);
		AosXmlTagPtr record_doc = schema_doc->getFirstChild();
		record_doc = record_doc->getFirstChild();
		mSchemaName = record_doc->getAttrStr("zky_name");

		mInputStat = mInput->dumpByNoQuote(); 
	}
	bool rslt;
	for (u32 k=0; k<mStatModelName.size(); k++)
	{
		mIdx++;
		//arvin 2015.08.27
		itr_t itr = mKeyFields.find(mStatModelName[k]);
		if(itr != mKeyFields.end())
		{
			rslt  = vFieldCheck(rdata,itr->second);	
		}
		rslt = createDataset(rdata, jimo_prog, statements_str,k);
		aos_assert_rr(rslt, rdata, false);

		rslt = createGroupbyDataProc(rdata, jimo_prog, statements_str,k);
		aos_assert_rr(rslt, rdata, false);

		rslt = createStatJoinDataProc(rdata, jimo_prog, statements_str,k);
		aos_assert_rr(rslt, rdata, false);

		rslt = createStatDocDataProc(rdata, jimo_prog, statements_str,k);
		aos_assert_rr(rslt, rdata, false);

		rslt = createUpdateStatKeyTableDataProc(rdata, jimo_prog, statements_str,k);
		aos_assert_rr(rslt, rdata, false);

		mIILDataprocName.clear();
		// keyfields
		itr = mKeyFields.find(mStatModelName[k]);
		if (itr == mKeyFields.end())
		{
			return false;
		}
		vector<OmnString> keyfields = itr->second;

		for (u32 i=0; i < keyfields.size(); i++)
		{
			//mIILDataprocName.clear();
			rslt = createUpdateIILDataProc(rdata, jimo_prog, keyfields, k, i, statements_str);
			aos_assert_rr(rslt, rdata, false);
		}

		if(!mIsService)
		{
			rslt = addGroupByTask(rdata, jimo_prog, k);
			aos_assert_rr(rslt, rdata, false);

			rslt = addJoinTask(rdata, jimo_prog, k);
			aos_assert_rr(rslt, rdata, false);

			rslt = addVector2DTask(rdata, jimo_prog, k);
			aos_assert_rr(rslt, rdata, false);

			rslt = addKeyTableTask(rdata, jimo_prog, k);
			aos_assert_rr(rslt, rdata, false);

			for	(u32 i=0; i< keyfields.size(); i++)
			{
				rslt = addIILTask(rdata, jimo_prog, keyfields, k, i);
				aos_assert_rr(rslt, rdata, false);
			}
		}
	}
	return true;
}

bool
AosJimoLogicStat::parseRun(AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog)
{
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	bool rslt = jimo_parser->parse(rdata, jimo_prog, stmt, statements);
	aos_assert_rr(rslt, rdata, false);
	if (statements.size() <= 0)
	{
		AosLogError(rdata, true, "failed_parsing_statement")
			<< AosFN("Statement") << stmt << enderr;
		return false;
	}

	OmnString ss;
	for(size_t i = 0;i < statements.size();i++)
	{
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, jimo_prog, ss, true);
	}
//	if (!statements[0]->run(rdata, jimo_prog))
//	{
//		AosLogError(rdata, true, "failed_run_statement") << enderr;
//		return false;
//	}
	return true;

}


bool
AosJimoLogicStat::createDataset(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str,
		u32 k)
{
	// Datasets are created as follows:
	// create datasetiil iil_statkey_t1_stat_model
	// {
	//	"type": "iil",
	//	"iilname": "_zt4k_stat",
	//	"conditions": true,
	//	"schema":
	//	{
	//		"type": "iil",
	//		"fields":
	//		[
	//			{
	//				"type": "str",
	//				"name": "right_statkey",
	//				"max_length": 50
	//			},
	//			{
	//				"type": "bin_u64",
	//				"name": "sdocid"
	//			}
	//		]
	//	}
	// };
	//
	// Create Dataset <name>
	// (
	//    Type=”iil”,
	//    IILName=”keyfield_inputs”,
	//    Schema=
	//    (
	//    Type=“iil“,
	//    Fields=
	//    [
	//      Field(Type=“str“, Name=“right_statkey“, MaxLength=50),
	//    	Field(Type=”bin_u64”, name=”docid”)
	//    ]
	//    )
	//  )
	//
	//
	OmnString dataset_str = "create datasetiil ";
	OmnString job_name = jimo_prog->getJobname();
	OmnString iilname;
	mDatasetName = "";
	mDatasetName << "ds_stat_" << job_name << "_" << mTableName << "_"<< mStatModelName[k];
	if (mIsNeedShuffle)
		iilname << "_zt4k_" << mTableName << "_stat_" << mStatModelName[k] << "_internal_0__$group";
	else
		iilname << "_zt4k_" << mTableName << "_stat_" << mStatModelName[k] << "_internal_0__0" ;
	if (mIsService)
	{
		iilname = "";
		iilname << "_zt4k_" << job_name << "_" << mTableName << "_stat_" << mStatModelName[k] << "_internal_0__0" ;
	}
	dataset_str << mDatasetName
		<< " { " <<"\"type\":" << "\"iil\","
		<< "\"iilname\":" << "\"" << iilname << "\"" << ", "
		<< "\"conditions\":" << "\"true\"" << ",";
	if (mIsNeedShuffle)
		dataset_str << "\"split\": {\"type\":\"datacol_by_cube\"},";

	dataset_str << "\"schema\":" << "{" << "\"type\":" << "\"iil\"," << "\"fields\":["
		<< "{"<< "\"type\":" << "\"str\"," << "\"name\":" << "\"right_statkey\"," << "\"max_length\":" << mMaxLen << "},"
		<< "{" << "\"type\":" << "\"bin_u64\"," << "\"name\":" << "\"docid\"}"
		<< "]"
		<< "}"
		<< "};";

	OmnScreen << dataset_str << endl;
	statements_str << "\n" << dataset_str;

	bool rslt = parseRun(rdata,dataset_str,jimo_prog);
	aos_assert_rr(rslt,rdata,false);

	// if (jimo_prog && mIsMemoryDataset)
	// {
	// 	jimo_prog->appendDataset(rdata, mDatasetName, ...);
	// }
	return true;
}


bool
AosJimoLogicStat::createGroupbyDataProc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statements_str,
		u32 k)
{
	// Create DataProc <name>
	// {
	//	  type="groupby",
	//	  keys=$.Keys,
	//	  Measures=$.Measures
	//	  MaxLength=80 (will not used in the future),
	//	  count_value = 0 | 1
	// };
	//
	// When running this statement, it creates an XML doc in the database.
	// In this new implementation, if the statement creates a database
	// data proc, it is done as before.

//	aos_assert_r(k < mKeyFields.size(), false);

	OmnString groupby_str = "";
	OmnString dataprocName = "";
	if(!mIsService)
	{
		groupby_str = "create dataprocgroupby ";
		mJobName = jimo_prog->getJobname();
		mGroupbyDataProcName = "";
		mGroupbyDataProcName << mJobName << "_dp_stat_groupby_" << mTableName << "_" << mStatModelName[k];
		groupby_str << mGroupbyDataProcName
					<< " { " << "\"type\": " << "\"groupby\","
					<< "\"table\":\"" << mTableName << "\",";
	}
	else
	{
		dataprocName << "dpGroup_" << mIdx;
		dataprocName << mStatModelName[k];
		mGroupbyDataProcName = dataprocName;
		OmnString inName = mInputStat;
		AosJimoLogicObjNew * jimo_logic = jimo_prog->getJimoLogic(rdata, mInputStat);
		if (jimo_logic)
		{
			AosJimoLogicType::E type = jimo_logic->getJimoLogicType();
			if (type == AosJimoLogicType::eDataset)
			{
				jimo_prog->setDatasetMap(mInputStat);
			}
			else
			{
				inName << ".output";
			}
		}
		else
		{
			OmnAlarm << "can't find jimo_logic : " << mInputStat << enderr;
		}
		jimo_prog->insertToOtherNameMap(inName, mGroupbyDataProcName);
		groupby_str << "\"" << dataprocName << "\" : "
					<< " { " << "\"type\": " << "\"dataprocgroupby\""<< ",";
	}
        groupby_str << "\"record_type\":" << "\"buff\","
        << "\"format\":" << "\""<< mSchemaName <<"\"";
	map<OmnString, OmnString>::iterator itr;
	map<OmnString, vector<OmnString> >::iterator itr_1;
	
	// conditon
	itr = mCond.find(mStatModelName[k]);
	if (itr != mCond.end()) 
	{
		groupby_str << ",\"condition\":" << "\"" << itr->second << "\"";
	}

	// keyfields
	itr_1 = mKeyFields.find(mStatModelName[k]);
	OmnString shuffle_field_str;
	if (itr_1 != mKeyFields.end())
	{
		vector<OmnString> keyfields = itr_1->second;
		groupby_str << ",\"keys\": " << "[" ;
		for(u32 i = 0 ; i < keyfields.size() ; i++)
		{
			if(i>0)
			{
				groupby_str << ",";
				shuffle_field_str << ",";
			}
			shuffle_field_str << keyfields[i];
			groupby_str << "\"" << keyfields[i] << "\"" ;
		}
		if(keyfields.size()==1 && keyfields[0] == AOS_EMPTY_KEY)
			shuffle_field_str="";
		groupby_str << "],";
	}

	itr_1 = mShuffleFields.find(mStatModelName[k]);
	if (itr_1 != mShuffleFields.end() && (itr_1->second).size())
	{
		vector<OmnString> shuffleFields = itr_1->second;
		shuffle_field_str = "";
		for(u32 i = 0 ; i < shuffleFields.size() ; i++)
		{
			if(i>0)
			{
				shuffle_field_str << ",";
			}
			shuffle_field_str << shuffleFields[i];
		}
	}
	// measure
	itr_1 = mMeasures.find(mStatModelName[k]);
	if (itr_1 == mMeasures.end())
	{
		    return false;
	}
	vector<OmnString> measures= itr_1->second;
	groupby_str << "\"measures\":" << "[";
	for(u32 i = 0; i < measures.size(); i++)
	{
		if(i>0)
			groupby_str << ",";
		groupby_str << "\"" << measures[i] << "\"" ;
	}
	groupby_str << "],"  << "\"max_keylen\":" << 500 ;
	
	// time 
	map<OmnString, StatTime>::iterator itr_2 = mTime.find(mStatModelName[k]);
	StatTime time;
	if (itr_2 != mTime.end())
	{
		time = itr_2->second;
		groupby_str << "," << "\"time\":" << "{";
		if(time.mTimeField != "")
			groupby_str << "\"field\":" << "\"" << time.mTimeField << "\"";

		if(time.mFormat != "") 
			groupby_str << "," << "\"format\":"  << "\"" << time.mFormat << "\"" ;

		if(time.mTimeUnit != "")
			groupby_str << "," << "\"unit\":" << "\"" << time.mTimeUnit << "\"";

		groupby_str <<  "}";
	}

	if (mIsNeedShuffle)
	{
		groupby_str << "," << "\"shuffle_type\":" << "\"cube\"";
		groupby_str << "," << "\"shuffle_field\":" << "\"getCubeId(" << shuffle_field_str << ")\"";
	}

	//for records without the groupby key fields
	groupby_str <<  "," << "\"append_non_existing_field\":" << "true"
				<< "," << "\"count_value\":" <<	mCountValue; 
	//arvin 2015.08.27
	if(mInfoFields.size() > 0)
	{
		groupby_str << ",\"info_fields\":["; 
		for(itr = mInfoFields.begin();itr != mInfoFields.end();itr++)
		{
			if(itr != mInfoFields.begin())
				groupby_str << ",";
			groupby_str << "\"" << itr->first << "\""; 
		}
		groupby_str << "]"; 
	}
	if(!mIsService)
		groupby_str	<< "};";
	else
		groupby_str << "}";

	OmnScreen << groupby_str << endl;
	statements_str << "\n" << groupby_str;

	if(mIsService)
	{
		jimo_prog->setConf(groupby_str);
	}
	else
	{
		bool rslt = parseRun(rdata,groupby_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}
	return true;
}


bool
AosJimoLogicStat::createStatJoinDataProc(
		AosRundata *rdata,
	    AosJimoProgObj *jimo_prog,
		OmnString &statements_str,
		u32 k)
{
	//Create DataProc <name>
	//(
	//   Type=”statjoin”,
	//   Keys=$.Keys,:
	//   Measures=$.Measures
	// );

	OmnString StatJoin_str = "";
	mJoinDataProcName = "";
	OmnString dataprocName = "";
	if(!mIsService)
	{
		StatJoin_str = "create dataprocstatjoin ";
		mJoinDataProcName << mJobName << "_dp_stat_join_"<< mTableName << "_" << mStatModelName[k];
		StatJoin_str << mJoinDataProcName << " {";
	}
	else
	{
		dataprocName << "dpStatJoin_" << mIdx;
		dataprocName << mStatModelName[k];
		mJoinDataProcName = dataprocName;
		OmnString inName = mGroupbyDataProcName;
		inName << ".output";
		jimo_prog->insertToDataFlowMap(inName, mJoinDataProcName);
		StatJoin_str << "\"" << dataprocName << "\" : {"
					<< "\"format\" :\"" << mSchemaName << "\" ,";
	}


	//yang,2015/08/22
	StatJoin_str << "\"stat_name\":\"" << mStatModelName[k] << "\",";

	if (mIsNeedShuffle)
	{
		StatJoin_str << "\"shuffle_type\":" << "\"cube\", "
			<< "\"shuffle_field\":" << "\"getCubeId(sdocid)\", ";
	}

	if(!mIsService)
		StatJoin_str << "\"type\":" << "\"statjoin\", ";
	else
	{
		StatJoin_str << "\"type\":" << "\"dataprocstatjoin\", "
				<< "\"cube_id\":" << "0, "
				<< "\"iilname\": \""
				<< "_zt44_" << mTableName << "_stat_" 
				<< mStatModelName[k] << "_internal_" << "0__0\",";
	}

	StatJoin_str << "\"record_type\":" << "\"buff\"" << ",";
	map<OmnString, OmnString>::iterator itr;
	map<OmnString, vector<OmnString> >::iterator itr_1;
	
	// keyfields
	itr_1 = mKeyFields.find(mStatModelName[k]);
	if (itr_1 == mKeyFields.end())
	{
		return false;
	}
	vector<OmnString> keyfields = itr_1->second;
	StatJoin_str << "\"keys\":" << "[" ;
	for(u32 i=0; i< keyfields.size(); i++)
	{
		if(i>0)
			StatJoin_str << ",";
		StatJoin_str << "\"" << keyfields[i] << "\"";
	}
	StatJoin_str << "],";

	if (mKeyIdxOpr.size() > 0)
	{
		OmnString opr;
		map<OmnString, OmnString>::iterator itr;
		StatJoin_str << "\"key_index_opr\":{";  
		for (itr = mKeyIdxOpr.begin();itr != mKeyIdxOpr.end() ; itr++)
		{
			if(itr != mKeyIdxOpr.begin())
				StatJoin_str << ",";
			StatJoin_str << "\"" << itr->first << "\":"
				         << "\"" << convertType(itr->second) << "\"";
		}
		StatJoin_str << "},";
	}

	// measure
	itr_1 = mMeasures.find(mStatModelName[k]);
	if (itr_1 == mMeasures.end())
	{
		    return false;
	}
	vector<OmnString> measures = itr_1->second;
	StatJoin_str << "\"measures\": " << "[";
	for(u32 i=0; i< measures.size(); i++)
	{
		if(i>0)
			StatJoin_str << ",";
		StatJoin_str << "\"" << measures[i] << "\"";
	}
	StatJoin_str << "]" ;

	// time
	StatTime time;
	map<OmnString, StatTime>::iterator itr_2 = mTime.find(mStatModelName[k]);
	if (itr_2 != mTime.end())
	{
		time = itr_2->second;
		if(time.mTimeField != "")
			StatJoin_str << ",\"time\":" << "\"" << time.mTimeField << "\"";
	}

	StatJoin_str << "," << "\"max_keylen\":"  << mMaxLen;
	//arvin
	StatJoin_str << ",\"opr\":\"" << mOpr << "\""; 	
	if(mInfoFields.size() > 0)
	{
		StatJoin_str << ",\"info_fields\":["; 
		for(itr = mInfoFields.begin();itr != mInfoFields.end();itr++)
		{
			if(itr != mInfoFields.begin())
				StatJoin_str << ",";
			StatJoin_str << "{\"field\":\"" << itr->first << "\",\"alias\":\"" << itr->second << "\"}";
		}
		StatJoin_str << "]";
	}

	if(!mIsService)
		StatJoin_str << "};";
	else
		StatJoin_str << "} ";

	OmnScreen << StatJoin_str << endl;
	statements_str << "\n" << StatJoin_str;
	
	if(mIsService)
	{
		jimo_prog->setConf(StatJoin_str);
	}
	else
	{
		bool rslt = parseRun(rdata,StatJoin_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}

	return true;
}

bool
AosJimoLogicStat::createStatDocDataProc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str,
		u32 k)
{
	//	Create DataProc <name>
	//	{
	//		Type=”statdoc”,
	//	   	Objid=”objid”
	//	};
	OmnString idx;
	OmnString StatDoc_str = "";
	OmnString dataprocName = "";
	if(!mIsService)
	{
		StatDoc_str = "create dataprocstatdoc ";
		mStatDocName = "";
		mStatDocName << mJobName << "_dp_stat_doc_" << mTableName << "_" << mStatModelName[k];
		if (mOpr == "delete")
			mStatDocName << "_del";
	}
	else
	{
		dataprocName << "dpStatDoc_" << mIdx;
		dataprocName << mStatModelName[k];
		mStatDocName = dataprocName;
		OmnString inName = mJoinDataProcName;
		inName << ".output_vt2d";
		jimo_prog->insertToDataFlowMap(inName, mStatDocName);
		StatDoc_str << "\"" << dataprocName << "\" : ";
	}

	OmnString objid ;
//	objid << mTableName << "_stat_" << mStatModelName[k] << "_internal_0";

	//if(mStatIdx != "")
	//	idx << mStatIdx;
	//else
	//	idx << k;
	
	// stat idx
	map<OmnString, OmnString>::iterator itr = mStatIdx.find(mStatModelName[k]);
	if (itr == mStatIdx.end())
	{
		return false;
	}
	idx = itr->second;
	objid << mTableName << "_stat_" << mStatModelName[k] << "_internal_" << idx;
/*	OmnString timeunit = "";
	if(mTimeUnit)
		timeunit = mTimeUnit->getValue(rdata);
*/
	//OmnString timeunit = "";
	if(!mIsService)
		StatDoc_str << mStatDocName <<" { "
			<< "\"type\":" << "\"statdoc\",";
	else
	{
		StatDoc_str <<" { "
			<< "\"type\":" << "\"dataprocstatdoc\",";
	}

	StatDoc_str	<< "\"stat_objid\":" << "\"" << objid << "\"";

	// time
	StatTime time;
	map<OmnString, StatTime>::iterator itr_2 = mTime.find(mStatModelName[k]);
	if (itr_2 != mTime.end())
	{
		time = itr_2->second;
		if (time.mTimeUnit != "")
			StatDoc_str << ",\"time_unit\":" << "\"" << time.mTimeUnit << "\"";
	}

	if (mOpr != "")
		StatDoc_str << ",\"opr\":" << "\"" << mOpr << "\"";
	else
		StatDoc_str << ",\"opr\":" << "\"insert\"";

	if(!mIsService)
		StatDoc_str << "};";
	else
		StatDoc_str << "}";

	OmnScreen << StatDoc_str <<endl;
	statements_str << "\n" << StatDoc_str;

	if(mIsService)
	{
		jimo_prog->setConf(StatDoc_str);
	}
	else
	{
		bool rslt = parseRun(rdata,StatDoc_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}

//	jimo_prog->addDataProc(rdata, "mStatDocName", StatDoc_str);
	return true;
}


bool
AosJimoLogicStat::createUpdateStatKeyTableDataProc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str,
		u32 k)
{
	//Create DataProc <name>
	//(
	//   IILName=”<iilname>”,
	//   UpdateMethod=”stradd”
	//);
	mStatKeyTableName = "";
	OmnString dataprocName = "";
	OmnString UpdateStatKeyTable_str = "";
	if(!mIsService)
	{
		UpdateStatKeyTable_str = "create dataprociilbatchopr ";
		mStatKeyTableName << mJobName << "_dp_stat_key_" << mTableName << "_" << mStatModelName[k];
	}
	else
	{
		dataprocName << "dpStatKey_" << mIdx;
		dataprocName << mStatModelName[k];
		mStatKeyTableName = dataprocName;
		OmnString inName = mJoinDataProcName;
		inName << ".output_statkey";
		jimo_prog->insertToDataFlowMap(inName, mStatKeyTableName);
		//mStatList.push_back(dataprocName);

		UpdateStatKeyTable_str << "\"" << dataprocName << "\" : ";
	}

	mKeyTableIILName = "";
	if(mIsService)
	{
		if (mIsNeedShuffle)
			mKeyTableIILName << "_zt44_" << mTableName << "_stat_" << mStatModelName[k] << "_internal_0__$group";
		else
			mKeyTableIILName << "_zt44_" << mTableName << "_stat_" << mStatModelName[k] << "_internal_0__0";

		mKeyTableIILName = "";
		mKeyTableIILName << "_zt44_" << mTableName << "_stat_" << mStatModelName[k] << "_internal_0__0";
	}
	else
	{
		if (mIsNeedShuffle)
			mKeyTableIILName << "_zt4k_" << mTableName << "_stat_" << mStatModelName[k] << "_internal_0__$group";
		else
			mKeyTableIILName << "_zt4k_" << mTableName << "_stat_" << mStatModelName[k] << "_internal_0__0";
	}
	if(!mIsService)
		UpdateStatKeyTable_str << mStatKeyTableName << " { ";
	else
	{
		UpdateStatKeyTable_str << " { "
						<< "\"type\":" << "\"dataprociilbatchopr\"" << ",";
	}
	UpdateStatKeyTable_str << "\"iilname\":" << "\"" << mKeyTableIILName << "\"" << ",";
	if (mIsNeedShuffle)
		UpdateStatKeyTable_str << "\"shuffle_type\":" << "\"cube\", ";

	// 2015/7/24
	//UpdateStatKeyTable_str << "\"opr\":" << "\"stradd\"" << ","
	UpdateStatKeyTable_str << "\"opr\":" << "\"add\"" << ","
		<< "\"iil_type\":" << "\"BigStr\"" << ","
		<< "\"build_bitmap\":" << "true" << ","
		<< "\"key_max_len\":" << "500";
	if(!mIsService)
		UpdateStatKeyTable_str << "};";
	else
		UpdateStatKeyTable_str << "}";

	OmnScreen << UpdateStatKeyTable_str << endl;
	statements_str << "\n" << UpdateStatKeyTable_str;

	if(mIsService)
	{
		jimo_prog->setConf(UpdateStatKeyTable_str);
	}
	else
	{
		bool rslt = parseRun(rdata,UpdateStatKeyTable_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}

	return true;
}



bool
AosJimoLogicStat::createUpdateIILDataProc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		//u32 k,
		vector<OmnString> &keyfields,
		u32 k, 
		u32 i,
		OmnString &statements_str)
{
	//Create DataProc <name>
	//(
	//   IILName=”<iilname>”,
	//   UpdateMethod=”stradd”
	//);

	OmnString UpdateStatIILDataProc_str = "";
	OmnString dataprocName = "";
	if(!mIsService)
		 UpdateStatIILDataProc_str = "create dataprociilbatchopr ";
	else
	{
		dataprocName << mStatModelName[k];
		dataprocName << "key_" << keyfields[i];
		OmnString inName = mJoinDataProcName;
		inName << ".output_key_" << keyfields[i];
		jimo_prog->insertToDataFlowMap(inName, dataprocName);
		UpdateStatIILDataProc_str << "\"" << dataprocName << "\" : ";
	}
	//OmnString mIILDataprocName;
	//vector<OmnString> mIILNames;
	OmnString iilDataprocname, iilname, idx;
	

	iilDataprocname << mJobName << "_dp_stat_index_" << mTableName << "_" << keyfields[i] << "_" << mStatModelName[k] ;
	mIILDataprocName.push_back(iilDataprocname);

	//if (mStatIdx == "")
	//	idx << k;
	//else
	//	idx << mStatIdx;

	map<OmnString, OmnString>::iterator itr = mStatIdx.find(mStatModelName[k]);
	if (itr == mStatIdx.end())
	{
		return false;
	}
	idx = itr->second;

	iilname << "_zt44_" << mTableName << "_stat_" << mStatModelName[k] << "_internal_" << idx  << "_key_" << keyfields[i];
	//mIILNames.push_back(iilname);

	itr = mKeyIdxOpr.find(keyfields[i]);
	OmnString opr,max_key_len,iil_type;
	if (itr != mKeyIdxOpr.end())
	{
		// 2015/7/24
		opr << "add";
		iil_type << itr->second;
		//opr << itr->second << "add";
		max_key_len = "8";

	}
	else 
	{
		opr << "add";
		iil_type << "BigStr";
		max_key_len = "500";
	}

	if(!mIsService)
		UpdateStatIILDataProc_str << iilDataprocname << " { ";
	else
	{
		UpdateStatIILDataProc_str << " { "
							<< "\"type\":" << "\"dataprociilbatchopr\"" << ",";
	}

	UpdateStatIILDataProc_str << "\"iilname\":" << "\"" << iilname << "\"" << ","
		<< "\"opr\":" << "\"" << opr << "\"" << ","
		<< "\"iil_type\":" << "\"" << iil_type << "\"" << ","
		<< "\"build_bitmap\":" << "true" << ","
		<< "\"key_max_len\":" << max_key_len;

	if(!mIsService)
		UpdateStatIILDataProc_str << "};";
	else
		UpdateStatIILDataProc_str << "}";

	OmnScreen << UpdateStatIILDataProc_str << endl;
	statements_str << "\n" << UpdateStatIILDataProc_str;

	if(mIsService)
	{
		jimo_prog->setConf(UpdateStatIILDataProc_str);
	}
	else
	{
		bool rslt = parseRun(rdata,UpdateStatIILDataProc_str,jimo_prog);
		aos_assert_rr(rslt,rdata,false);
	}

	//jimo_prog->addDataProc(rdata, "mStatKeyTableName", UpdateStatIILTask_str);
	 return true;
}


bool
AosJimoLogicStat::addGroupByTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		u32 k)
{
	//OmnString addgroupby_str = "Add Task to " ;
	OmnString addgroupby_str;
	OmnString jobname = mJobName;
	vector<OmnString> inputs;
 	bool rslt = AosJimoLogicNew::getDatasetOutput(rdata, jimo_prog, mInput->dumpByNoQuote(), inputs);
	if (!rslt)
	{
		AosLogError(rdata, true, "failed_retrieving_output")
			<< AosFN("Input Name") << mInput->dumpByNoQuote() << enderr;
		return false;
	}

	OmnString output;
	OmnString TaskName;
	TaskName << "task_stat_groupby_" << mStatModelName[k];
	output << mGroupbyDataProcName << "_output";
	//addgroupby_str << jobname;
	addgroupby_str << "{"
		<< "\"name\":" << "\"" << mTableName << "_" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" <<  "\"" << mGroupbyDataProcName << "\","
		<< "\"inputs\":[";
		
		for (u32 i =0; i<inputs.size(); i++)
		{
			if (i>0)
				addgroupby_str << ",";
			addgroupby_str << "\"" << inputs[i] << "\"";
		}
		addgroupby_str << "],\"outputs\":" << "[\"" << output << "\"" << "]}]}";
	if(jimo_prog)
	{
		jimo_prog->appendStatement(rdata, "tasks", addgroupby_str);
		jimo_prog->addTask(rdata, addgroupby_str, "");
	}

	return true;
}


bool
AosJimoLogicStat::addJoinTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		u32 k)
{
	//OmnString join_str = "Add Task to ";
	OmnString join_str;
	OmnString jobname = mJobName;
	OmnString input;
	//join_str << jobname ;
	OmnString TaskName;
	TaskName << "task_stat_join_"  << mDataProcName << "_" << mStatModelName[k];
	input << mGroupbyDataProcName << "_output" << "\"," <<  "\"" << mDatasetName;
	OmnString output;
	output << "\"" << mJoinDataProcName << "_output_statkey" << "\"" << ",";
	output << "\"" << mJoinDataProcName  << "_output_vt2d" << "\"";
	
	map <OmnString, vector<OmnString> >::iterator itr_1;
	vector<OmnString> keyfields;
	// keyfields
	itr_1 = mKeyFields.find(mStatModelName[k]);
	if (itr_1 == mKeyFields.end())
	{
		return false;
	}
	keyfields = itr_1->second;
	for(u32 i=0; i<keyfields.size(); i++)
	{
		output << ",";
		output << "\"" << mJoinDataProcName << "_output_key_" << keyfields[i] << "\"";
	}
	join_str << "{" << "\"name\":" << "\"" << mTableName << "_" << TaskName << "\"" << ","
		<< "\"dataengine_type\":" <<  "\"dataengine_join2\"" << ","
		<< "\"model_type\":" << "\"simple\"" << ","
		<< "\"dataset_splitter\":{	\"type\":\"stat_join\"},"
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mJoinDataProcName << "\"" << ","
		<< "\"inputs\":"<< "[\""<< input << "\"]" << ","
		<< "\"outputs\":" << "[" << output << "]" << "}]}";
	if(jimo_prog)
	{
		jimo_prog->appendStatement(rdata, "tasks", join_str);
		jimo_prog->addTask(rdata, join_str, "");
	}

	return true;
}


bool
AosJimoLogicStat::addVector2DTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		u32 k)
{
	//OmnString stat_str = "Add Task to ";
	OmnString stat_str;
	OmnString jobname = mJobName;
	OmnString input ;
	//stat_str << jobname << "{";
	OmnString TaskName;
	TaskName << "task_stat_doc_" << mStatModelName[k];
	input << mJoinDataProcName << "_output_vt2d";
	stat_str << " { " << "\"name\":" << "\"" << mTableName << "_" << TaskName << "\","
		<< "\"dataengine_type\":" <<  "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mStatDocName << "\"" << ","
		<< "\"inputs\":" <<"[\"" << input << "\"]" << "}]}";
	if(jimo_prog)
	{
		jimo_prog->appendStatement(rdata, "tasks", stat_str);
		jimo_prog->addTask(rdata, stat_str, "");
	}

	return true;
}


bool
AosJimoLogicStat::addKeyTableTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		u32 k)
{
	//OmnString keytable_str = "Add Task to ";
	OmnString keytable_str;
	//OmnString jobname = mJobName;
	OmnString input;
	//keytable_str << jobname << "{";
	OmnString TaskName;
	TaskName << "task_stat_key_" << mStatModelName[k];
	input << mJoinDataProcName << "_output_statkey";
	keytable_str << " { " << "\"name\":" << "\"" << mTableName << "_" << TaskName << "\","
		<< "\"dataengine_type\":" <<  "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":[{"
		<< "\"dataproc\":" << "\"" << mStatKeyTableName << "\"" << ","
		<< "\"inputs\":" <<"[\"" << input << "\"]" << "}]}";
	if(jimo_prog)
	{
		jimo_prog->appendStatement(rdata, "tasks", keytable_str);
		jimo_prog->addTask(rdata, keytable_str, "");
	}

	return true;
}


bool
AosJimoLogicStat::addIILTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		vector<OmnString> &keyfields,
		u32 k,
		u32 i)
{
	//OmnString iil_str = "Add Task to ";
	OmnString iil_str;
	OmnString jobname = mJobName;
	OmnString input;
	OmnString TaskName;
	//iil_str << jobname << "{";
	input << mJoinDataProcName << "_output_key_" << keyfields[i];
	TaskName << "task_stat_index_" << keyfields[i] << "_" << mStatModelName[k];
	iil_str << " { " << "\"name\":" << "\"" << mTableName << "_" << TaskName << "\","
		<< "\"dataengine_type\":" <<  "\"dataengine_scan2\"" << ","
		<< "\"physicalid\":" << "\"" << AosGetIdealSvrIdByCubeId(AosGetCubeId(mIILDataprocName[i])) << "\"," 
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mIILDataprocName[i] << "\"" << ","
		<< "\"inputs\":" <<"[\"" << input << "\"]" << "}]}";
	if (jimo_prog)
	{
		jimo_prog->appendStatement(rdata, "tasks", iil_str);
		jimo_prog->addTask(rdata, iil_str, "");
	}

	return true;
}


OmnString
AosJimoLogicStat::getObjType(AosRundata*)
{
	OmnNotImplementedYet;
	return "";
}

//arvin	2015.07.26
//JIMODB-72
OmnString 
AosJimoLogicStat::convertType(OmnString type)
{
	bool flag = false;
	type.toLower();
	if(type.findSubString("u64",0,flag) != -1)
	{
		return "u64";
	}
	if(type.findSubString("str",0,flag) != -1)
	{
		return "str";
	}
	//arvin 2015.07.27
	//JIMODB-117:support i64 and d64
	if(type.findSubString("i64",0,flag) != -1)
	{
		return "i64";
	}
	if(type.findSubString("d64",0,flag) != -1)
	{
		return "d64";
	}
	OmnShouldNeverComeHere;
	return "";
}
			
bool
AosJimoLogicStat::vFieldCheck(AosRundata *rdata,const vector<OmnString> &keys)
{
	mInfoFields.clear();
	OmnString errmsg;
	for(size_t  i = 0; i < keys.size();i++)
	{
		AosXmlTagPtr doc = AosJqlStatement::getDoc(rdata,JQLTypes::eDataFieldDoc,keys[i]);
		if(!doc) return false;
		AosXmlTagPtr expr_doc = doc->getFirstChild("expr");
		if(!expr_doc)
		{
			AosXmlTagPtr value_doc = doc->getFirstChild("value");
			if(value_doc)
			{	
				OmnString schema_name = value_doc->getAttrStr("zky_record_name","");
				AosXmlTagPtr schema_doc = AosJqlStatement::getDoc(rdata, JQLTypes::eDataRecordDoc, schema_name);
				if(!schema_doc)continue;
				AosXmlTagPtr datafield_docs = schema_doc->getFirstChild("datafields");
				if(!datafield_docs)continue;
				AosXmlTagPtr datafield_doc = datafield_docs->getFirstChild("datafield");
				while(datafield_doc)
				{
					OmnString name = datafield_doc->getAttrStr("zky_name");
					if(name == keys[i])
					{
						expr_doc = datafield_doc->getFirstChild("expr");
						break;
					}
					datafield_doc = datafield_docs->getNextChild();
				}
			}
		}
		if(!expr_doc)continue;
		OmnString expr_str = expr_doc->getNodeText();
		expr_str << ";";
		AosExprObjPtr expr = AosParseExpr(expr_str,errmsg,rdata);
		if(expr->getType()!= AosExprType::eGenFunc)continue;
		OmnString funcName = expr->getFuctName();
		if(funcName != "iilmap")continue;
		if(expr)
		{
			vector<AosExprObjPtr> exprV;
			AosExprList *exprList = expr->getParmList();
			exprV = *exprList;
			for(size_t  j = 1; j < exprV.size(); j++)
			{
				OmnString key = exprV[j]->dumpByNoEscape();
				if(count(keys.begin(),keys.end(),key))
				{
					mInfoFields[keys[i]] = expr->dumpByNoEscape();
				}
			}
		}
	}
	return true;
}
