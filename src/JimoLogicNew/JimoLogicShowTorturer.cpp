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
// A Indexistics Task is defined as:
// 	Create Indexistics stat-name
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
#include "JimoLogicNew/JimoLogicShowTorturer.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoParserAPI.h"
#include "Util/File.h"

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosJimoLogicShowTorturer_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicShowTorturer(version);
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


AosJimoLogicShowTorturer::AosJimoLogicShowTorturer(const int version)
:
AosJimoLogicNew(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicShowTorturer::~AosJimoLogicShowTorturer()
{
}

AosJimoPtr
AosJimoLogicShowTorturer::cloneJimo() const
{
	return OmnNew AosJimoLogicShowTorturer(*this);
}


bool
AosJimoLogicShowTorturer::parseJQL(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		AosJimoProgObj *prog,
		bool &parsed,
		bool dft)
{
	// The statement is:
	// 	Create Indexistics <stat-name>
	// 	(
	// 		name-value-list
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords[0] == "torturer", rdata, false);
	aos_assert_rr(mKeywords[1] == "show", rdata, false);

	mTorturerName = jimo_parser->nextObjName(rdata);
	if (mTorturerName == "")
	{
		// This is not what it expects.
		mErrmsg << "missing Torturer Name or unexpected Torturer Name!";		
		setErrMsg(rdata, eGenericError,"",mErrmsg);
		return false;
	}

	mTorturerType = eErrTorturer;
	if(mTorturerName == "iiltorturer") mTorturerType = eIILTorturer;
	if(mTorturerName == "sorttorturer") mTorturerType = eSortTorturer;
	if(mTorturerName == "multicondquery") mTorturerType = eMultiCondTorturer;
	if(mTorturerName == "bitmaptorturer") mTorturerType = eBitmapTorturer;
	
	if(mTorturerType == eErrTorturer)
	{
		 mErrmsg << "wrong torturer name";
		 setErrMsg(rdata, eGenericError,"",mErrmsg);
		 return false;
	}

	bool rslt = jimo_parser->getNameValueList(rdata, mNameValueList);

	// Parse keys
    mNumRound = jimo_parser->getParmInt(rdata, "round", mNameValueList);

	mShowOthers = false;
	if (mNumRound == -1)
	{
		mNumRound = 1;
		mShowOthers = true;
	}
	mShowMsg << "Show Round Number: " << mNumRound << "\n";
	
	if(!checkNameValueList(rdata, mErrmsg, mNameValueList))
	{
		return false;
	}

	parsed = true;
	return true;
}


bool 
AosJimoLogicShowTorturer::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	system("mkdir /tmp/torturer/");

	OmnString msg = "";
	OmnString show_filestr = "/tmp/torturer/";
	OmnString not_running_str = "";
	
	switch(mTorturerType)
	{
		case eSortTorturer:
			show_filestr << "sorttorturer_states_file";
			not_running_str = "sorttorturer";
			break;
			
		case eIILTorturer:
			show_filestr << "iiltorturer_states_file";
			not_running_str = "iiltorturer";
			break;
			
		case eMultiCondTorturer:
			show_filestr << "multicondquery_states_file";
			not_running_str = "multicondquery";
			break;
			
		case eBitmapTorturer:
			show_filestr << "bitmaptorturer_states_file";
			not_running_str = "bitmaptorturer";
			break;
			
		default:
			msg << "Wrong torturer name or order number\n";
			rdata->setJqlMsg(msg);
			return true;
	}
	
	OmnString statues_contents = "";
	OmnFile status_file(show_filestr, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if(!status_file.isGood())
	{
		not_running_str << " is not running";
		rdata->setJqlMsg(not_running_str);
	}
	else
	{
		if(!mShowOthers)
		{
			//show round msg
			status_file.openFile(OmnFile::eReadOnly);
			bool rslt = status_file.readAll(statues_contents);
			aos_assert_r(rslt, false);
			i64 end_pos = statues_contents.length()-1;
			i64 start_pos = end_pos;
			i64 tmp_start_pos;
			while(mNumRound)
			{
				tmp_start_pos = start_pos;
				start_pos = statues_contents.indexOf(0, '\n', true);
				if(start_pos==0 || start_pos==-1)
				{
					break;
				}
				if(tmp_start_pos == start_pos+1)
				{
					mNumRound--;
				}
				statues_contents.setLength(start_pos);
			}
			rslt = status_file.seek(start_pos);
			aos_assert_r(rslt, false);
			rslt = status_file.readToString(statues_contents);
			aos_assert_r(rslt, false);
		}
		else
		{
			//show memory msg
				
		}
		rdata->setJqlMsg(statues_contents);
	}


	return true;
}

bool
AosJimoLogicShowTorturer::parseRun(
		AosRundata* rdata,
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
	return true;
}


bool
AosJimoLogicShowTorturer::createIndexDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
    mJobName = jimo_prog->getJobname();
    OmnString schemaname = jimo_prog->getEnv("schema_name");
	OmnString dataprocIndex_str = "create dataprocindex ";
	mDataprocIndex << mJobName << "_dp_index_" << mTableName << "_" << mIndexName;
	dataprocIndex_str << mDataprocIndex << " { "
		<< "\"type\":" << "\"" << "index" << "\"" << ","
        << "\"record_type\":" << "\"buff\"";
	if(mCond != "")
		dataprocIndex_str << "," << "\"condition\":" << "\"" << mCond << "\"";
	dataprocIndex_str << "," << "\"keys\":" << "[" ;
	for(u32 i=0; i< mKeyFields.size(); i++)
	{
		if(i>0)
			dataprocIndex_str << ",";
		dataprocIndex_str << "\"" << mKeyFields[i] << "\"";
	}
	dataprocIndex_str << "],";
	dataprocIndex_str << "\"max_keylen\":" << "100,"
        << "\"docid\":" << "\"" << "getDocid('" << schemaname << "')" << "\"";
	if(mCond != "")
		dataprocIndex_str << "," << "\"condition\":" << "\"" << mCond << "\"";
	dataprocIndex_str << "};";

	OmnScreen << dataprocIndex_str << endl;
	statements_str << "\n" << dataprocIndex_str;

    bool rslt = parseRun(rdata,dataprocIndex_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}


bool
AosJimoLogicShowTorturer::createIndexU64(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
    mJobName = jimo_prog->getJobname();
    OmnString schemaname = jimo_prog->getEnv("schema_name");
	OmnString dataprocindex_str = "create dataprocindexu64 ";
	mDataprocIndex << mJobName << "_dp_index_" << mTableName << "_" << mIndexName;
	dataprocindex_str << mDataprocIndex << " { "
		<< "\"type\":" << "\"indexu64\"" << ","
		<< "\"key\":" << "\"" << mKeyFields[0] << "\", ";
	if(mCond != "")
	{
		dataprocindex_str << "," << "\"condition\":" << "\"" << mCond << "\"";
	}
	dataprocindex_str << "\"docid\":" << "\"" << "getDocid('" << schemaname << "')" << "\""
		<< "};";

	OmnScreen << dataprocindex_str << endl;
	statements_str << "\n" << dataprocindex_str;

	bool rslt = parseRun(rdata,dataprocindex_str,jimo_prog);
	aos_assert_rr(rslt,rdata,false);
	return true;

}

bool
AosJimoLogicShowTorturer::createIILDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{

	OmnString dataprociil_str = "create dataprociilbatchopr ";
	OmnString mIILName;
	mIndexIILName << mJobName << "_dp_index_iil_" << mTableName << "_" << mIndexName;
	mIILName << "_zt44_idx_" << mTableName << "_" << mIndexName;
	//for(u32 i=0; i<mKeyFields.size(); i++)
//	{
		//mIILName << "_" << mKeyFields[0];
//	}
	dataprociil_str << mIndexIILName << " { "
		<< "\"iilname\":" << "\"" << mIILName << "\"" << ",";
	if (mType == "indexu64")
		dataprociil_str << "\"opr\":" << "\"u64add\"" << "};";
	else
		dataprociil_str << "\"opr\":" << "\"stradd\"" << "};";

	OmnScreen << dataprociil_str << endl;
	statements_str << "\n" << dataprociil_str;

    bool rslt = parseRun(rdata,dataprociil_str,jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	return true;
}

bool
AosJimoLogicShowTorturer::addIndexTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{

	OmnString addindex_str;
 	OmnString input = mInput->dumpByNoQuote();
	OmnString output;
	OmnString TaskName;
	TaskName << "task_index_" << mTableName << "_" << mIndexName;
	output << mDataprocIndex << "_output";
	//addgroupby_str << jobname;
	addindex_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" <<  "\"" << mDataprocIndex << "\","
		<< "\"inputs\":" << "[\"" << input << "\"],"
		<< "\"outputs\":" << "[\"" << output << "\"" << "]}]}";
	jimo_prog->addTask(rdata, addindex_str, "");
//	mTasks.push_back(addindex_str);
//	OmnScreen << addindex_str << endl;
//  bool rslt = parseRun(rdata,addindex_str,jimo_prog);
//  aos_assert_rr(rslt,rdata,false);
	return true;
}

/*
bool
AosJimoLogicShowTorturer::addIndexU64Task(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{
	OmnString addindex_str;
 	OmnString input = mInput->dumpByNoQuote();
	OmnString output;
	OmnString TaskName;
	TaskName << "task_index_" << mTableName << "_" << mIndexName;
	output << mDataprocU64Index << "_output";
	//addgroupby_str << jobname;
	addindex_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" <<  "\"" << mDataprocU64Index << "\","
		<< "\"inputs\":" << "[\"" << input << "\"],"
		<< "\"outputs\":" << "[\"" << output << "\"" << "]}]}";
	jimo_prog->addTask(rdata, addindex_str, "");
//	mTasks.push_back(addindex_str);
//	OmnScreen << addindex_str << endl;
//  bool rslt = parseRun(rdata,addindex_str,jimo_prog);
//  aos_assert_rr(rslt,rdata,false);
	return true;
}
*/

bool
AosJimoLogicShowTorturer::addIILTask(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog)
{
	OmnString addiil_str;
 	OmnString input;
	OmnString output;
	OmnString TaskName;
	TaskName << "task_index_iil_" << mTableName << "_" << mIndexName;
	input << mDataprocIndex << "_output";
	//addgroupby_str << jobname;
	addiil_str << " {"
		<< "\"name\":" << "\"" << TaskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mIndexIILName << "\","
		<< "\"inputs\":" << "[\"" << input << "\"" << "]}]}";
	jimo_prog->addTask(rdata, addiil_str, "");
	//OmnScreen << addiil_str << endl;
	return true;
}


